/**
 * @file
 */

#include <gtest/gtest.h>
using namespace testing;

extern "C"
{
#include "auto_off.h"
#include "rte.h"
#include "autoconf.h"
}

#include "mockup_components_auto_off.h"

namespace
{
/* Number of autoOff() cycles that make up the configured timeout. */
const uint32_t timeoutTicks = (CONFIG_AUTO_OFF_TIMEOUT * 1000u) / CONFIG_OS_TASK_PERIOD;

/* Inputs the mocked RTE delivers to autoOff() in the following cycles. */
struct Inputs
{
    bool powerKeyEvent;
    bool up;
    bool down;
    PowerState powerState;
};

const Inputs idleOn = {false, false, false, POWER_STATE_ON};
const Inputs idleOff = {false, false, false, POWER_STATE_OFF};

template <class Mock>
void setInputs(Mock &mock, const Inputs &inputs)
{
    ON_CALL(mock, RteGetPowerKeyPressedEvent()).WillByDefault(Return(inputs.powerKeyEvent ? TRUE : FALSE));
    ON_CALL(mock, RteIsKeyPressed(_)).WillByDefault(Return(FALSE));
    ON_CALL(mock, RteIsKeyPressed(CONTROL_KEY_UP)).WillByDefault(Return(inputs.up ? TRUE : FALSE));
    ON_CALL(mock, RteIsKeyPressed(CONTROL_KEY_DOWN)).WillByDefault(Return(inputs.down ? TRUE : FALSE));
    ON_CALL(mock, RteGetPowerState()).WillByDefault(Return(inputs.powerState));
    EXPECT_CALL(mock, RteGetPowerKeyPressedEvent()).Times(AnyNumber());
    EXPECT_CALL(mock, RteIsKeyPressed(_)).Times(AnyNumber());
    EXPECT_CALL(mock, RteGetPowerState()).Times(AnyNumber());
}

/* Runs the given number of cycles with the given inputs; no cycle may set the power state or the Auto Off state to TRUE.
 * Clearing the Auto Off state (FALSE) on an operation is allowed. */
template <class Mock>
void runWithoutSwitchOff(Mock &mock, const Inputs &inputs, uint32_t cycles)
{
    setInputs(mock, inputs);
    EXPECT_CALL(mock, RteSetPowerState(_)).Times(0);
    EXPECT_CALL(mock, RteSetAutoOffState(TRUE)).Times(0);
    EXPECT_CALL(mock, RteSetAutoOffState(FALSE)).Times(AnyNumber());
    for (uint32_t i = 0; i < cycles; i++)
    {
        autoOff();
    }
    testing::Mock::VerifyAndClearExpectations(&mock);
}

/* Runs one cycle with the given inputs and expects exactly one switch-off with the Auto Off state TRUE. */
template <class Mock>
void runExpectingSwitchOff(Mock &mock, const Inputs &inputs)
{
    setInputs(mock, inputs);
    EXPECT_CALL(mock, RteSetPowerState(POWER_STATE_OFF)).Times(1);
    EXPECT_CALL(mock, RteSetAutoOffState(TRUE)).Times(1);
    autoOff();
    testing::Mock::VerifyAndClearExpectations(&mock);
}
} // namespace

class AutoOffTest : public Test
{
protected:
    void SetUp() override
    {
        autoOffInit();
    }
};

/*!
 * @rst
 *
 * .. test:: AutoOffTest.SwitchesOffAfterTimeout
 *    :id: TS_AO-001
 *    :tests: SWDD_AO-100, SWDD_AO-103
 *
 * @endrst
 */
TEST_F(AutoOffTest, SwitchesOffAfterTimeout)
{
    CREATE_MOCK(mymock);

    runWithoutSwitchOff(mymock, idleOn, timeoutTicks - 1);
    runExpectingSwitchOff(mymock, idleOn);
}

/*!
 * @rst
 *
 * .. test:: AutoOffTest.UpDownKeyResetsTimer
 *    :id: TS_AO-002
 *    :tests: SWDD_AO-102
 *
 * @endrst
 */
TEST_F(AutoOffTest, UpDownKeyResetsTimer)
{
    CREATE_MOCK(mymock);

    const Inputs upPressed = {false, true, false, POWER_STATE_ON};
    const Inputs downPressed = {false, false, true, POWER_STATE_ON};
    const Inputs keys[] = {upPressed, downPressed};

    for (const Inputs &key : keys)
    {
        autoOffInit();
        runWithoutSwitchOff(mymock, idleOn, timeoutTicks - 1);
        runWithoutSwitchOff(mymock, key, 1);
        runWithoutSwitchOff(mymock, idleOn, timeoutTicks - 1);
    }
}

