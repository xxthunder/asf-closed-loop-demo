import json
from pathlib import Path

from spl_core.test_utils.spl_build import SplBuild


def schema_violations(variant: str) -> dict[str, list[dict]]:
    """Return the schema violations of a variant's test report, keyed by need ID."""
    build_dir = SplBuild(variant=variant, build_kit="test", build_type="Debug", target="reports").build_dir
    report = Path(build_dir) / "reports" / "html" / "schema_violations.json"
    assert report.is_file(), f"Schema violations report not found: {report}"
    return json.loads(report.read_text(encoding="utf-8"))["validation_warnings"]
