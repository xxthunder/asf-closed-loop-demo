"""The rules in needs_schema.json, checked on a minimal Sphinx project with known faults."""

import ast
import json
from importlib.resources import files
from pathlib import Path

import pytest
from sphinx.cmd.build import build_main

pytestmark = [
    pytest.mark.reports,
    pytest.mark.gate_develop_pr,
    pytest.mark.gate_develop_push,
    pytest.mark.gate_develop_nightly,
    pytest.mark.gate_release_pr,
    pytest.mark.gate_release,
]

REPO = Path(__file__).parent.parent
UBPROJECT = files("spl_core.report_generation").joinpath("ubproject.toml")
SCHEMA_OPTION = "needs_schema_definitions_from_json"


def schema_from_conf() -> Path:
    """The schema file the repository's conf.py names, resolved like sphinx-needs does (against the confdir).

    Read with ast, not executed: conf.py opens the files that environment variables name.
    """
    tree = ast.parse((REPO / "conf.py").read_text(encoding="utf-8"))
    for node in tree.body:
        if isinstance(node, ast.Assign) and any(isinstance(target, ast.Name) and target.id == SCHEMA_OPTION for target in node.targets):
            return (REPO / ast.literal_eval(node.value)).resolve()
    pytest.fail(f"conf.py does not set {SCHEMA_OPTION}: the report runs without the link rules")


INDEX = """
Schema fixture
==============

.. spec:: Tested spec
   :id: SPEC_TESTED

.. spec:: Lone spec
   :id: SPEC_LONE

.. req:: A requirement
   :id: REQ_ONE

.. impl:: Good impl
   :id: IMPL_GOOD
   :implements: SPEC_TESTED

.. impl:: Impl of a missing spec
   :id: IMPL_MISSING
   :implements: SPEC_MISSING

.. impl:: Impl of a requirement
   :id: IMPL_OF_REQ
   :implements: REQ_ONE

.. test:: component.test_with_result
   :id: TEST_GOOD
   :tests: SPEC_TESTED

.. test:: component.test_without_result
   :id: TEST_NO_RESULT
   :tests: SPEC_TESTED

.. test-file:: Unit test results
   :id: TESTFILE_ONE
   :file: results.xml
   :auto_suites:
   :auto_cases:
"""

# One passed test case, named like TEST_GOOD's title: spl-core's dynamic `results` link
# (links_from_match('case', 'title')) resolves it, as in the real report.
RESULTS = """<?xml version="1.0" encoding="utf-8"?>
<testsuites>
  <testsuite name="component" tests="1" failures="0" errors="0" skipped="0">
    <testcase classname="component" name="component.test_with_result" time="0.001"/>
  </testsuite>
</testsuites>
"""


@pytest.fixture(scope="module")
def violations(tmp_path_factory) -> dict[str, list[dict]]:
    conf = f"""
extensions = ["sphinx_needs", "sphinx_test_reports"]
needs_from_toml = {str(UBPROJECT)!r}
{SCHEMA_OPTION} = {str(schema_from_conf())!r}
"""
    src = tmp_path_factory.mktemp("schema_fixture")
    (src / "conf.py").write_text(conf, encoding="utf-8")
    (src / "index.rst").write_text(INDEX, encoding="utf-8")
    (src / "results.xml").write_text(RESULTS, encoding="utf-8")
    out = src / "_build"
    assert build_main(["-b", "html", "-E", "-q", str(src), str(out)]) == 0
    report = out / "schema_violations.json"
    return json.loads(report.read_text(encoding="utf-8"))["validation_warnings"]


@pytest.mark.parametrize(
    ("need_id", "subtype"),
    [
        ("IMPL_MISSING", "network_missing_target"),
        ("IMPL_OF_REQ", "network_local_fail"),
        ("TEST_NO_RESULT", "local_fail"),
    ],
)
def test_faulty_need_violates(violations, need_id, subtype):
    assert need_id in violations, json.dumps(violations, indent=2)
    assert [warning["subtype"] for warning in violations[need_id]] == [subtype]


@pytest.mark.parametrize("need_id", ["IMPL_GOOD", "TEST_GOOD", "SPEC_LONE"])
def test_sound_need_passes(violations, need_id):
    assert need_id not in violations, json.dumps(violations[need_id], indent=2)
