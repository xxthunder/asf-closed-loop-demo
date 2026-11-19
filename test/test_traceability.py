import json
from pathlib import Path

import pytest
from traceability import schema_violations

pytestmark = [
    pytest.mark.reports,
    pytest.mark.gate_develop_pr,
    pytest.mark.gate_develop_push,
    pytest.mark.gate_develop_nightly,
    pytest.mark.gate_release_pr,
    pytest.mark.gate_release,
]


def write_report(variant: str, warnings: dict) -> None:
    report = f"build/{variant}/test/Debug/reports/html/schema_violations.json"
    path = Path(report)
    path.parent.mkdir(parents=True)
    path.write_text(json.dumps({"validation_warnings": warnings}))


def test_no_violations(tmp_path, monkeypatch):
    monkeypatch.chdir(tmp_path)
    write_report("Sleep", {})
    assert schema_violations("Sleep") == {}


def test_returns_the_violations(tmp_path, monkeypatch):
    monkeypatch.chdir(tmp_path)
    warnings = {"SWIMPL_LC-002": [{"subtype": "network_missing_target"}]}
    write_report("Sleep", warnings)
    assert schema_violations("Sleep") == warnings


def test_variant_with_slash(tmp_path, monkeypatch):
    monkeypatch.chdir(tmp_path)
    warnings = {"TS_LC-001": [{"subtype": "local_fail"}]}
    write_report("Base/Dev", warnings)
    assert schema_violations("Base/Dev") == warnings


def test_missing_report_names_path(tmp_path, monkeypatch):
    monkeypatch.chdir(tmp_path)
    with pytest.raises(AssertionError, match="schema_violations.json"):
        schema_violations("Sleep")
