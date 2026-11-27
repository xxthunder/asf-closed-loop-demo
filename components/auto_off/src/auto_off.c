/**
 * @file auto_off.c
 * @brief Module to switch the light off after a period without activity.
 */

#include "auto_off.h"

/** @brief Number of ticks without activity after which the power is switched off. */
#define AUTO_OFF_TIMEOUT_TICKS ((uint32_t)((CONFIG_AUTO_OFF_TIMEOUT * 1000) / CONFIG_OS_TASK_PERIOD))

static uint32_t idleTicks = 0;       /**< Ticks counted without activity while the power is ON. */
static bool_t autoOffActive = FALSE; /**< Last auto-off state reported to the RTE. */

/**
 * @rst
 * .. impl:: Reset auto off
 *    :id: SWIMPL_AO-002
 *    :implements: SWDD_AO-005
 * @endrst
 */
void autoOffInit(void)
{
    idleTicks = 0;
    autoOffActive = FALSE;
}

/**
 * @rst
 * .. impl:: Auto off
 *    :id: SWIMPL_AO-001
 *    :implements: SWDD_AO-001, SWDD_AO-002, SWDD_AO-003, SWDD_AO-004, SWDD_AO-005
 *    :fulfills: REQ_39, REQ_40, REQ_59
 * @endrst
 */
void autoOff(void)
{
    const bool_t activity = (RteIsKeyPressed(CONTROL_KEY_UP) != FALSE) ||
                            (RteIsKeyPressed(CONTROL_KEY_DOWN) != FALSE) ||
                            (RteGetPowerKeyPressedEvent() != FALSE);

    if ((RteGetPowerState() == POWER_STATE_OFF) || activity)
    {
        idleTicks = 0;
    }
    else
    {
        idleTicks++;
        if (idleTicks >= AUTO_OFF_TIMEOUT_TICKS)
        {
            idleTicks = 0;
            RteSetPowerState(POWER_STATE_OFF);
            RteSetAutoOffState(TRUE);
            autoOffActive = TRUE;
        }
    }

    if (activity && autoOffActive)
    {
        RteSetAutoOffState(FALSE);
        autoOffActive = FALSE;
    }
}
