/**
 * @file auto_off.c
 * @brief Module that switches the light off after a period without use.
 */

#include "auto_off.h"

/** @brief Number of runnable calls that make up the auto off timeout. */
#define AUTO_OFF_TIMEOUT_TICKS ((uint32_t)((CONFIG_AUTO_OFF_TIMEOUT * 1000) / CONFIG_OS_TASK_PERIOD))

/** @brief Runnable calls since the last use while the light is on. */
SPLE_TESTABLE_STATIC uint32_t autoOffTicks = 0;

/**
 * @rst
 * .. impl:: Auto Off
 *    :id: SWIMPL_AO-001
 *    :implements: SWDD_AO-100, SWDD_AO-101, SWDD_AO-102, SWDD_AO-103, SWDD_AO-200, SWDD_AO-201, SWDD_AO-202, SWDD_AO-203
 *    :fulfills: REQ_39, REQ_40, REQ_59
 * @endrst
 */
void autoOff(void)
{
    if ((RteGetPowerKeyPressedEvent() == TRUE) || (RteIsKeyPressed(CONTROL_KEY_UP) == TRUE) || (RteIsKeyPressed(CONTROL_KEY_DOWN) == TRUE))
    {
        // Use of the light restarts the timeout and clears the auto off state
        autoOffTicks = 0;
        RteSetAutoOffState(FALSE);
    }
    else if (RteGetPowerState() == POWER_STATE_ON)
    {
        autoOffTicks++;
        if (autoOffTicks >= AUTO_OFF_TIMEOUT_TICKS)
        {
            RteSetPowerState(POWER_STATE_OFF);
            RteSetAutoOffState(TRUE);
            autoOffTicks = 0;
        }
    }
    else
    {
        // Light is off, there is nothing to switch off
        autoOffTicks = 0;
    }
}
