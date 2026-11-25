/**
 * @file
 */
#include <gtest/gtest.h>
using namespace testing;

extern "C"
{
#include "autoconf.h"
#include "auto_off.h"
#include "rte.h"
}

// Auto-generated mockups for this component
#include "mockup_components_auto_off.h"

namespace
{
const uint32_t timeoutTicks = CONFIG_AUTO_OFF_TIMEOUT * 1000 / CONFIG_OS_TASK_PERIOD;

/**
 * Allows the getters and RteSetAutoOffState any number of times, so gmock prints no
 * "uninteresting call" warnings. RteSetPowerState is left out on purpose: a call without an
 * explicit expectation must fail the test. Expectations added later take precedence.
 * Mock::VerifyAndClearExpectations removes these too, so every phase sets them again.
 */
void allowTickCalls(class_mockup &mock)
{
    EXPECT_CALL(mock, RteGetPowerState()).Times(AnyNumber());
    EXPECT_CALL(mock, RteGetPowerKeyPressedEvent()).Times(AnyNumber());
    EXPECT_CALL(mock, RteIsKeyPressed(_)).Times(AnyNumber());
    EXPECT_CALL(mock, RteSetAutoOffState(_)).Times(AnyNumber());
}

/** The inputs of one tick, set as defaults of the mock. */
void setInputs(class_mockup &mock, PowerState powerState, bool_t powerKeyEvent, bool_t upPressed, bool_t downPressed)
{
    allowTickCalls(mock);
    ON_CALL(mock, RteGetPowerState()).WillByDefault(Return(powerState));
    ON_CALL(mock, RteGetPowerKeyPressedEvent()).WillByDefault(Return(powerKeyEvent));
    ON_CALL(mock, RteIsKeyPressed(CONTROL_KEY_UP)).WillByDefault(Return(upPressed));
    ON_CALL(mock, RteIsKeyPressed(CONTROL_KEY_DOWN)).WillByDefault(Return(downPressed));
}

void setIdle(class_mockup &mock)
{
    setInputs(mock, POWER_STATE_ON, FALSE, FALSE, FALSE);
}

/** One tick with a power key event: the light is on and the timer starts from zero. */
void activityTick(class_mockup &mock)
{
    setInputs(mock, POWER_STATE_ON, TRUE, FALSE, FALSE);
    autoOff();
    setIdle(mock);
}

/** Runs the ticks and asserts that none of them sets the power state. */
void tickWithoutPowerStateChange(class_mockup &mock, uint32_t ticks)
{
    EXPECT_CALL(mock, RteSetPowerState(_)).Times(0);
    for (uint32_t i = 0; i < ticks; ++i)
    {
        autoOff();
    }
    Mock::VerifyAndClearExpectations(&mock);
    allowTickCalls(mock);
}

/** Runs one tick and asserts that it switches the light off. */
void tickSwitchesOff(class_mockup &mock)
{
    EXPECT_CALL(mock, RteSetPowerState(POWER_STATE_OFF)).Times(1);
    autoOff();
    Mock::VerifyAndClearExpectations(&mock);
    allowTickCalls(mock);
}
} // namespace

/**
 * @rst
 * .. test:: auto_off.test_turns_off_after_timeout
 *    :id: TS_AO-001
 *    :tests: SWDD_AO-100, SWDD_AO-200, SWDD_AO-202
 * @endrst
 */
TEST(auto_off, test_turns_off_after_timeout)
{
    CREATE_MOCK(mymock);
    activityTick(mymock);

    tickWithoutPowerStateChange(mymock, timeoutTicks - 1);

    EXPECT_CALL(mymock, RteSetPowerState(POWER_STATE_OFF)).Times(1);
    EXPECT_CALL(mymock, RteSetAutoOffState(TRUE)).Times(1);
    autoOff();
}

/**
 * @rst
 * .. test:: auto_off.test_no_timer_while_off
 *    :id: TS_AO-002
 *    :tests: SWDD_AO-101, SWDD_AO-201
 * @endrst
 */
