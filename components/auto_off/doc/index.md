# Software Detailed Design

```{toctree}
:maxdepth: 2
:caption: Table of Contents
```

## Introduction

Auto Off switches the light off by itself after a configurable time without operation. It is a
selectable feature (`AUTO_OFF`); variants that do not select it do not contain the component.

## Design Considerations

```{spec} Timeout
:id: SWDD_AO-100
:refines: SWARCH_001, REQ_39

The light is switched off after the time without operation given by the configuration `AUTO_OFF_TIMEOUT`, in seconds. The time is converted to a number of runnable cycles by the OS task period.
```

```{spec} Timer Only While Power ON
:id: SWDD_AO-101
:refines: SWARCH_001, REQ_39

The timer runs only while the power state is `POWER_STATE_ON`. While the power state is not ON, the timer is held at zero, so it starts from zero when the light is switched on.
```

```{spec} Operation Resets the Timer
:id: SWDD_AO-102
:refines: SWARCH_001, REQ_40

Every cycle in which the Up key or the Down key is pressed resets the timer, so a held key counts as continuous operation. The power key resets the timer once per press, through its key event.
```

```{spec} Switch Off
:id: SWDD_AO-103
:refines: SWARCH_001, REQ_39

When the timer reaches the timeout, Auto Off switches the light off with `RteSetPowerState(POWER_STATE_OFF)`.
```

## Interfaces

```{spec} Runnable
:id: SWDD_AO-200
:refines: SWARCH_001

Auto Off is called by its runnable `autoOff()`.
```

```{spec} Auto Off State Output
:id: SWDD_AO-201
:refines: SWARCH_001, REQ_59

Auto Off uses the RTE interface `RteSetAutoOffState()` to report its state: TRUE after the timer switched the light off, FALSE from the next operation on.
```

## Internal Behavior

```{mermaid}
stateDiagram-v2
    [*] --> Idle
    Idle --> Counting : Power State == ON
    Counting --> Counting : operation (timer reset)
    Counting --> Idle : Power State != ON
    Counting --> Idle : timeout (switch off, Auto Off state TRUE)
```
