# [DEMO-003] Eine eigene Rubrik für die Reviews

**Status**: Open
**Priority**: Medium
**Component**: `docs/rubric.md`

**Summary**:
Als Autor möchte ich, dass die Reviews der Loop auch die Regeln dieses Repos prüfen, damit ein
Review mehr findet als das, was für jedes Repo gilt.

**Description**:
Das Repo hat weder `docs/rubric.md` noch `CONTRIBUTING.md`. In der Loop gilt deshalb nur die
allgemeine Rubrik. Was nur hier zählt, steht heute als Fließtext in `AGENTS.md`, unter
*How We Develop*: Eine Spec nennt ihr REQ neben `SWARCH_001`. `:fulfills:` steht nur auf Code,
den nur die Varianten mit der Anforderung enthalten. Eine Spec, die eine Variante nicht wählt,
steht hinter einem Jinja-Guard mit demselben KConfig-Namen wie der Code.

Eine Rubrik nimmt nur auf, was ein Urteil braucht. Was ein Skript, ein Test oder das Gate
entscheiden kann, gehört dorthin (DEMO-004).

**Acceptance Criteria**:
- [ ] `docs/rubric.md` hält die Regeln fest, die nur dieses Repo hat und die ein Urteil brauchen.
- [ ] Jedes Kriterium sagt, woran ein Reviewer es im Diff erkennt.
- [ ] Kein Kriterium wiederholt die allgemeine Rubrik oder etwas, das das Gate prüft.
- [ ] `AGENTS.md` nennt die Rubrik in einem Abschnitt `Closed loop`.

**Related**: DEMO-004
