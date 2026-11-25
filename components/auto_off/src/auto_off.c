/**
 * @file auto_off.c
 * @brief Module to switch the light off after a period without use.
 */

#include "auto_off.h"

/** @brief Number of OS ticks without use after which the light is switched off. */
#define AUTO_OFF_TIMEOUT_TICKS ((uint32_t)(((uint32_t)CONFIG_AUTO_OFF_TIMEOUT * 1000u) / CONFIG_OS_TASK_PERIOD))

/**
 * @rst
 * .. impl:: Auto Off runnable
 *    :id: SWIMPL_AO-001
 *    :implements: SWDD_AO-100, SWDD_AO-101, SWDD_AO-102, SWDD_AO-103, SWDD_AO-200, SWDD_AO-201, SWDD_AO-202
 *    :fulfills: REQ_39, REQ_40, REQ_59
 * @endrst
 */
void autoOff(void)
{
    static uint32_t ticksCounter = 0;
    static bool_t autoOffState = FALSE;

    const bool_t activity = (RteGetPowerKeyPressedEvent() == TRUE) ||
                            (RteIsKeyPressed(CONTROL_KEY_UP) == TRUE) ||
                            (RteIsKeyPressed(CONTROL_KEY_DOWN) == TRUE);

    if (activity)
    {
        ticksCounter = 0;
        autoOffState = FALSE;
    }
    else if (RteGetPowerState() != POWER_STATE_ON)
    {
        ticksCounter = 0;
    }
    else
    {
        ticksCounter++;
        if (ticksCounter >= AUTO_OFF_TIMEOUT_TICKS)
        {
            ticksCounter = 0;
            RteSetPowerState(POWER_STATE_OFF);
            autoOffState = TRUE;
        }
    }

    RteSetAutoOffState(autoOffState);
}
