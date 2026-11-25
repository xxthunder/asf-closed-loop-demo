# Software Detailed Design

```{toctree}
:maxdepth: 2
:caption: Table of Contents
```

## Introduction

The Auto Off component switches the light off after a configurable time without use.
The time is set by `AUTO_OFF_TIMEOUT` in seconds.

## Design Considerations

```{spec} Automatic Power Off
:id: SWDD_AO-100
:refines: SWARCH_001, REQ_39

The light is switched off after `AUTO_OFF_TIMEOUT` seconds without activity.
```

```{spec} Timer Only While On
:id: SWDD_AO-101
:refines: SWARCH_001, REQ_39

The timer runs only while the power state is `POWER_STATE_ON`.
A switch-off by the user stops the timer and clears the counted time.
```

```{spec} Activity Detection
:id: SWDD_AO-102
:refines: SWARCH_001, REQ_40

A power key event counts as one activity per key press.
The keys Up and Down count as activity in every tick in which they are pressed.
Each activity restarts the timer.
```

```{spec} Auto Off State
:id: SWDD_AO-103
:refines: SWARCH_001, REQ_59

The auto off state is `TRUE` after the timer has switched the light off.
It is `FALSE` from the next activity.
```

## Interfaces

```{spec} Runnable
:id: SWDD_AO-200
:refines: SWARCH_001

The Auto Off component is called by its runnable `autoOff()`.
```

```{spec} Inputs
:id: SWDD_AO-201
:refines: SWARCH_001, REQ_40

The Auto Off component uses the RTE interfaces `RteGetPowerState()`, `RteGetPowerKeyPressedEvent()` and `RteIsKeyPressed()` to read the power state and detect activity.
```

```{spec} Outputs
:id: SWDD_AO-202
:refines: SWARCH_001, REQ_39, REQ_59

The Auto Off component uses the RTE interfaces `RteSetPowerState()` to switch the light off and `RteSetAutoOffState()` to set the auto off state.
```
