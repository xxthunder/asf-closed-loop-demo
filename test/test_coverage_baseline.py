import json
import shutil

import coverage_baseline
import pytest
from coverage_baseline import BASELINE_FILE, METRICS, assert_coverage_not_dropped, coverage_drops

pytestmark = [
    pytest.mark.reports,
    pytest.mark.gate_develop_pr,
    pytest.mark.gate_develop_push,
    pytest.mark.gate_develop_nightly,
    pytest.mark.gate_release_pr,
    pytest.mark.gate_release,
]


def write_summary(tmp_path, summary: dict) -> None:
    (tmp_path / "variant-coverage.json").write_text(json.dumps(summary), encoding="utf-8")


def sleep_baseline() -> dict:
    return json.loads(BASELINE_FILE.read_text(encoding="utf-8"))[coverage_baseline.PLATFORM]["Sleep"]


def test_no_drop_when_equal():
    b = {m: 90.0 for m in METRICS}
    assert coverage_drops(b, b) == []


def test_no_drop_when_higher():
    baseline = {m: 90.0 for m in METRICS}
    summary = {m: 90.1 for m in METRICS}
    assert coverage_drops(summary, baseline) == []


def test_lists_every_dropped_metric():
    baseline = {m: 90.0 for m in METRICS}
    summary = {"line_percent": 89.9, "function_percent": 90.0, "branch_percent": 90.0, "condition_percent": 80.0}
    assert coverage_drops(summary, baseline) == ["line_percent: 89.9 < 90.0", "condition_percent: 80.0 < 90.0"]


def test_baseline_covers_every_platform_variant_and_metric():
    baseline = json.loads(BASELINE_FILE.read_text(encoding="utf-8"))
    assert set(baseline) == {"linux", "windows"}
    for platform_name, section in baseline.items():
        assert set(section) == {"Base/Dev", "Disco", "IDEA/Sloemada", "Sleep", "Spa"}, platform_name
        for variant, entry in section.items():
            assert set(entry) == set(METRICS), (platform_name, variant)
            assert all(isinstance(v, (int, float)) and not isinstance(v, bool) for v in entry.values()), (platform_name, variant)


def test_passes_at_the_baseline(tmp_path):
    write_summary(tmp_path, sleep_baseline())
    assert_coverage_not_dropped("Sleep", tmp_path)


def test_fails_below_the_baseline(tmp_path):
    summary = sleep_baseline()
    summary["line_percent"] = round(summary["line_percent"] - 0.1, 1)
    write_summary(tmp_path, summary)
    with pytest.raises(AssertionError) as error:
        assert_coverage_not_dropped("Sleep", tmp_path)
    assert "Sleep" in str(error.value)
    assert "line_percent" in str(error.value)


def test_fails_for_a_variant_without_baseline(tmp_path):
    write_summary(tmp_path, {m: 90.0 for m in METRICS})
    with pytest.raises(AssertionError) as error:
        assert_coverage_not_dropped("Nope", tmp_path)
    assert "Nope" in str(error.value)


def test_uses_the_values_of_the_current_platform(tmp_path, baseline_copy, monkeypatch):
    data = json.loads(baseline_copy.read_text(encoding="utf-8"))
    data["windows"]["Sleep"]["line_percent"] = 99.9
    baseline_copy.write_text(json.dumps(data, indent=4) + "\n", encoding="utf-8")
    write_summary(tmp_path, data["linux"]["Sleep"])
    monkeypatch.setattr(coverage_baseline, "PLATFORM", "linux")
    assert_coverage_not_dropped("Sleep", tmp_path)
    monkeypatch.setattr(coverage_baseline, "PLATFORM", "windows")
    with pytest.raises(AssertionError) as error:
        assert_coverage_not_dropped("Sleep", tmp_path)
    for text in ("Sleep", "windows", "line_percent"):
        assert text in str(error.value)


