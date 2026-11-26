/**
 * @file
 */

#include <gtest/gtest.h>
using namespace testing;

extern "C"
{
#include "auto_off.h"
#include "rte.h"
}

#include "mockup_components_auto_off.h"

/* Number of ticks until the light is switched off, derived from the configuration. */
static const unsigned int TIMEOUT_TICKS =
    (unsigned int)CONFIG_AUTO_OFF_TIMEOUT * 1000U / (unsigned int)CONFIG_OS_TASK_PERIOD;

/* Inputs the mocked RTE returns, set by the tests before each tick. */
static PowerState g_power = POWER_STATE_OFF;
static bool_t g_powerKeyEvent = FALSE;
static bool_t g_upPressed = FALSE;
static bool_t g_downPressed = FALSE;
static bool_t g_autoOffState = FALSE;

/* Outputs of the component, counted by the mocked RTE. */
static unsigned int g_setPowerStateCalls = 0;
static unsigned int g_setPowerStateOffCalls = 0;
static unsigned int g_setAutoOffTrueCalls = 0;
static unsigned int g_setAutoOffFalseCalls = 0;

class AutoOffTest : public Test
{
protected:
    void SetUp() override
    {
        g_power = POWER_STATE_OFF;
        g_powerKeyEvent = FALSE;
        g_upPressed = FALSE;
        g_downPressed = FALSE;
        g_autoOffState = FALSE;
        g_setPowerStateCalls = 0;
        g_setPowerStateOffCalls = 0;
        g_setAutoOffTrueCalls = 0;
        g_setAutoOffFalseCalls = 0;
        autoOffInit();
    }

    /* Install the RTE behaviour: getters read the g_* inputs, setters are counted. */
    template <typename Mock>
    void stubRte(Mock &mymock)
    {
        EXPECT_CALL(mymock, RteGetPowerState()).WillRepeatedly(Invoke([]() { return g_power; }));
        EXPECT_CALL(mymock, RteGetPowerKeyPressedEvent()).WillRepeatedly(Invoke([]() { return g_powerKeyEvent; }));
        EXPECT_CALL(mymock, RteIsKeyPressed(_)).WillRepeatedly(Invoke([](int32_t key) {
            if (key == CONTROL_KEY_UP)
            {
                return g_upPressed;
            }
            if (key == CONTROL_KEY_DOWN)
            {
                return g_downPressed;
            }
            return (bool_t)FALSE;
        }));
        EXPECT_CALL(mymock, RteGetAutoOffState()).WillRepeatedly(Invoke([]() { return g_autoOffState; }));
        EXPECT_CALL(mymock, RteSetPowerState(_)).WillRepeatedly(Invoke([](PowerState state) {
            g_setPowerStateCalls++;
            if (state == POWER_STATE_OFF)
            {
                g_setPowerStateOffCalls++;
            }
        }));
        EXPECT_CALL(mymock, RteSetAutoOffState(_)).WillRepeatedly(Invoke([](bool_t state) {
            if (state == TRUE)
            {
                g_setAutoOffTrueCalls++;
            }
            else
            {
                g_setAutoOffFalseCalls++;
            }
        }));
    }

    /* Run the component for n ticks with the current inputs. */
    static void tick(unsigned int n)
    {
        for (unsigned int i = 0; i < n; i++)
        {
            autoOff();
        }
    }

    /* Run n idle ticks: no power key event, Up and Down not pressed. */
    static void idle(PowerState power, unsigned int n)
    {
        g_power = power;
        g_powerKeyEvent = FALSE;
        g_upPressed = FALSE;
        g_downPressed = FALSE;
        tick(n);
    }
};

/*!
 * @rst
 *
 * .. test:: AutoOffTest.test_turns_off_after_timeout
 *    :id: TS_AO-001
 *    :tests: SWDD_AO-001, SWDD_AO-004
 *
 * @endrst
 */
