import json
import platform
import sys
from pathlib import Path

METRICS = ("line_percent", "function_percent", "branch_percent", "condition_percent")
BASELINE_FILE = Path(__file__).parent / "coverage_baseline.json"
SUMMARY_FILE = "variant-coverage.json"
PLATFORM: str = platform.system().lower()


def coverage_drops(summary: dict, baseline: dict) -> list[str]:
    return [f"{metric}: {summary[metric]} < {baseline[metric]}" for metric in METRICS if summary[metric] < baseline[metric]]


def assert_coverage_not_dropped(variant: str, build_dir: Path) -> None:
    baselines = json.loads(BASELINE_FILE.read_text(encoding="utf-8"))
    baseline = baselines.get(PLATFORM, {}).get(variant)
    assert baseline is not None, f"{variant}: no entry for platform {PLATFORM} in {BASELINE_FILE}"
    summary = json.loads((build_dir / SUMMARY_FILE).read_text(encoding="utf-8"))
    drops = coverage_drops(summary, baseline)
    assert not drops, f"{variant} on {PLATFORM}: coverage dropped below the baseline in {BASELINE_FILE}:\n" + "\n".join(drops)


def raise_baseline(variant: str, build_dir: Path) -> None:
    baselines = json.loads(BASELINE_FILE.read_text(encoding="utf-8"))
    summary = json.loads((build_dir / SUMMARY_FILE).read_text(encoding="utf-8"))
    current = baselines.setdefault(PLATFORM, {}).get(variant, summary)
    baselines[PLATFORM][variant] = {metric: max(current[metric], summary[metric]) for metric in METRICS}
    with BASELINE_FILE.open("w", encoding="utf-8", newline="\n") as file:
        file.write(json.dumps(baselines, indent=4) + "\n")


if __name__ == "__main__":
    from spl_core.test_utils.spl_build import SplBuild

    for name in sys.argv[1:]:
        raise_baseline(name, SplBuild(variant=name, build_kit="test", build_type="Debug", target="reports").build_dir)
