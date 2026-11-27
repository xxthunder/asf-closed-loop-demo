"""Which product variants select the auto_off component, read from the variant files."""

from pathlib import Path

import pytest

pytestmark = [
    pytest.mark.reports,
    pytest.mark.gate_develop_pr,
    pytest.mark.gate_develop_push,
    pytest.mark.gate_develop_nightly,
    pytest.mark.gate_release_pr,
    pytest.mark.gate_release,
]

VARIANTS = Path(__file__).parent.parent / "variants"


def read_lines(variant: str, name: str) -> list[str]:
    return (VARIANTS / variant / name).read_text(encoding="utf-8").splitlines()


@pytest.mark.parametrize("variant", ["Sleep", "IDEA/Sloemada"])
def test_auto_off_selected(variant: str):
    config = read_lines(variant, "config.txt")
    parts = (VARIANTS / variant / "parts.cmake").read_text(encoding="utf-8")

    assert "CONFIG_AUTO_OFF=y" in config
    assert "CONFIG_AUTO_OFF_TIMEOUT=10" in config
    assert "spl_add_component(components/auto_off)" in parts


@pytest.mark.parametrize("variant", ["Disco", "Spa"])
def test_auto_off_not_selected(variant: str):
    config = read_lines(variant, "config.txt")
    parts = (VARIANTS / variant / "parts.cmake").read_text(encoding="utf-8")

    assert "# CONFIG_AUTO_OFF is not set" in config
    assert not any(line.startswith("CONFIG_AUTO_OFF_TIMEOUT") for line in config)
    assert "components/auto_off" not in parts
