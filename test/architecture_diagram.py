"""Checks of the Mermaid class diagram on the software architecture page."""

import html
import json
import re
from pathlib import Path

_MERMAID_PRE = re.compile(r'<pre\b([^>]*?\bclass="mermaid[^"]*"[^>]*)>(.*?)</pre>', re.DOTALL)
_MEMBER = re.compile(r"^\+([A-Za-z0-9_]+)\(\)$")
_ANNOTATION = re.compile(r"^<<[^>]+>>$")
_CLASS = re.compile(r'^\s*class\s+([a-z][a-z0-9_]*)\["[^"]+"\]\s*(\{\s*)?$')
_RELATION = re.compile(r"^\s*([a-z][a-z0-9_]*)\s*\.\.>\s*([a-z][a-z0-9_]*)(?:\s*:\s*(.+?))?\s*$")
_NOT_PRODUCT = ("platform_types", "examples_", "test_")
_PERIOD_MEMBERS = ("GetBrightnessAdjustmentCounter()", "SetBrightnessAdjustmentCounter()")

# (caller, call, feature, shown when the feature is set)
KCONFIG_CALLS = [
    ("light_controller", "GetMainKnobValue", "BLINKING", True),
    ("light_controller", "GetBrightnessValue", "BRIGHTNESS_ADJUSTMENT_ENABLED", True),
    ("light_controller", "GetBrightnessAdjustmentCounter", "BRIGHTNESS_ADJUSTMENT_PERIOD", True),
    ("brightness_controller", "SetBrightnessAdjustmentCounter", "BRIGHTNESS_ADJUSTMENT_AUTOMATIC", True),
    ("brightness_controller", "GetMainKnobValue", "BRIGHTNESS_ADJUSTMENT_AUTOMATIC", False),
]


def _declared_classes(source: str) -> set[str]:
    return {m.group(1) for m in map(_CLASS.match, source.splitlines()) if m}


def _relations(source: str) -> list[tuple[str, str, list[str]]]:
    relations = []
    for line in source.splitlines():
        match = _RELATION.match(line)
        if match:
            calls = [call.strip() for call in (match.group(3) or "").split(",") if call.strip()]
            relations.append((match.group(1), match.group(2), calls))
    return relations


def rte_calls_keyboard_interface(source: str) -> bool:
    """Return True when a relation rte ..> keyboard_interface names IsKeyPressed."""
    return any(src == "rte" and dst == "keyboard_interface" and any("IsKeyPressed" in c for c in calls) for src, dst, calls in _relations(source))


def _class_body(source: str, class_id: str) -> list[str]:
    """Return the member lines of the class body of class_id (empty without a body)."""
    body: list[str] = []
    inside = False
    for line in source.splitlines():
        match = _CLASS.match(line)
        if inside:
            if line.strip() == "}":
                break
            body.append(line.strip())
        elif match and match.group(1) == class_id and match.group(2):
            inside = True
    return body


def relations_join_declared_classes(source: str) -> list[str]:
    """Return the relation lines that name a class the diagram does not declare."""
    declared = _declared_classes(source)
    offending = []
    for line in source.splitlines():
        match = _RELATION.match(line)
        if match and not {match.group(1), match.group(2)} <= declared:
            offending.append(line.strip())
    return offending


def assert_architecture_diagram(build_dir: Path) -> None:
    """Assert that the architecture page of the variant shows its own Mermaid class diagram.

    build_dir is the build directory of the test/Debug reports build of one variant.
    """
    page_path = build_dir / "reports/html/doc/software_architecture/index.html"
    config_path = build_dir / "reports/config.json"
    autoconf_path = build_dir / "kconfig/autoconf.json"
    page = page_path.read_text(encoding="utf-8")

    match = _MERMAID_PRE.search(page)
    assert match, f"{build_dir}: the architecture page holds no Mermaid block"
    assert "data-zoom-id=" in match.group(1), f"{build_dir}: the Mermaid block carries no data-zoom-id attribute (the :zoom: option is missing)"
    source = html.unescape(match.group(2))
    lines = [line for line in source.splitlines() if line.strip()]
    assert lines and lines[0].strip() == "classDiagram", f"{build_dir}: the first line of the Mermaid block is not 'classDiagram'"

    need_pos = page.find("SWARCH_001")
    assert need_pos != -1 and match.start() > need_pos, f"{build_dir}: the Mermaid block does not appear after SWARCH_001 (it is not inside the need)"

    assert "sw_arch_component_diagram" not in page, f"{build_dir}: the page still references sw_arch_component_diagram"

    config = json.loads(config_path.read_text(encoding="utf-8"))
    selected = {c["name"].removeprefix("components_") for c in config["components_info"]}
    expected = {name for name in selected if not name.startswith(_NOT_PRODUCT)}
    declared = _declared_classes(source)
    assert "rte" in declared, f"{build_dir}: the diagram declares no 'rte' class (declared: {sorted(declared)})"
    assert declared == expected, f"{build_dir}: declared classes differ from the selected product components: missing {sorted(expected - declared)}, extra {sorted(declared - expected)}"

    offending = relations_join_declared_classes(source)
    assert not offending, f"{build_dir}: relations with an undeclared class: {offending}"

    if {"rte", "keyboard_interface"} <= declared:
        assert rte_calls_keyboard_interface(source), f"{build_dir}: no relation 'rte ..> keyboard_interface' with IsKeyPressed"

    features = json.loads(autoconf_path.read_text(encoding="utf-8"))["features"]
    relations = _relations(source)
    for caller, call, feature, shown_when_set in KCONFIG_CALLS:
        if caller not in declared:
            continue
        is_set = bool(features.get(feature))
        calls = {c for src, dst, cs in relations if src == caller and dst == "rte" for c in cs}
        should_show = is_set == shown_when_set
        assert (call in calls) == should_show, f"{build_dir}: {caller} {'misses' if should_show else 'shows'} {call} with {feature} {'set' if is_set else 'not set'} (calls to rte: {sorted(calls)})"

    members = "\n".join(_class_body(source, "rte"))
    period_set = bool(features.get("BRIGHTNESS_ADJUSTMENT_PERIOD"))
    for member in _PERIOD_MEMBERS:
        assert (member in members) == period_set, f"{build_dir}: rte class body {'misses' if period_set else 'shows'} {member} with BRIGHTNESS_ADJUSTMENT_PERIOD {'set' if period_set else 'not set'}"

    called: dict[str, set[str]] = {name: set() for name in declared}
    for _, dst, calls in relations:
        called[dst].update(calls)
    for name in sorted(declared):
        body = _class_body(source, name)
        stray = [line for line in body if line and not _ANNOTATION.match(line) and not _MEMBER.match(line)]
        assert not stray, f"{build_dir}: class {name} has body lines that are neither an annotation nor a member '+Name()': {stray}"
        member_names = [m.group(1) for m in map(_MEMBER.match, body) if m]
        duplicates = sorted({n for n in member_names if member_names.count(n) > 1})
        assert not duplicates, f"{build_dir}: class {name} lists members twice: {duplicates}"
        listed = set(member_names)
        assert listed == called[name], f"{build_dir}: class {name} lists members {sorted(listed)} but its incoming calls are {sorted(called[name])}"
