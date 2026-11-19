from pathlib import Path

WARNINGS_FILE = "reports-sphinx-warnings.log"


def assert_no_sphinx_warnings(build_dir: Path) -> None:
    warnings_file = build_dir / WARNINGS_FILE
    assert warnings_file.is_file(), f"Sphinx wrote no warnings file: {warnings_file}"
    warnings = [line for line in warnings_file.read_text(encoding="utf-8").splitlines() if line.strip()]
    assert not warnings, f"{len(warnings)} Sphinx warning(s) in {warnings_file}:\n" + "\n".join(warnings)
