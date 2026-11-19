# [DEMO-009] Der Bericht zeigt Mermaid-Diagramme auch ohne Netz

**Status**: Open
**Priority**: Low
**Component**: `conf.py`

**Summary**:
Als Leser eines archivierten Berichts möchte ich die Diagramme auch ohne Netz sehen, damit ein
Bericht für sich allein steht.

**Description**:
`sphinxcontrib.mermaid` rendert im Browser und lädt `mermaid.js` von cdn.jsdelivr.net. Ohne Netz
zeigt ein Bericht statt des Diagramms den Mermaid-Text. Seit DEMO-006 gilt das auch für das
Komponentendiagramm, das vorher ein SVG war.

**Acceptance Criteria**:
- [ ] Ein Bericht, ohne Netz im Browser geöffnet, zeigt jedes Mermaid-Diagramm als Bild.

**Related**: DEMO-006
