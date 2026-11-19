# [DEMO-005] Ein End-to-End-Test für Auto Off

**Status**: Open
**Priority**: Medium
**Component**: `test/`, `variants/Sleep/`, `variants/IDEA/Sloemada/`

**Summary**:
Als Autor möchte ich, dass ein Test das gebaute Sleep Light nach der Zeitdauer ausgehen sieht,
damit Auto Off nicht nur in Unit-Tests mit Mocks belegt ist.

**Description**:
Die Unit-Tests einer Komponente laufen gegen Mocks der RTE. Dass Auto Off (DEMO-002) im ganzen
Programm wirkt, zeigt dann nur die UAT von Hand.
`test/spled_integration` ist der einzige Test, der das ganze Programm laufen lässt, und er gehört
nur zu Disco. Disco hat kein Auto Off.

**Acceptance Criteria**:
- [ ] Ein Test lässt das Programm einer Variante mit Auto Off laufen: Licht an, keine Bedienung,
  nach der Zeitdauer aus.
- [ ] Bedienung mit Up oder Down hält das Licht über die Zeitdauer hinaus an.
- [ ] Der Test braucht keine echte Wartezeit von 10 Sekunden.
- [ ] Er läuft im Gate, also mit dem Marker `gate_develop_pr`.
- [ ] Ein absichtlich abgeschaltetes Auto Off macht ihn rot.

**Depends on**: DEMO-002
