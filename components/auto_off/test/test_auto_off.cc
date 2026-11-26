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

    extern uint32_t autoOffTicks;
}

// Auto-generated mockups for this component
#include "mockup_components_auto_off.h"

namespace
{
/** Timeout in ticks, derived from the configuration. */
constexpr uint32_t timeoutTicks = CONFIG_AUTO_OFF_TIMEOUT * 1000 / CONFIG_OS_TASK_PERIOD;

/** Simulated RTE inputs and a record of the outputs written by the unit under test. */
struct Scenario
{
    PowerState powerState = POWER_STATE_ON;
    bool_t upPressed = FALSE;
    bool_t downPressed = FALSE;
    bool_t powerEvent = FALSE;
    int setPowerOffCount = 0;
    int setPowerOtherCount = 0;
    int autoOffTrueCount = 0;
    int autoOffFalseCount = 0;
};

template <typename Mock>
void wire(Mock &mock, Scenario &s)
{
    /* Calls are allowed in any number; the behaviour is defined by the ON_CALLs below and checked via Scenario. */
    EXPECT_CALL(mock, RteGetPowerState()).Times(AnyNumber());
    EXPECT_CALL(mock, RteGetPowerKeyPressedEvent()).Times(AnyNumber());
    EXPECT_CALL(mock, RteIsKeyPressed(_)).Times(AnyNumber());
    EXPECT_CALL(mock, RteSetPowerState(_)).Times(AnyNumber());
    EXPECT_CALL(mock, RteSetAutoOffState(_)).Times(AnyNumber());

    ON_CALL(mock, RteGetPowerState()).WillByDefault(Invoke([&s]() { return s.powerState; }));
    ON_CALL(mock, RteGetPowerKeyPressedEvent()).WillByDefault(Invoke([&s]() { return s.powerEvent; }));
    ON_CALL(mock, RteIsKeyPressed(_)).WillByDefault(Return(FALSE));
    ON_CALL(mock, RteIsKeyPressed(CONTROL_KEY_UP)).WillByDefault(Invoke([&s](int32_t) { return s.upPressed; }));
    ON_CALL(mock, RteIsKeyPressed(CONTROL_KEY_DOWN)).WillByDefault(Invoke([&s](int32_t) { return s.downPressed; }));
    ON_CALL(mock, RteSetPowerState(_)).WillByDefault(Invoke([&s](PowerState p) {
        if (p == POWER_STATE_OFF)
        {
            s.setPowerOffCount++;
        }
        else
        {
            s.setPowerOtherCount++;
        }
    }));
    ON_CALL(mock, RteSetAutoOffState(_)).WillByDefault(Invoke([&s](bool_t v) {
        if (v == TRUE)
        {
            s.autoOffTrueCount++;
        }
        else
        {
            s.autoOffFalseCount++;
        }
    }));
}

void run(uint32_t calls)
{
    for (uint32_t i = 0; i < calls; ++i)
    {
        autoOff();
    }
}
} // namespace

/**
 * @rst
 * .. test:: auto_off.test_switches_off_after_timeout
 *    :id: TS_AO-001
 *    :tests: SWDD_AO-100, SWDD_AO-101, SWDD_AO-200, SWDD_AO-202, SWDD_AO-203
 * @endrst
 */
TEST(auto_off, test_switches_off_after_timeout)
{
    /* Arrange */
    CREATE_MOCK(mymock);
    Scenario s;
    wire(mymock, s);
    autoOffTicks = 0;

    /* Act / Assert: one tick before the timeout nothing happens */
    run(timeoutTicks - 1);
    EXPECT_EQ(s.setPowerOffCount, 0);
    EXPECT_EQ(s.setPowerOtherCount, 0);
    EXPECT_EQ(s.autoOffTrueCount, 0);

    /* Act / Assert: exactly at the timeout tick the light goes off */
    run(1);
    EXPECT_EQ(s.setPowerOffCount, 1);
    EXPECT_EQ(s.setPowerOtherCount, 0);
    EXPECT_EQ(s.autoOffTrueCount, 1);
}

/**
 * @rst
 * .. test:: auto_off.test_timer_runs_only_while_on
 *    :id: TS_AO-002
 *    :tests: SWDD_AO-101, SWDD_AO-200
 * @endrst
 */
TEST(auto_off, test_timer_runs_only_while_on)
{
    /* Arrange */
    CREATE_MOCK(mymock);
    Scenario s;
    wire(mymock, s);
    autoOffTicks = 0;
    s.powerState = POWER_STATE_OFF;

    /* Act: light is off for twice the timeout */
    run(2 * timeoutTicks);

    /* Assert */
    EXPECT_EQ(s.setPowerOffCount, 0);
    EXPECT_EQ(s.setPowerOtherCount, 0);

    /* Act: light is switched on, the full timeout runs again */
    s.powerState = POWER_STATE_ON;
    run(timeoutTicks - 1);
    EXPECT_EQ(s.setPowerOffCount, 0);
    run(1);
    EXPECT_EQ(s.setPowerOffCount, 1);
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
    /* Arrange */
    CREATE_MOCK(mymock);
    Scenario s;
    wire(mymock, s);
    autoOffTicks = 0;

    /* Act */
    run(timeoutTicks - 1);
    s.upPressed = TRUE;
    run(1);
    s.upPressed = FALSE;
    run(timeoutTicks - 1);

    /* Assert */
    EXPECT_EQ(s.setPowerOffCount, 0);
    run(1);
    EXPECT_EQ(s.setPowerOffCount, 1);
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
    /* Arrange */
    CREATE_MOCK(mymock);
    Scenario s;
    wire(mymock, s);
    autoOffTicks = 0;

    /* Act */
    run(timeoutTicks - 1);
    s.downPressed = TRUE;
    run(1);
    s.downPressed = FALSE;
    run(timeoutTicks - 1);

    /* Assert */
    EXPECT_EQ(s.setPowerOffCount, 0);
    run(1);
    EXPECT_EQ(s.setPowerOffCount, 1);
}

