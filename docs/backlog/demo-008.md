# [DEMO-008] Das Komponentendiagramm zeigt je Variante nur die Schnittstellen, die sie nutzt

**Status**: Open
**Priority**: Low
**Component**: `doc/software_architecture/index.md`

**Summary**:
Als Leser des Berichts möchte ich im Komponentendiagramm einer Variante nur die Schnittstellen
sehen, die sie nutzt, damit das Diagramm für die Variante stimmt.

**Description**:
Seit DEMO-006 zeigt das Diagramm je Variante nur die Komponenten, die sie wählt. Die Kanten folgen
aber der alten draw.io-Datei und kennen kein KConfig:

- Disco zeigt `GetBrightnessValue` zum LightController. `light_controller.c` liest den Wert nur
  mit `CONFIG_BRIGHTNESS_ADJUSTMENT_ENABLED`.
- Sleep und Sloemada zeigen `SetBrightnessAdjustmentCounter`. Den gibt es nur mit der
  automatischen Helligkeit, also nur in Spa.
- `GetMainKnobValue` zum LightController liest der Code nur mit `CONFIG_BLINKING`.

Der Code weicht auch sonst vom Diagramm ab: `main_control_knob.c` ruft `RteGetMainKnobValue`,
`light_controller.c` ruft `RteGetBrightnessAdjustmentCounter`, und KeyboardInterface fehlt.

**Acceptance Criteria**:
- [ ] Jede Kante, die nur mit einem Feature existiert, steht hinter einem Jinja-Guard mit dem
  KConfig-Namen aus dem Code.
- [ ] Das Diagramm zeigt die Schnittstellen, die der Code aufruft, und keine anderen.

**Related**: DEMO-006
