# [DEMO-004c] Das Gate wird rot, wenn die Coverage fällt

**Status**: Open
**Priority**: Medium
**Component**: `test/`, `pytest.ini`

**Summary**:
Als Autor möchte ich, dass das Gate rot wird, wenn die Coverage einer Variante fällt, damit neuer
Code nicht ohne Tests durchgeht.

**Description**:
Die Coverage steht heute nur im Coverage-Bericht jeder Variante. Ob sie fällt, sieht nur, wer den
Bericht von Hand liest.

**Acceptance Criteria**:
- [ ] Ein Test schlägt fehl, wenn die Coverage einer Variante unter eine Schwelle fällt.
- [ ] Die Schwelle steht an einer Stelle.
- [ ] Der Test trägt den Marker `gate_develop_pr`, läuft also im Gate und in der CI.
- [ ] Eine absichtlich gesenkte Coverage macht das Gate rot.