TEST_F(AutoOffTest, test_turns_off_after_timeout)
{
    CREATE_MOCK(mymock);
    stubRte(mymock);
    ASSERT_GT(TIMEOUT_TICKS, 1U);

    idle(POWER_STATE_ON, TIMEOUT_TICKS - 1);
    EXPECT_EQ(g_setPowerStateCalls, 0U);
    EXPECT_EQ(g_setAutoOffTrueCalls, 0U);

    idle(POWER_STATE_ON, 1);
    EXPECT_EQ(g_setPowerStateCalls, 1U);
    EXPECT_EQ(g_setPowerStateOffCalls, 1U);
    EXPECT_EQ(g_setAutoOffTrueCalls, 1U);
    EXPECT_EQ(g_setAutoOffFalseCalls, 0U);
}

/*!
 * @rst
 *
 * .. test:: AutoOffTest.test_timer_stops_while_off
 *    :id: TS_AO-002
 *    :tests: SWDD_AO-002
 *
 * @endrst
 */
TEST_F(AutoOffTest, test_timer_stops_while_off)
{
    CREATE_MOCK(mymock);
    stubRte(mymock);

    idle(POWER_STATE_OFF, 2 * TIMEOUT_TICKS);
    idle(POWER_STATE_ON, TIMEOUT_TICKS - 1);

    EXPECT_EQ(g_setPowerStateCalls, 0U);
    EXPECT_EQ(g_setAutoOffTrueCalls, 0U);
    EXPECT_EQ(g_setAutoOffFalseCalls, 0U);
}

/*!
 * @rst
 *
 * .. test:: AutoOffTest.test_timer_restarts_after_power_off
 *    :id: TS_AO-003
 *    :tests: SWDD_AO-002
 *
 * @endrst
 */
TEST_F(AutoOffTest, test_timer_restarts_after_power_off)
{
    CREATE_MOCK(mymock);
    stubRte(mymock);

    idle(POWER_STATE_ON, TIMEOUT_TICKS - 1);
    idle(POWER_STATE_OFF, 1);
    idle(POWER_STATE_ON, TIMEOUT_TICKS - 1);
    EXPECT_EQ(g_setPowerStateCalls, 0U);

    idle(POWER_STATE_ON, 1);
    EXPECT_EQ(g_setPowerStateOffCalls, 1U);
}

/*!
 * @rst
 *
 * .. test:: AutoOffTest.test_power_key_resets_timer
 *    :id: TS_AO-004
 *    :tests: SWDD_AO-003
 *
 * @endrst
 */
TEST_F(AutoOffTest, test_power_key_resets_timer)
{
    CREATE_MOCK(mymock);
    stubRte(mymock);

    idle(POWER_STATE_ON, TIMEOUT_TICKS - 1);

    g_power = POWER_STATE_ON;
    g_powerKeyEvent = TRUE;
    tick(1);
    EXPECT_EQ(g_setPowerStateCalls, 0U);

    idle(POWER_STATE_ON, TIMEOUT_TICKS - 1);
    EXPECT_EQ(g_setPowerStateCalls, 0U);

    idle(POWER_STATE_ON, 1);
    EXPECT_EQ(g_setPowerStateOffCalls, 1U);
}

/*!
 * @rst
 *
 * .. test:: AutoOffTest.test_held_up_key_keeps_light_on
 *    :id: TS_AO-005
 *    :tests: SWDD_AO-003
 *
 * @endrst
 */
TEST_F(AutoOffTest, test_held_up_key_keeps_light_on)
{
    CREATE_MOCK(mymock);
    stubRte(mymock);

    g_power = POWER_STATE_ON;
    g_upPressed = TRUE;
    tick(3 * TIMEOUT_TICKS);

    EXPECT_EQ(g_setPowerStateCalls, 0U);
}

/*!
 * @rst
 *
 * .. test:: AutoOffTest.test_held_down_key_keeps_light_on
 *    :id: TS_AO-006
 *    :tests: SWDD_AO-003
 *
 * @endrst
 */
TEST_F(AutoOffTest, test_held_down_key_keeps_light_on)
{
    CREATE_MOCK(mymock);
    stubRte(mymock);

    g_power = POWER_STATE_ON;
    g_downPressed = TRUE;
    tick(3 * TIMEOUT_TICKS);

    EXPECT_EQ(g_setPowerStateCalls, 0U);
}

