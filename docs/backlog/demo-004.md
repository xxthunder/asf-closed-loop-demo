# [DEMO-004] Das Gate prüft, was in den Berichten steht

**Status**: Open
**Priority**: High
**Component**: `test/`, `pytest.ini`, `.github/workflows/ci.yml`

**Summary**:
Als Autor möchte ich, dass das Gate rot wird, wenn die Traceability bricht, eine neue
Sphinx-Warnung auftaucht oder die Coverage fällt, damit eine Loop nicht auf einem Bericht endet,
der nur baut.

**Description**:
Das Gate (`./build.sh --selftests --marker gate_develop_pr`) prüft heute, dass die Berichte
bauen, nicht was in ihnen steht. Traceability und Coverage prüft nur, wer
den Bericht von Hand liest. Jeder Bericht endet mit denselben 7 Sphinx-Warnungen. Weil sie immer
da sind, fiele eine neue Warnung niemandem auf.

Die drei Prüfungen sind unabhängig voneinander. Jede ist eine eigene Substory, damit jede für
sich durch die Loop gehen kann.

**Substories**:
- DEMO-004a: Das Gate prüft die Traceability der Berichte
- DEMO-004b: Das Gate wird rot bei einer neuen Sphinx-Warnung
- DEMO-004c: Das Gate wird rot, wenn die Coverage fällt

**Acceptance Criteria**:
- [ ] DEMO-004a, DEMO-004b und DEMO-004c sind erledigt.
