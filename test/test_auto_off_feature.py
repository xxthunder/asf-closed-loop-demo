"""The feature model of Auto Off: the KConfig symbols and what each variant selects."""

import os
from importlib.resources import files
from pathlib import Path

import kconfiglib
import pytest

pytestmark = [
    pytest.mark.reports,
    pytest.mark.gate_develop_pr,
    pytest.mark.gate_develop_push,
    pytest.mark.gate_develop_nightly,
    pytest.mark.gate_release_pr,
    pytest.mark.gate_release,
]

REPO = Path(__file__).parent.parent


def load_model(variant: str | None = None) -> kconfiglib.Kconfig:
    """The repository's KConfig, with the variant's config.txt loaded when one is named.

    The model sources $(SPL_CORE_DIR)/config/KConfig and the component models by relative path,
    so SPL_CORE_DIR points at the installed spl_core package and the working directory is the repository root.
    """
    os.environ["SPL_CORE_DIR"] = str(files("spl_core").joinpath(""))
    previous = Path.cwd()
    os.chdir(REPO)
    try:
        kconf = kconfiglib.Kconfig(str(REPO / "KConfig"), warn=False)
        if variant:
            kconf.load_config(str(REPO / "variants" / variant / "config.txt"))
    finally:
        os.chdir(previous)
    return kconf


def symbol(kconf: kconfiglib.Kconfig, name: str) -> kconfiglib.Symbol:
    sym = kconf.syms.get(name)
    assert sym is not None, f"the feature model does not define {name}"
    return sym


def test_timeout_range():
    kconf = load_model()
    auto_off = symbol(kconf, "AUTO_OFF")
    timeout = symbol(kconf, "AUTO_OFF_TIMEOUT")

    assert auto_off.orig_type == kconfiglib.BOOL
    assert timeout.orig_type == kconfiglib.INT
    assert auto_off in kconfiglib.expr_items(timeout.direct_dep), "AUTO_OFF_TIMEOUT must depend on AUTO_OFF"
    assert [(low.str_value, high.str_value) for low, high, _ in timeout.ranges] == [("5", "7200")]


@pytest.mark.parametrize("variant", ["Sleep", "IDEA/Sloemada"])
def test_variant_selection_with_auto_off(variant):
    kconf = load_model(variant)

    assert symbol(kconf, "AUTO_OFF").str_value == "y"
    assert symbol(kconf, "AUTO_OFF_TIMEOUT").str_value == "10"


@pytest.mark.parametrize("variant", ["Disco", "Spa"])
def test_variant_selection_without_auto_off(variant):
    kconf = load_model(variant)
    timeout = symbol(kconf, "AUTO_OFF_TIMEOUT")

    assert symbol(kconf, "AUTO_OFF").str_value == "n"
    assert timeout.visibility == 0, "AUTO_OFF_TIMEOUT must not be visible without AUTO_OFF"
    assert timeout.str_value == "", "AUTO_OFF_TIMEOUT must have no value without AUTO_OFF"
