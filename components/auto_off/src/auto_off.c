/**
 * @file auto_off.c
 * @brief Module to switch the light off after a period without operation.
 */

#include "auto_off.h"

/** @brief Number of runnable cycles that make up the configured timeout. */
#define AUTO_OFF_TIMEOUT_TICKS ((uint32_t)((CONFIG_AUTO_OFF_TIMEOUT * 1000u) / CONFIG_OS_TASK_PERIOD))

static uint32_t cycleCounter = 0u;

void autoOffInit(void)
{
    cycleCounter = 0u;
}

/**
 * @rst
 * .. impl:: Auto Off runnable
 *    :id: SWIMPL_AO-001
 *    :implements: SWDD_AO-100, SWDD_AO-101, SWDD_AO-102, SWDD_AO-103, SWDD_AO-200, SWDD_AO-201
 *    :fulfills: REQ_39, REQ_40, REQ_59
 * @endrst
 */
void autoOff(void)
{
    const bool_t powerKeyEvent = RteGetPowerKeyPressedEvent();
    const bool_t upPressed = RteIsKeyPressed(CONTROL_KEY_UP);
    const bool_t downPressed = RteIsKeyPressed(CONTROL_KEY_DOWN);
    const bool_t operation = (powerKeyEvent == TRUE) || (upPressed == TRUE) || (downPressed == TRUE);

    if (operation)
    {
        cycleCounter = 0u;
        RteSetAutoOffState(FALSE);
    }

    if (RteGetPowerState() != POWER_STATE_ON)
    {
        cycleCounter = 0u;
    }
    else if (!operation)
    {
        cycleCounter++;
        if (cycleCounter >= AUTO_OFF_TIMEOUT_TICKS)
        {
            RteSetPowerState(POWER_STATE_OFF);
            RteSetAutoOffState(TRUE);
            cycleCounter = 0u;
        }
    }
    else
    {
        /* Operation in this cycle: the counter was reset above. */
    }
}
