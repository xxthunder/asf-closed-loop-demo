# [DEMO-009] Der Bericht zeigt Mermaid-Diagramme auch ohne Netz

**Status**: Open
**Priority**: Low
**Component**: `conf.py`

**Summary**:
Als Leser eines archivierten Berichts möchte ich die Diagramme auch ohne Netz sehen, damit ein
Bericht für sich allein steht.

**Description**:
`sphinxcontrib.mermaid` rendert im Browser. Jede Seite mit einem Mermaid-Block importiert
`mermaid@11.12.1` von cdn.jsdelivr.net; eine lokale Kopie liegt nicht im Bericht. Ohne Netz zeigt
jede dieser Seiten statt des Diagramms den Mermaid-Text. Das betrifft alle Mermaid-Diagramme des
Berichts, nicht erst DEMO-006: die Design-Dokumente von PowerSignalProcessing, PowerButton und
LightController und seit DEMO-006 die Softwarearchitektur.

**Acceptance Criteria**:
- [ ] Ein Bericht, ohne Netz im Browser geöffnet, zeigt jedes Mermaid-Diagramm als Bild.

**Related**: DEMO-006
