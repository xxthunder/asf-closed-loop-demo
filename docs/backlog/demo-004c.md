# [DEMO-004c] Das Gate wird rot, wenn die Coverage fällt

**Status**: In Progress
**Priority**: Medium
**Component**: `test/`, `pytest.ini`

**Summary**:
Als Autor möchte ich, dass das Gate rot wird, wenn die Coverage einer Variante fällt, damit neuer
Code nicht ohne Tests durchgeht.

**Description**:
Die Coverage steht heute nur im Coverage-Bericht jeder Variante. Ob sie fällt, sieht nur, wer den
Bericht von Hand liest.

**Acceptance Criteria**:
- [ ] Ein Test schlägt fehl, wenn eine Coverage-Metrik einer Variante unter ihren Basiswert
  fällt. Das gilt für Lines, Functions, Branches und Conditions.
- [ ] Die Basiswerte stehen in einer Datei im Repo, je Variante und Metrik.
- [ ] Der Test trägt den Marker `gate_develop_pr`, läuft also im Gate und in der CI.
- [ ] Eine absichtlich gesenkte Coverage macht das Gate rot.

**Scope Decisions**:
- 2026-11-20: Das Gate prüft, dass die Coverage nicht fällt. Es prüft keinen Mindestwert. Die
  Kriterien sagten vorher „unter eine Schwelle“ und „Die Schwelle steht an einer Stelle“; sie
  sind entsprechend umformuliert.
- 2026-11-20: Geprüft werden alle vier Metriken, die `variant-coverage.json` und der Bericht
  heute zeigen: Lines, Functions, Branches, Conditions.
- 2026-11-20: Der Vergleich läuft gegen Basiswerte in einer Datei im Repo, je Variante die
  heutigen Werte, exakt wie gcovr sie schreibt. Ein Vergleich gegen einen Build von `develop`
  hätte jeden Gate- und CI-Lauf verdoppelt. Codecov hätte nur die CI geprüft, nicht das lokale
  Gate. Steigt die Coverage, hebt der Commit, der sie hebt, die Basiswerte an. Ein Test, der
  das erzwingt, gehört nicht dazu.
