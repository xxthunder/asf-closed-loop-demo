# [DEMO-006] Das Komponentendiagramm in Mermaid

**Status**: In Progress
**Priority**: Medium
**Component**: `doc/software_architecture/`

**Summary**:
Als Autor möchte ich das Komponentendiagramm als Mermaid-Text im Architekturdokument, damit jede
Änderung im Diff lesbar ist und ein Agent es ohne draw.io ändern kann.

**Description**:
Das Komponentendiagramm ist heute eine draw.io-Datei (`_figures/spled.drawio`) mit einem
exportierten SVG (`_figures/sw_arch_component_diagram.svg`), das `index.md` einbindet. Ändern
lässt es sich nur mit draw.io. Beide Dateien sind XML, das kein Mensch im
Diff liest, und das SVG kann still vom `.drawio` abweichen.

Der Bericht kann Mermaid schon: `sphinxcontrib.mermaid` gehört zu den Standard-Extensions von
spl-core (`SplSphinx.default_extensions`).

**Acceptance Criteria**:
- [ ] `doc/software_architecture/index.md` zeigt das Komponentendiagramm als Mermaid-Block, mit
  denselben Komponenten und Verbindungen wie die draw.io-Datei, die es ersetzt.
- [ ] `spled.drawio` und `sw_arch_component_diagram.svg` sind entfernt; nichts verweist mehr
  auf sie.
- [ ] Der Bericht jeder Variante rendert das Diagramm.

**Scope Decisions**:
- 2026-11-19: `spled.drawio` hat auch eine Seite „Feature Model“. Ihr Export
  `_figures/feature_model.svg` ist nirgends eingebunden. Er geht mit der draw.io-Datei. Die
  Features stehen in `KConfig`.
- 2026-11-19: Mermaid kennt keine Lollipop-Schnittstellen. Jede Schnittstelle wird ein Pfeil
  zwischen Komponente und RTE, beschriftet mit ihrem Namen (`SetLightValue`). Die Richtung zeigt
  den Datenfluss: ein Set-Aufruf zeigt zur RTE, ein Get-Aufruf von ihr weg.
- 2026-11-19: Das Diagramm zeigt je Variante nur die Komponenten, die sie wählt. Eine
  Komponente, die nicht jede Variante wählt, steht hinter einem Jinja-Guard mit demselben
  KConfig-Namen wie im CMake, zum Beispiel `BrightnessController` hinter
  `config.BRIGHTNESS_ADJUSTMENT_ENABLED`. Jinja rendert jede Seite des Berichts (`conf.py`,
  `source-read`).
- 2026-11-19: Kein Test hält die Knoten und Kanten fest. Die Gleichheit mit der draw.io-Datei
  prüft der Review: der Pull Request listet Komponenten und Schnittstellen aus der draw.io-Datei
  und aus dem Mermaid-Block nebeneinander. Ein Test prüft nur, dass der Bericht jeder Variante
  das Diagramm enthält.
