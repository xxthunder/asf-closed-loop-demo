# [DEMO-007] Das Gate wird rot, wenn ein Selbsttest fehlschlägt

**Status**: Open
**Priority**: High
**Component**: `build.sh`, `build.ps1`, `.github/workflows/ci.yml`

**Summary**:
Als Autor möchte ich, dass `./build.sh --selftests` mit einem Fehlercode endet, wenn ein Test
fehlschlägt, damit das Gate an seinem Exit-Code rot wird.

**Description**:
`run_selftests` in `build.sh` ruft pytest mit `|| true` auf. Das Gate
(`./build.sh --selftests --marker gate_develop_pr`) endet deshalb auch bei fehlgeschlagenen Tests
mit 0. Die CI fängt den Fehler erst im Schritt „Test Results (Linux)“ ab: Er liest die
junit-Datei und schlägt mit `fail_on_failure` fehl. Lokal sieht man den Fehler nur in der Ausgabe
oder in `test/output/test-report.xml`. Die Loop für DEMO-006 hat das gefunden und die junit-Datei
nach jedem Gate-Lauf gelesen.

Der Kommentar an `|| true` sagt, warum: Die CI wertet den JUnit-Bericht aus
(`mikepenz/action-junit-report` mit `fail_on_failure`). Die Loop von DEMO-004b hat denselben Fehler
unabhängig gefunden: Ein absichtlich roter Report-Test, und `build.sh` endete trotzdem mit 0.
Ihr Item DEMO-010 ist hierin aufgegangen.

Ob `build.ps1` dasselbe tut, ist nicht geprüft.

**Acceptance Criteria**:
- [ ] `./build.sh --selftests` endet mit einem Code ungleich 0, wenn ein Test fehlschlägt oder
  einen Fehler hat, und mit 0, wenn alle bestehen.
- [ ] `build.ps1 -selftests` verhält sich gleich.
- [ ] Die CI lädt die Testergebnisse auch dann hoch, wenn die Tests fehlschlagen.
- [ ] Ein absichtlich roter Test macht das Gate rot.

**Related**: DEMO-004
