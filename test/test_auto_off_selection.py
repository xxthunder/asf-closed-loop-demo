"""Auto Off is a selectable feature: only some variants select it and compile its component."""

from pathlib import Path

import pytest

REPO_ROOT = Path(__file__).resolve().parent.parent

pytestmark = [
    pytest.mark.gate_develop_pr,
    pytest.mark.gate_develop_push,
    pytest.mark.gate_develop_nightly,
    pytest.mark.gate_release_pr,
    pytest.mark.gate_release,
]


def _lines(path: Path) -> list[str]:
    return [line.strip() for line in path.read_text().splitlines()]


@pytest.mark.parametrize("variant", ["Sleep", "IDEA/Sloemada"])
def test_variant_selects_auto_off(variant: str) -> None:
    config = _lines(REPO_ROOT / "variants" / variant / "config.txt")
    parts = _lines(REPO_ROOT / "variants" / variant / "parts.cmake")

    assert "CONFIG_AUTO_OFF=y" in config
    assert "CONFIG_AUTO_OFF_TIMEOUT=10" in config
    assert "spl_add_component(components/auto_off)" in parts


@pytest.mark.parametrize("variant", ["Disco", "Spa"])
def test_variant_does_not_select_auto_off(variant: str) -> None:
    config = _lines(REPO_ROOT / "variants" / variant / "config.txt")
    parts = (REPO_ROOT / "variants" / variant / "parts.cmake").read_text()

    assert "# CONFIG_AUTO_OFF is not set" in config
    assert "components/auto_off" not in parts