/**
 * @rst
 * .. test:: auto_off.test_held_key_keeps_light_on
 *    :id: TS_AO-005
 *    :tests: SWDD_AO-102, SWDD_AO-201
 * @endrst
 */
TEST(auto_off, test_held_key_keeps_light_on)
{
    /* Arrange */
    CREATE_MOCK(mymock);
    Scenario s;
    wire(mymock, s);
    autoOffTicks = 0;
    s.upPressed = TRUE;

    /* Act: key held for twice the timeout */
    run(2 * timeoutTicks);

    /* Assert */
    EXPECT_EQ(s.setPowerOffCount, 0);
    EXPECT_EQ(s.setPowerOtherCount, 0);
    EXPECT_EQ(s.autoOffTrueCount, 0);
}

/**
 * @rst
 * .. test:: auto_off.test_power_event_resets_timer
 *    :id: TS_AO-006
 *    :tests: SWDD_AO-102, SWDD_AO-201
 * @endrst
 */
TEST(auto_off, test_power_event_resets_timer)
{
    /* Arrange */
    CREATE_MOCK(mymock);
    Scenario s;
    wire(mymock, s);
    autoOffTicks = 0;

    /* Act: the power key event is seen in exactly one tick */
    run(timeoutTicks - 1);
    s.powerEvent = TRUE;
    run(1);
    s.powerEvent = FALSE;
    run(timeoutTicks - 1);

    /* Assert */
    EXPECT_EQ(s.setPowerOffCount, 0);
    run(1);
    EXPECT_EQ(s.setPowerOffCount, 1);
}

/**
 * @rst
 * .. test:: auto_off.test_auto_off_state_cleared_by_use
 *    :id: TS_AO-007
 *    :tests: SWDD_AO-103, SWDD_AO-203
 * @endrst
 */
TEST(auto_off, test_auto_off_state_cleared_by_use)
{
    /* Arrange */
    CREATE_MOCK(mymock);
    Scenario s;
    wire(mymock, s);
    autoOffTicks = 0;
    run(timeoutTicks);
    ASSERT_EQ(s.autoOffTrueCount, 1);
    ASSERT_EQ(s.autoOffFalseCount, 0);

    /* Act: the user presses the power key after the automatic switch-off */
    s.powerState = POWER_STATE_OFF;
    s.powerEvent = TRUE;
    run(1);

    /* Assert */
    EXPECT_GE(s.autoOffFalseCount, 1);
    EXPECT_EQ(s.autoOffTrueCount, 1);
}

/**
 * @rst
 * .. test:: auto_off.test_no_repeated_switch_off
 *    :id: TS_AO-008
 *    :tests: SWDD_AO-100, SWDD_AO-101, SWDD_AO-202
 * @endrst
 */
TEST(auto_off, test_no_repeated_switch_off)
{
    /* Arrange */
    CREATE_MOCK(mymock);
    Scenario s;
    wire(mymock, s);
    autoOffTicks = 0;
    run(timeoutTicks);
    ASSERT_EQ(s.setPowerOffCount, 1);

    /* Act: the power state now reads OFF, nothing else happens */
    s.powerState = POWER_STATE_OFF;
    run(timeoutTicks);

    /* Assert */
    EXPECT_EQ(s.setPowerOffCount, 1);
    EXPECT_EQ(s.autoOffTrueCount, 1);
}

/**
 * @rst
 * .. test:: auto_off.test_power_event_clears_auto_off_state_without_switching_off
 *    :id: TS_AO-009
 *    :tests: SWDD_AO-102, SWDD_AO-103
 * @endrst
 */
TEST(auto_off, test_power_event_clears_auto_off_state_without_switching_off)
{
    /* Arrange: light is on, one tick before the timeout */
    CREATE_MOCK(mymock);
    Scenario s;
    wire(mymock, s);
    autoOffTicks = 0;
    run(timeoutTicks - 1);

    /* Act: power key event on the tick that would switch the light off */
    s.powerEvent = TRUE;
    run(1);

    /* Assert */
    EXPECT_EQ(s.autoOffFalseCount, 1);
    EXPECT_EQ(s.autoOffTrueCount, 0);
    EXPECT_EQ(s.setPowerOffCount, 0);
    EXPECT_EQ(s.setPowerOtherCount, 0);
}
