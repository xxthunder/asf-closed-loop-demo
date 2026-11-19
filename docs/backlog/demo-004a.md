# [DEMO-004a] Das Gate prüft die Links von Implementierung und Tests

**Status**: In Progress
**Priority**: High
**Component**: `test/`

**Summary**:
Als Autor möchte ich, dass das Gate rot wird, wenn eine Implementierung oder ein Test auf keine
Spec zeigt oder ein Test kein Ergebnis hat, damit eine Loop nicht mit Code oder Tests endet, die
im Bericht ins Leere zeigen.

**Description**:
Eine Implementierung verweist mit `:implements:` auf ihre Design-Specs, ein Test mit `:tests:`.
Der Test-Report verbindet jeden Test über den Link `results` mit seinen Testfällen aus dem
Testlauf. Ob ein Link ins Leere zeigt oder ein Test kein Ergebnis hat, sieht heute nur, wer den
Bericht von Hand liest.

Das Gate prüft die Links von unten nach oben. Eine Spec ohne Implementierung oder Test bleibt
erlaubt.

**Scope Decisions**:
- 2026-11-19: Das Gate prüft keine Anforderungen. Der DOORS-Export gibt nicht her, welche
  Anforderung in welcher Variante verfeinert sein muss.
- 2026-11-19: Eine Spec darf ohne Implementierung und ohne Test bestehen. Eine Spec kann in
  einem Sprint entstehen und im nächsten implementiert werden.

**Acceptance Criteria**:
- [ ] Je Variante schlägt ein Test fehl, wenn eine Implementierung auf keine Spec der Variante
  zeigt: ohne `:implements:` oder mit einem Link auf eine Spec, die es nicht gibt.
- [ ] Je Variante schlägt ein Test fehl, wenn ein Test auf keine Spec der Variante zeigt: ohne
  `:tests:` oder mit einem Link auf eine Spec, die es nicht gibt.
- [ ] Je Variante schlägt ein Test fehl, wenn ein Test kein Ergebnis aus dem Test-Report hat.
- [ ] Eine Spec ohne Implementierung oder ohne Test macht das Gate nicht rot.
- [ ] Die Tests tragen den Marker `gate_develop_pr`, laufen also im Gate und in der CI.
- [ ] Ein absichtlich gebrochener Link und ein Test ohne Ergebnis machen das Gate jeweils rot.
