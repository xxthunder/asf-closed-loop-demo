# [DEMO-010] build.sh --selftests endet mit einem Fehlercode, wenn ein Test fehlschlägt

**Status**: Open
**Priority**: High
**Component**: `build.sh`, `build.ps1`, `.github/workflows/ci.yml`

**Summary**:
Als Autor möchte ich, dass `./build.sh --selftests` mit einem Fehlercode endet, wenn ein Test
fehlschlägt, damit ein Gate, das den Exit-Code liest, rot wird.

**Description**:
`run_selftests` in `build.sh` ruft pytest mit `|| true` auf. Der Kommentar sagt, warum: Die CI
wertet den JUnit-Bericht aus (`mikepenz/action-junit-report` mit `fail_on_failure`). Das Skript
endet deshalb auch dann mit 0, wenn Tests fehlschlagen.

Die Loop nimmt den Exit-Code von `./build.sh --install && ./build.sh --selftests --marker
gate_develop_pr` als Urteil. Mit `|| true` ist dieses Gate immer grün. Gefunden in der Loop von
DEMO-004b (2026-11-19): ein absichtlich roter Report-Test, und `build.sh` endete trotzdem mit 0.

**Acceptance Criteria**:
- [ ] `./build.sh --selftests` endet mit einem Fehlercode ungleich 0, wenn ein Test fehlschlägt
  oder pytest abbricht, und mit 0, wenn alle ausgewählten Tests bestehen.
- [ ] `build.ps1 -selftests` verhält sich genauso.
- [ ] Die CI veröffentlicht den JUnit-Bericht weiterhin auch dann, wenn die Tests fehlschlagen.
- [ ] Ein absichtlich fehlschlagender Test macht `./build.sh --selftests` rot.
