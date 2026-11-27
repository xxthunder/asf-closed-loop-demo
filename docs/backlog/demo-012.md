# [DEMO-012] Jede Variante prüft, dass sie genau ihre Komponenten baut

**Status**: Open
**Priority**: Medium
**Component**: `test/<Variante>/test_<Variante>.py`

**Summary**:
Als Entwicklerin der Produktlinie möchte ich, dass jede Variante genau die Komponenten baut, die
ihr Test nennt, damit ein Feature, das in einer Variante fehlen soll, dort nicht unbemerkt
auftaucht, ohne dass jedes Feature einen eigenen Test braucht.

**Description**:
Jede Varianten-Testklasse führt eine Liste `components` und prüft nur, dass deren Artefakte
existieren. Ob die Variante weitere Komponenten baut, prüft kein Test. Ein Feature, das in
Disco oder Spa fehlen soll, kann dort also auftauchen, und das Gate bleibt grün. Auto Off
(DEMO-002) ist ein solches Feature: Disco Light und Spa Light enthalten den Code dafür nicht.

Ein eigener Test je Feature, der `config.txt` und `parts.cmake` der Varianten als Text liest,
ist schlecht zu warten: Die Zuordnung steht doppelt, im Test und in der Variante. Eine neue
Variante müsste in jedem dieser Tests nachgetragen werden. Und der Test prüft die Eingabe,
nicht das, was der Build daraus macht.

Der Build schreibt je Variante `reports/config.json`. Darin markiert `components_info` mit
`has_reports: True` genau die Komponenten, die in der `components`-Liste der Variante stehen
(nachgesehen an einem Sleep-Build). Ein Vergleich auf Gleichheit prüft damit Anwesenheit und
Abwesenheit, für alle Features auf einmal.

**Acceptance Criteria**:
- [ ] Der Report-Test jeder Produktvariante schlägt fehl, wenn die Menge der Komponenten mit
  `has_reports: True` in `reports/config.json` von ihrer `components`-Liste abweicht, und nennt
  die fehlenden und die zusätzlichen Komponenten.
- [ ] Die Prüfung steht einmal in einem gemeinsamen Hilfsmodul unter `test/`, wie
  `test/architecture_diagram.py`, und jede Varianten-Testklasse ruft sie auf.
- [ ] Ein Test zeigt, dass die Prüfung rot wird, wenn eine Variante eine Komponente baut, die
  ihre Liste nicht nennt.

**Open Questions**:
- Gilt die Prüfung auch für Base/Dev, deren Liste Beispielkomponenten enthält?
- Soll die Prüfung in einer eigenen Testfunktion stehen oder in `test_reports`, das
  `config.json` schon erzeugt?

**Related**: DEMO-002
