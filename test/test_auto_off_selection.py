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

REPO_ROOT = Path(__file__).resolve().parent.parent

# variant -> AUTO_OFF_TIMEOUT in seconds, None where the feature is not selected
VARIANTS = {
    "Sleep": "10",
    "IDEA/Sloemada": "10",
    "Disco": None,
    "Spa": None,
}


def read_lines(variant: str, name: str) -> list[str]:
    return (REPO_ROOT / "variants" / variant / name).read_text().splitlines()


@pytest.mark.parametrize(("variant", "timeout"), VARIANTS.items())
def test_auto_off_selection(variant: str, timeout: str | None) -> None:
    config = read_lines(variant, "config.txt")
    parts = read_lines(variant, "parts.cmake")
    component_selected = "spl_add_component(components/auto_off)" in [line.strip() for line in parts]

    if timeout is not None:
        assert "CONFIG_AUTO_OFF=y" in config
        assert f"CONFIG_AUTO_OFF_TIMEOUT={timeout}" in config
        assert component_selected
    else:
        assert "# CONFIG_AUTO_OFF is not set" in config
        assert not [line for line in config if line.startswith("CONFIG_AUTO_OFF_TIMEOUT")]
        assert not component_selected