/*!
 * @rst
 *
 * .. test:: AutoOffTest.test_activity_clears_auto_off_state
 *    :id: TS_AO-007
 *    :tests: SWDD_AO-004
 *
 * @endrst
 */
TEST_F(AutoOffTest, test_activity_clears_auto_off_state)
{
    CREATE_MOCK(mymock);
    stubRte(mymock);

    g_autoOffState = TRUE;
    g_power = POWER_STATE_OFF;
    g_upPressed = TRUE;
    tick(1);

    EXPECT_EQ(g_setAutoOffFalseCalls, 1U);
    EXPECT_EQ(g_setAutoOffTrueCalls, 0U);
}

/*!
 * @rst
 *
 * .. test:: AutoOffTest.test_activity_keeps_cleared_state
 *    :id: TS_AO-008
 *    :tests: SWDD_AO-004
 *
 * @endrst
 */
TEST_F(AutoOffTest, test_activity_keeps_cleared_state)
{
    CREATE_MOCK(mymock);
    stubRte(mymock);

    g_autoOffState = FALSE;
    g_power = POWER_STATE_ON;
    g_downPressed = TRUE;
    tick(1);

    EXPECT_EQ(g_setAutoOffFalseCalls, 0U);
    EXPECT_EQ(g_setAutoOffTrueCalls, 0U);
}

/*!
 * @rst
 *
 * .. test:: AutoOffTest.test_up_key_resets_timer
 *    :id: TS_AO-009
 *    :tests: SWDD_AO-003
 *
 * @endrst
 */
TEST_F(AutoOffTest, test_up_key_resets_timer)
{
    CREATE_MOCK(mymock);
    stubRte(mymock);

    idle(POWER_STATE_ON, TIMEOUT_TICKS - 1);

    g_power = POWER_STATE_ON;
    g_upPressed = TRUE;
    tick(1);
    EXPECT_EQ(g_setPowerStateCalls, 0U);

    idle(POWER_STATE_ON, TIMEOUT_TICKS - 1);
    EXPECT_EQ(g_setPowerStateCalls, 0U);

    idle(POWER_STATE_ON, 1);
    EXPECT_EQ(g_setPowerStateOffCalls, 1U);
}

/*!
 * @rst
 *
 * .. test:: AutoOffTest.test_down_key_resets_timer
 *    :id: TS_AO-010
 *    :tests: SWDD_AO-003
 *
 * @endrst
 */
TEST_F(AutoOffTest, test_down_key_resets_timer)
{
    CREATE_MOCK(mymock);
    stubRte(mymock);

    idle(POWER_STATE_ON, TIMEOUT_TICKS - 1);

    g_power = POWER_STATE_ON;
    g_downPressed = TRUE;
    tick(1);
    EXPECT_EQ(g_setPowerStateCalls, 0U);

    idle(POWER_STATE_ON, TIMEOUT_TICKS - 1);
    EXPECT_EQ(g_setPowerStateCalls, 0U);

    idle(POWER_STATE_ON, 1);
    EXPECT_EQ(g_setPowerStateOffCalls, 1U);
}

/*!
 * @rst
 *
 * .. test:: AutoOffTest.test_auto_off_state_stays_true_while_idle
 *    :id: TS_AO-011
 *    :tests: SWDD_AO-004
 *
 * @endrst
 */
TEST_F(AutoOffTest, test_auto_off_state_stays_true_while_idle)
{
    CREATE_MOCK(mymock);
    stubRte(mymock);

    idle(POWER_STATE_ON, TIMEOUT_TICKS);
    EXPECT_EQ(g_setPowerStateOffCalls, 1U);
    EXPECT_EQ(g_setAutoOffTrueCalls, 1U);

    // The light is OFF and the auto-off state is TRUE, as the RTE now reports it
    g_autoOffState = TRUE;
    idle(POWER_STATE_OFF, TIMEOUT_TICKS);

    EXPECT_EQ(g_setAutoOffFalseCalls, 0U);
    EXPECT_EQ(g_setAutoOffTrueCalls, 1U);
}
