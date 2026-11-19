# [DEMO-006] Das Komponentendiagramm in Mermaid

**Status**: Open
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