def test_fails_for_a_platform_without_baseline(tmp_path, monkeypatch):
    write_summary(tmp_path, {m: 90.0 for m in METRICS})
    monkeypatch.setattr(coverage_baseline, "PLATFORM", "darwin")
    with pytest.raises(AssertionError) as error:
        assert_coverage_not_dropped("Sleep", tmp_path)
    assert "darwin" in str(error.value)


@pytest.fixture
def baseline_copy(tmp_path, monkeypatch):
    copy = tmp_path / "baseline" / "coverage_baseline.json"
    copy.parent.mkdir()
    shutil.copy(BASELINE_FILE, copy)
    monkeypatch.setattr(coverage_baseline, "BASELINE_FILE", copy)
    return copy


def summary_dir(tmp_path, summary: dict):
    directory = tmp_path / "build"
    directory.mkdir()
    write_summary(directory, summary)
    return directory


def test_raise_baseline_takes_the_higher_values(tmp_path, baseline_copy):
    before = json.loads(baseline_copy.read_text(encoding="utf-8"))
    plat = coverage_baseline.PLATFORM
    summary = dict(before[plat]["Sleep"])
    summary["line_percent"] = round(summary["line_percent"] + 0.5, 1)
    summary["branch_percent"] = round(summary["branch_percent"] - 0.5, 1)
    coverage_baseline.raise_baseline("Sleep", summary_dir(tmp_path, summary))
    after = json.loads(baseline_copy.read_text(encoding="utf-8"))
    assert after[plat]["Sleep"]["line_percent"] == summary["line_percent"]
    for metric in ("function_percent", "branch_percent", "condition_percent"):
        assert after[plat]["Sleep"][metric] == before[plat]["Sleep"][metric]
    for variant in before[plat]:
        if variant != "Sleep":
            assert after[plat][variant] == before[plat][variant]
    for other in before:
        if other != plat:
            assert after[other] == before[other]


def test_raise_baseline_never_lowers(tmp_path, baseline_copy):
    before_bytes = baseline_copy.read_bytes()
    lower = {metric: round(value - 1.0, 1) for metric, value in json.loads(before_bytes)[coverage_baseline.PLATFORM]["Sleep"].items()}
    coverage_baseline.raise_baseline("Sleep", summary_dir(tmp_path, lower))
    assert baseline_copy.read_bytes() == before_bytes


def test_raise_baseline_keeps_the_file_format(tmp_path, baseline_copy):
    expected = json.loads(baseline_copy.read_text(encoding="utf-8"))
    plat = coverage_baseline.PLATFORM
    summary = dict(expected[plat]["Sleep"])
    summary["line_percent"] = round(summary["line_percent"] + 0.5, 1)
    expected[plat]["Sleep"]["line_percent"] = summary["line_percent"]
    coverage_baseline.raise_baseline("Sleep", summary_dir(tmp_path, summary))
    assert baseline_copy.read_text(encoding="utf-8") == json.dumps(expected, indent=4) + "\n"


def test_raise_baseline_adds_a_missing_entry(tmp_path, baseline_copy, monkeypatch):
    before = json.loads(baseline_copy.read_text(encoding="utf-8"))
    summary = {metric: 91.1 for metric in METRICS}
    monkeypatch.setattr(coverage_baseline, "PLATFORM", "darwin")
    coverage_baseline.raise_baseline("Sleep", summary_dir(tmp_path, summary))
    after = json.loads(baseline_copy.read_text(encoding="utf-8"))
    assert after["darwin"]["Sleep"] == summary
    assert after["linux"] == before["linux"]
    assert after["windows"] == before["windows"]


def test_raise_baseline_writes_lf(tmp_path, baseline_copy):
    summary = dict(json.loads(baseline_copy.read_text(encoding="utf-8"))[coverage_baseline.PLATFORM]["Sleep"])
    summary["line_percent"] = round(summary["line_percent"] + 0.5, 1)
    coverage_baseline.raise_baseline("Sleep", summary_dir(tmp_path, summary))
    assert b"\r" not in baseline_copy.read_bytes()
