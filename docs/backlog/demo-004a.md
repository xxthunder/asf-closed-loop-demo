# [DEMO-004a] Das Gate prüft die Links von Implementierung und Tests

**Status**: In Progress
**Priority**: High
**Component**: `conf.py`, `test/`

**Summary**:
Als Autor möchte ich, dass das Gate rot wird, wenn eine Implementierung oder ein Test auf keine
Spec zeigt oder ein Test kein Ergebnis hat, damit eine Loop nicht mit Code oder Tests endet, die
im Bericht ins Leere zeigen.

**Description**:
Eine Implementierung verweist mit `:implements:` auf ihre Design-Specs, ein Test mit `:tests:`.
Der Test-Report verbindet jeden Test über den Link `results` mit seinen Testfällen aus dem
Testlauf. Ob ein Link ins Leere zeigt oder ein Test kein Ergebnis hat, sieht heute nur, wer den
Bericht von Hand liest.

sphinx-needs prüft Needs gegen Schemas und schreibt jeden Verstoß nach `schema_violations.json`
im Bericht. Die Regeln für die Links stehen deshalb als Schema dort. Sphinx endet bei einem
Verstoß aber mit Exit 0, weil spl-core es ohne `-W` aufruft. Ein Test je Variante liest deshalb
`schema_violations.json` und macht das Gate rot.

**Scope Decisions**:
- 2026-11-19: Das Gate prüft keine Anforderungen. Der DOORS-Export gibt nicht her, welche
  Anforderung in welcher Variante verfeinert sein muss.
- 2026-11-19: Eine Spec darf ohne Implementierung und ohne Test bestehen. Eine Spec kann in
  einem Sprint entstehen und im nächsten implementiert werden.
- 2026-11-19: Die Prüfung nutzt die Schema-Validierung von sphinx-needs, keinen eigenen Checker.

**Acceptance Criteria**:
- [ ] Ein Schema der Schema-Validierung von sphinx-needs hält die Regeln an einer Stelle fest:
  Eine Implementierung zeigt mit `:implements:` auf mindestens eine Spec, ein Test mit `:tests:`
  auf mindestens eine Spec, und ein Test hat mindestens ein Ergebnis (`results`).
- [ ] Je Variante schlägt ein Test fehl, wenn `schema_violations.json` des Berichts einen Verstoß
  enthält. Seine Meldung nennt die Need und den Verstoß.
- [ ] Eine Spec ohne Implementierung oder ohne Test macht das Gate nicht rot.
- [ ] Die Tests tragen den Marker `gate_develop_pr`, laufen also im Gate und in der CI.
- [ ] Ein absichtlich gebrochener Link und ein Test ohne Ergebnis machen das Gate jeweils rot.
