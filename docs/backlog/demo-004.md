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

**Acceptance Criteria**:
- [ ] Je Variante schlägt ein Test fehl, wenn eine Anforderung der Variante keine Spec hat, die
  sie verfeinert, oder wenn ein Link auf eine Need zeigt, die es nicht gibt.
- [ ] Ein Test schlägt fehl, wenn eine Sphinx-Warnung dazukommt. Die bekannten Warnungen sind
  behoben oder stehen einzeln und mit Grund auf einer Liste.
- [ ] Ein Test schlägt fehl, wenn die Coverage einer Variante unter eine Schwelle fällt. Die
  Schwelle steht an einer Stelle.
- [ ] Die Tests tragen den Marker `gate_develop_pr`, laufen also im Gate und in der CI.
- [ ] Ein absichtlich gebrochener Link, eine neue Warnung und eine gesenkte Coverage machen das
  Gate jeweils rot.
