# [DEMO-004b] Das Gate wird rot bei einer neuen Sphinx-Warnung

**Status**: Open
**Priority**: High
**Component**: `test/`, `doc/`

**Summary**:
Als Autor möchte ich, dass das Gate rot wird, wenn eine Sphinx-Warnung dazukommt, damit eine neue
Warnung nicht zwischen den alten untergeht.

**Description**:
Jeder Bericht endet mit denselben 7 Sphinx-Warnungen. Weil sie immer da sind, fiele eine neue
Warnung niemandem auf.

DEMO-006 entfernt das draw.io-Diagramm und sein SVG. Das kann die Liste der Warnungen ändern.
Wer von beiden als Zweiter fertig wird, prüft die Liste nach dem Rebase.

**Acceptance Criteria**:
- [ ] Ein Test schlägt fehl, wenn eine Sphinx-Warnung dazukommt.
- [ ] Die bekannten Warnungen sind behoben oder stehen einzeln und mit Grund auf einer Liste.
- [ ] Der Test trägt den Marker `gate_develop_pr`, läuft also im Gate und in der CI.
- [ ] Eine absichtlich erzeugte neue Warnung macht das Gate rot.

**Related**: DEMO-006
