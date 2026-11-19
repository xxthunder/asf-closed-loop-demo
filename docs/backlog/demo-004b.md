# [DEMO-004b] Das Gate wird rot bei einer neuen Sphinx-Warnung

**Status**: In Progress
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

**Scope Decisions**:
- **Alle 7 Warnungen beheben** (Entscheidung des Autors, 2026-11-19): Keine Liste bekannter
  Warnungen. Der Test verlangt null Warnungen. Die 7 sind, in jeder Variante gleich:
  vier Dokumente in keinem Toctree, zweimal der unbekannte Pygments-Lexer `jsonc`, einmal
  unbekannte Import-Keys in `doc/sw_requirements/index.md`.
- **Dokumente ohne Toctree** (Entscheidung des Autors, 2026-11-19):
  `doc/cross-platform-alignment-proposal.md` und `doc/customer_requirements/index.md` werden
  gelöscht. `doc/devcontainer-and-bootstrap-design.md` und `doc/testing_strategy.md` bleiben,
  weil `AGENTS.md` und `README.md` auf sie verweisen; sie kommen aus dem Bericht heraus. Ihre
  beiden `jsonc`-Warnungen verschwinden damit. Der Verweis von
  `devcontainer-and-bootstrap-design.md` auf das gelöschte Proposal fällt weg.
- **DOORS-Keys** (Entscheidung des Autors, 2026-11-19): `configuration`, `origin`, `priority`
  und `reqif_uuid` werden als Need-Optionen deklariert. Sie bleiben an jeder Anforderung
  sichtbar.
- **Je Variante** (Entscheidung des Autors, 2026-11-19): Der Test prüft den Bericht jeder
  Variante für sich. Jinja-Guards machen Teile des Berichts variantenabhängig.
- **DEMO-006**: Wer als Zweiter mergt, hält den Bericht bei null Warnungen.

**Acceptance Criteria**:
- [ ] Ein Test schlägt fehl, wenn eine Sphinx-Warnung dazukommt.
- [ ] Die bekannten Warnungen sind behoben oder stehen einzeln und mit Grund auf einer Liste.
- [ ] Der Test trägt den Marker `gate_develop_pr`, läuft also im Gate und in der CI.
- [ ] Eine absichtlich erzeugte neue Warnung macht das Gate rot.

**Related**: DEMO-006
