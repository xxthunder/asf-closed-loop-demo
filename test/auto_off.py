"""Checks that the Auto Off feature is selected, or absent, in a variant's test/Debug reports build."""

import json
from pathlib import Path


def assert_auto_off(build_dir: Path, timeout: int | None) -> None:
    """Check the Auto Off selection of a variant.

    timeout an int: component auto_off selected, AUTO_OFF set, AUTO_OFF_TIMEOUT equal to timeout.
    timeout None: component auto_off not selected, AUTO_OFF not set.
    """
    config = json.loads((build_dir / "reports/config.json").read_text(encoding="utf-8"))
    features = json.loads((build_dir / "kconfig/autoconf.json").read_text(encoding="utf-8"))["features"]
    selected = {c["name"] for c in config["components_info"]}
    if timeout is None:
        assert "components_auto_off" not in selected, f"{build_dir}: components_auto_off is selected"
        assert not features.get("AUTO_OFF"), f"{build_dir}: AUTO_OFF is set: {features.get('AUTO_OFF')!r}"
        assert "AUTO_OFF_TIMEOUT" not in features, f"{build_dir}: AUTO_OFF_TIMEOUT is set: {features['AUTO_OFF_TIMEOUT']!r}"
    else:
        assert "components_auto_off" in selected, f"{build_dir}: components_auto_off is not selected"
        assert features.get("AUTO_OFF") is True, f"{build_dir}: AUTO_OFF is not set: {features.get('AUTO_OFF')!r}"
        assert features.get("AUTO_OFF_TIMEOUT") == timeout, f"{build_dir}: AUTO_OFF_TIMEOUT is {features.get('AUTO_OFF_TIMEOUT')!r}, expected {timeout}"
