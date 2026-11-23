# [DEMO-011] Das Backlog liegt in doc/backlog bei der übrigen Dokumentation

**Status**: Open
**Priority**: Medium
**Component**: `docs/backlog/`, `conf.py`, `README.md`

**Summary**:
Als Entwickler möchte ich das Backlog in `doc/backlog/` finden, damit die ganze Dokumentation
unter einem Verzeichnis liegt.

**Description**:
Die Dokumentation liegt in `doc/`, das Backlog als einziges in `docs/backlog/`. Zwei
Verzeichnisse mit fast gleichem Namen verwirren: wer `doc/` liest, findet das Backlog nicht.

Der Umzug berührt drei Stellen:

- `conf.py` nimmt mit `include_patterns = ["index.md", "doc/**"]` alles unter `doc/` in den
  Bericht auf. Ohne Ausschluss werden die Items zu Sphinx-Quellen ohne Toctree; jede Variante
  meldet dann Warnungen, und `test_reports` lässt das Gate rot werden.
- `README.md` verlinkt `docs/backlog/`.
- Das Plugin `xxthunder-dev-skills` sucht das Backlog fest in `docs/backlog/`
  (`BACKLOG_IN_REPO` in `scripts/backlog.py`). XAS-104 in xxthunder-agentic-skills bringt ihm
  bei, auch `doc/backlog/` zu finden. Der Umzug wartet auf diese Plugin-Version; ohne sie finden
  `backlog-ops`, `refinement` und `closed-loop` das Backlog hier nicht mehr.

Links von außen auf `docs/backlog/demo-*.md` (Board-Karten, Notizen) zeigen nach dem Umzug ins
Leere.

**Acceptance Criteria**:
- [ ] Alle Items und die Backlog-`README.md` liegen in `doc/backlog/`; `docs/` gibt es nicht mehr.
- [ ] `git log --follow` zeigt für jedes Item die Geschichte vor dem Umzug.
- [ ] `conf.py` schließt `doc/backlog/**` aus, und das Gate ist grün.
- [ ] `README.md` verlinkt `doc/backlog/README.md`.
- [ ] `backlog.py toc` findet `doc/backlog/` ohne `--dir` und schreibt das Inhaltsverzeichnis
  ohne Änderung.

**Depends on**: XAS-104 in xxthunder-agentic-skills