/*!
 * @rst
 *
 * .. test:: AutoOffTest.PowerKeyEventResetsTimer
 *    :id: TS_AO-003
 *    :tests: SWDD_AO-102
 *
 * @endrst
 */
TEST_F(AutoOffTest, PowerKeyEventResetsTimer)
{
    CREATE_MOCK(mymock);

    const Inputs powerKeyEvent = {true, false, false, POWER_STATE_ON};

    runWithoutSwitchOff(mymock, idleOn, timeoutTicks - 1);
    runWithoutSwitchOff(mymock, powerKeyEvent, 1);
    runWithoutSwitchOff(mymock, idleOn, timeoutTicks - 1);
}

/*!
 * @rst
 *
 * .. test:: AutoOffTest.TimerStopsWhilePowerOff
 *    :id: TS_AO-004
 *    :tests: SWDD_AO-101
 *
 * @endrst
 */
TEST_F(AutoOffTest, TimerStopsWhilePowerOff)
{
    CREATE_MOCK(mymock);

    runWithoutSwitchOff(mymock, idleOn, timeoutTicks - 1);
    runWithoutSwitchOff(mymock, idleOff, 2 * timeoutTicks);
    runWithoutSwitchOff(mymock, idleOn, timeoutTicks - 1);
    runExpectingSwitchOff(mymock, idleOn);
}

/*!
 * @rst
 *
 * .. test:: AutoOffTest.HeldKeyKeepsLightOn
 *    :id: TS_AO-005
 *    :tests: SWDD_AO-102
 *
 * @endrst
 */
TEST_F(AutoOffTest, HeldKeyKeepsLightOn)
{
    CREATE_MOCK(mymock);

    const Inputs upPressed = {false, true, false, POWER_STATE_ON};

    runWithoutSwitchOff(mymock, upPressed, 2 * timeoutTicks);
}

/*!
 * @rst
 *
 * .. test:: AutoOffTest.AutoOffStateClearedByNextOperation
 *    :id: TS_AO-006
 *    :tests: SWDD_AO-103, SWDD_AO-201
 *
 * @endrst
 */
TEST_F(AutoOffTest, AutoOffStateClearedByNextOperation)
{
    CREATE_MOCK(mymock);

    runWithoutSwitchOff(mymock, idleOn, timeoutTicks - 1);
    runExpectingSwitchOff(mymock, idleOn);

    // Nobody touches anything: the Auto Off state stays TRUE.
    setInputs(mymock, idleOff);
    EXPECT_CALL(mymock, RteSetAutoOffState(_)).Times(0);
    EXPECT_CALL(mymock, RteSetPowerState(_)).Times(0);
    for (uint32_t i = 0; i < timeoutTicks; i++)
    {
        autoOff();
    }
    testing::Mock::VerifyAndClearExpectations(&mymock);

    // The next operation clears it.
    const Inputs powerKeyEvent = {true, false, false, POWER_STATE_OFF};
    setInputs(mymock, powerKeyEvent);
    EXPECT_CALL(mymock, RteSetAutoOffState(FALSE)).Times(1);
    autoOff();
    testing::Mock::VerifyAndClearExpectations(&mymock);
}

/*!
 * @rst
 *
 * .. test:: AutoOffTest.EachOperationClearsAutoOffState
 *    :id: TS_AO-007
 *    :tests: SWDD_AO-102, SWDD_AO-201
 *
 * @endrst
 */
TEST_F(AutoOffTest, EachOperationClearsAutoOffState)
{
    CREATE_MOCK(mymock);

    const Inputs powerKeyEvent = {true, false, false, POWER_STATE_OFF};
    const Inputs upPressed = {false, true, false, POWER_STATE_OFF};
    const Inputs downPressed = {false, false, true, POWER_STATE_OFF};
    const Inputs operations[] = {powerKeyEvent, upPressed, downPressed};

    for (const Inputs &operation : operations)
    {
        autoOffInit();
        runWithoutSwitchOff(mymock, idleOn, timeoutTicks - 1);
        runExpectingSwitchOff(mymock, idleOn);

        setInputs(mymock, operation);
        EXPECT_CALL(mymock, RteSetAutoOffState(FALSE)).Times(1);
        EXPECT_CALL(mymock, RteSetAutoOffState(TRUE)).Times(0);
        EXPECT_CALL(mymock, RteSetPowerState(_)).Times(0);
        autoOff();
        testing::Mock::VerifyAndClearExpectations(&mymock);
    }
}