TEST(auto_off, test_no_timer_while_off)
{
    CREATE_MOCK(mymock);
    activityTick(mymock);

    // Almost expired: old ticks must not carry over into the next ON phase
    tickWithoutPowerStateChange(mymock, timeoutTicks - 1);

    setInputs(mymock, POWER_STATE_OFF, FALSE, FALSE, FALSE);
    tickWithoutPowerStateChange(mymock, 2 * timeoutTicks);

    setIdle(mymock);
    tickWithoutPowerStateChange(mymock, timeoutTicks - 1);
}

/**
 * @rst
 * .. test:: auto_off.test_up_key_resets_timer
 *    :id: TS_AO-003
 *    :tests: SWDD_AO-102, SWDD_AO-201
 * @endrst
 */
TEST(auto_off, test_up_key_resets_timer)
{
    CREATE_MOCK(mymock);
    activityTick(mymock);

    tickWithoutPowerStateChange(mymock, timeoutTicks - 1);

    setInputs(mymock, POWER_STATE_ON, FALSE, TRUE, FALSE);
    tickWithoutPowerStateChange(mymock, 1);

    setIdle(mymock);
    tickWithoutPowerStateChange(mymock, timeoutTicks - 1);
    tickSwitchesOff(mymock);
}

/**
 * @rst
 * .. test:: auto_off.test_down_key_resets_timer
 *    :id: TS_AO-004
 *    :tests: SWDD_AO-102, SWDD_AO-201
 * @endrst
 */
TEST(auto_off, test_down_key_resets_timer)
{
    CREATE_MOCK(mymock);
    activityTick(mymock);

    tickWithoutPowerStateChange(mymock, timeoutTicks - 1);

    setInputs(mymock, POWER_STATE_ON, FALSE, FALSE, TRUE);
    tickWithoutPowerStateChange(mymock, 1);

    setIdle(mymock);
    tickWithoutPowerStateChange(mymock, timeoutTicks - 1);
    tickSwitchesOff(mymock);
}

/**
 * @rst
 * .. test:: auto_off.test_held_key_keeps_light_on
 *    :id: TS_AO-005
 *    :tests: SWDD_AO-102
 * @endrst
 */
TEST(auto_off, test_held_key_keeps_light_on)
{
    CREATE_MOCK(mymock);
    activityTick(mymock);

    setInputs(mymock, POWER_STATE_ON, FALSE, TRUE, FALSE);
    tickWithoutPowerStateChange(mymock, 3 * timeoutTicks);
}

/**
 * @rst
 * .. test:: auto_off.test_power_key_event_resets_timer
 *    :id: TS_AO-006
 *    :tests: SWDD_AO-102, SWDD_AO-201
 * @endrst
 */
TEST(auto_off, test_power_key_event_resets_timer)
{
    CREATE_MOCK(mymock);
    activityTick(mymock);

    tickWithoutPowerStateChange(mymock, timeoutTicks - 1);

    setInputs(mymock, POWER_STATE_ON, TRUE, FALSE, FALSE);
    tickWithoutPowerStateChange(mymock, 1);

    setIdle(mymock);
    tickWithoutPowerStateChange(mymock, timeoutTicks - 1);
    tickSwitchesOff(mymock);
}

/**
 * @rst
 * .. test:: auto_off.test_state_false_after_next_activity
 *    :id: TS_AO-007
 *    :tests: SWDD_AO-103, SWDD_AO-202
 * @endrst
 */
TEST(auto_off, test_state_false_after_next_activity)
{
    CREATE_MOCK(mymock);
    activityTick(mymock);

    tickWithoutPowerStateChange(mymock, timeoutTicks - 1);
    tickSwitchesOff(mymock);

    // The light is off now; the state stays TRUE until the next activity
    setInputs(mymock, POWER_STATE_OFF, FALSE, FALSE, FALSE);
    EXPECT_CALL(mymock, RteSetAutoOffState(TRUE)).Times(5);
    for (int i = 0; i < 5; ++i)
    {
        autoOff();
    }
    Mock::VerifyAndClearExpectations(&mymock);
    allowTickCalls(mymock);

    setInputs(mymock, POWER_STATE_OFF, FALSE, TRUE, FALSE);
    EXPECT_CALL(mymock, RteSetAutoOffState(FALSE)).Times(1);
    autoOff();
}
