# Software Detailed Design

This module switches the light off after a configurable period without user activity.

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

```{spec} Switch off at the timeout
:id: SWDD_AO-001
:refines: SWARCH_001, REQ_39

If the power state is POWER_STATE_ON and no user activity happened for `AUTO_OFF_TIMEOUT` seconds, the function shall set the power state to POWER_STATE_OFF.
```

```{spec} Timer only while ON
:id: SWDD_AO-002
:refines: SWARCH_001, REQ_39

The timer shall count only while the power state is POWER_STATE_ON. While the power state is POWER_STATE_OFF, the timer shall stay at zero, so that it restarts from zero at the next switch-on.
```

```{spec} User activity
:id: SWDD_AO-003
:refines: SWARCH_001, REQ_40

A power key press event (`RteGetPowerKeyPressedEvent()`), or the Up or Down key pressed in a tick (`RteIsKeyPressed()`), shall reset the timer. A held Up or Down key is continuous activity.
```

```{spec} Auto-off state
:id: SWDD_AO-004
:refines: SWARCH_001, REQ_59

When the timer switched the light off, the function shall call `RteSetAutoOffState(TRUE)`. At the next user activity, it shall call `RteSetAutoOffState(FALSE)`.
```

```{spec} Runnable
:id: SWDD_AO-005
:refines: SWARCH_001

`autoOff()` shall run once per OS tick. `autoOffInit()` shall restart the timer from zero.
```

## Function Flow

```{mermaid}
graph TD
   Start[Start]
   Activity{Power key event, Up or Down pressed?}
   ResetActive[Reset timer]
   WasOff{Auto-off state TRUE?}
   ClearState[Set auto-off state FALSE]
   IsOn{Is power state ON?}
   ResetOff[Reset timer]
   Count[Increment timer]
   Timeout{Timer reached timeout?}
   SwitchOff[Set power state OFF, set auto-off state TRUE, reset timer]
   End[End]

   Start --> Activity
   Activity -->|Yes| ResetActive --> WasOff
   WasOff -->|Yes| ClearState --> End
   WasOff -->|No| End
   Activity -->|No| IsOn
   IsOn -->|No| ResetOff --> End
   IsOn -->|Yes| Count --> Timeout
   Timeout -->|Yes| SwitchOff --> End
   Timeout -->|No| End
```
