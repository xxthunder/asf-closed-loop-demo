# Software Detailed Design

```{toctree}
:maxdepth: 2
:caption: Table of Contents
```

## Introduction

The Auto Off component switches the light off after a configurable period without use.

## Design Considerations

```{spec} Auto Off Timeout
:id: SWDD_AO-100
:refines: SWARCH_001, REQ_39

The light is switched off when it was not used for the configured number of seconds (`AUTO_OFF_TIMEOUT`, 5 to 7200).
```

```{spec} Timer Runs While Light Is On
:id: SWDD_AO-101
:refines: SWARCH_001, REQ_39

The timeout is counted only while the power state is `ON`. While the power state is `OFF` the timer is held at zero, so the full timeout runs again after the light is switched on.
```

```{spec} Use Restarts The Timeout
:id: SWDD_AO-102
:refines: SWARCH_001, REQ_40

Every call in which the Up key or the Down key is pressed restarts the timeout, so a held key counts as continuous use. The power key counts through its event, once per press.
```

```{spec} Auto Off State
:id: SWDD_AO-103
:refines: SWARCH_001, REQ_59

The auto off state is `TRUE` as soon as the timeout has switched the light off and `FALSE` from the next use on.
```

## Interfaces

```{spec} Runnable
:id: SWDD_AO-200
:refines: SWARCH_001

The Auto Off component is called by its runnable `autoOff()`.
```

```{spec} Use Inputs
:id: SWDD_AO-201
:refines: SWARCH_001, REQ_40

The Auto Off component uses the RTE interfaces `RteGetPowerKeyPressedEvent()` and `RteIsKeyPressed()` to detect use.
```

```{spec} Power State Interface
:id: SWDD_AO-202
:refines: SWARCH_001, REQ_39

The Auto Off component uses the RTE interface `RteGetPowerState()` to count only while the light is on and the RTE interface `RteSetPowerState()` to switch the light off.
```

```{spec} Auto Off State Output
:id: SWDD_AO-203
:refines: SWARCH_001, REQ_59

The Auto Off component uses the RTE interface `RteSetAutoOffState()` to report the auto off state. The RTE provides it, together with `RteGetAutoOffState()`, only where `AUTO_OFF` is set.
```
