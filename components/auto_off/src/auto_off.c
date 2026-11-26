/**
 * @file auto_off.c
 * @brief Module that switches the light off after a period without user activity.
 */

#include "auto_off.h"
#include "rte.h"

/** @brief Number of OS ticks without user activity until the light is switched off. */
#define AUTO_OFF_TIMEOUT_TICKS ((uint32_t)(((uint32_t)CONFIG_AUTO_OFF_TIMEOUT * 1000u) / (uint32_t)CONFIG_OS_TASK_PERIOD))

static uint32_t idleTicks = 0; /**< OS ticks since the last user activity while the light is ON. */

/**
 * @rst
 *
 * .. impl:: Auto off initialization
 *    :id: SWIMPL_AO-001
 *    :implements: SWDD_AO-005
 * @endrst
 */
void autoOffInit(void)
{
    idleTicks = 0;
}

/**
 * @rst
 *
 * .. impl:: Auto off runnable
 *    :id: SWIMPL_AO-002
 *    :implements: SWDD_AO-001, SWDD_AO-002, SWDD_AO-003, SWDD_AO-004, SWDD_AO-005
 *    :fulfills: REQ_39, REQ_40, REQ_59
 * @endrst
 */
void autoOff(void)
{
    const bool_t powerKeyEvent = RteGetPowerKeyPressedEvent();
    const bool_t upPressed = RteIsKeyPressed(CONTROL_KEY_UP);
    const bool_t downPressed = RteIsKeyPressed(CONTROL_KEY_DOWN);

    if ((powerKeyEvent == TRUE) || (upPressed == TRUE) || (downPressed == TRUE))
    {
        // User activity restarts the timer and clears the auto-off state
        idleTicks = 0;
        if (RteGetAutoOffState() == TRUE)
        {
            RteSetAutoOffState(FALSE);
        }
    }
    else if (RteGetPowerState() == POWER_STATE_ON)
    {
        // Count only while the light is ON
        idleTicks++;
        if (idleTicks >= AUTO_OFF_TIMEOUT_TICKS)
        {
            RteSetPowerState(POWER_STATE_OFF);
            RteSetAutoOffState(TRUE);
            idleTicks = 0;
        }
    }
    else
    {
        // Nothing to switch off: the timer starts from zero at the next ON
        idleTicks = 0;
    }
}
