# Software Detailed Design

The Auto Off component switches the light off after a period without activity.

## Module Header

```c
#include "auto_off.h"
#include "rte.h"
```

## Function Description

```c
void autoOffInit(void)
void autoOff(void)
```

```{spec} Timeout
:id: SWDD_AO-001
:refines: SWARCH_001, REQ_39
:integrity: QM

The timer counts ticks while the power state is ON. After AUTO_OFF_TIMEOUT the component switches the power state to OFF.
```

```{spec} Activity
:id: SWDD_AO-002
:refines: SWARCH_001, REQ_40
:integrity: QM

Activity is the Up key pressed, the Down key pressed, or a power key event in a tick. A key that stays pressed counts as activity in every tick. Activity resets the timer.
```

```{spec} Timer only while ON
:id: SWDD_AO-003
:refines: SWARCH_001
:integrity: QM

The timer runs only while the power state is ON. While the power state is OFF, the timer is reset.
This is a derived design decision. No requirement states it.
```

```{spec} Auto-off state
:id: SWDD_AO-004
:refines: SWARCH_001, REQ_59
:integrity: QM

The component reports the auto-off state with `RteSetAutoOffState()`. The state is TRUE when the timer switched the power off. It is FALSE from the next activity.
```

```{spec} Runnable and initialization
:id: SWDD_AO-005
:refines: SWARCH_001
:integrity: QM

The runnable `autoOff()` is called once per tick. The function `autoOffInit()` resets the timer and the auto-off state.
This is a derived design decision. No requirement states it.
```

## Function Flow

```{mermaid}
graph TD
   Start[Start]
   Off{Power state OFF or activity?}
   Reset[Reset timer]
   Count[Count tick]
   Timeout{Timeout reached?}
   SwitchOff[Reset timer, set power state OFF, auto-off state TRUE]
   Clear{Activity and auto-off state TRUE?}
   ClearState[Auto-off state FALSE]
   End[End]

   Start --> Off
   Off -->|Yes| Reset --> Clear
   Off -->|No| Count --> Timeout
   Timeout -->|Yes| SwitchOff --> Clear
   Timeout -->|No| Clear
   Clear -->|Yes| ClearState --> End
   Clear -->|No| End
```
