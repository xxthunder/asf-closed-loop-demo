# [DEMO-004a] Das Gate prüft die Traceability der Berichte

**Status**: In Progress
**Priority**: High
**Component**: `test/`

**Summary**:
Als Autor möchte ich, dass das Gate rot wird, wenn die Traceability bricht, damit eine Loop nicht
mit einer Anforderung ohne Spec endet.

**Description**:
Die Traceability-Matrix in `doc/results/index.md` listet je Variante jede Anforderung mit ihren
Links *is refined by* und *is fulfilled by*. Ob eine Anforderung leer ausgeht, sieht heute nur,
wer den Bericht von Hand liest. Ein Link auf eine Need, die es nicht gibt, fällt ebenso wenig auf.

**Acceptance Criteria**:
- [ ] Je Variante schlägt ein Test fehl, wenn eine Anforderung der Variante keine Spec hat, die
  sie verfeinert.
- [ ] Je Variante schlägt ein Test fehl, wenn ein Link auf eine Need zeigt, die es nicht gibt.
- [ ] Die Tests tragen den Marker `gate_develop_pr`, laufen also im Gate und in der CI.
- [ ] Eine absichtlich entfernte Verfeinerung und ein absichtlich gebrochener Link machen das
  Gate jeweils rot.
