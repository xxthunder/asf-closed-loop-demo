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

#include "mockup_components_auto_off.h"

/* Timeout in ticks, derived from the configuration (Sleep variant: 10 s / 10 ms = 1000 ticks). */
static const unsigned int AUTO_OFF_TIMEOUT_TICKS = (CONFIG_AUTO_OFF_TIMEOUT * 1000) / CONFIG_OS_TASK_PERIOD;

class AutoOffTest : public Test
{
protected:
    void SetUp() override
    {
        autoOffInit();
    }

    /* Set what the RTE returns from now on. Later calls override earlier ones. */
    template <typename Mock>
    static void setInputs(Mock &mock, PowerState power, bool up, bool down, bool powerKeyEvent)
    {
        ON_CALL(mock, RteGetPowerState()).WillByDefault(Return(power));
        ON_CALL(mock, RteIsKeyPressed(CONTROL_KEY_UP)).WillByDefault(Return(up ? TRUE : FALSE));
        ON_CALL(mock, RteIsKeyPressed(CONTROL_KEY_DOWN)).WillByDefault(Return(down ? TRUE : FALSE));
        ON_CALL(mock, RteGetPowerKeyPressedEvent()).WillByDefault(Return(powerKeyEvent ? TRUE : FALSE));
    }

    template <typename Mock>
    static void idle(Mock &mock)
    {
        setInputs(mock, POWER_STATE_ON, false, false, false);
    }

    static void run(unsigned int ticks)
    {
        for (unsigned int i = 0; i < ticks; i++)
        {
            autoOff();
        }
    }
};

/*!
 * @rst
 *
 * .. test:: AutoOffTest.SwitchesOffAfterTimeout
 *    :id: TS_AO-001
 *    :tests: SWDD_AO-001, SWDD_AO-004
 *
 * @endrst
 */
TEST_F(AutoOffTest, SwitchesOffAfterTimeout)
{
    NiceMock<class_mockup> mymock;
    idle(mymock);

    // Nothing happens before the last tick.
    EXPECT_CALL(mymock, RteSetPowerState(_)).Times(0);
    EXPECT_CALL(mymock, RteSetAutoOffState(_)).Times(0);
    run(AUTO_OFF_TIMEOUT_TICKS - 1);
    Mock::VerifyAndClearExpectations(&mymock);

    // The last tick switches the power off and sets the auto-off state.
    EXPECT_CALL(mymock, RteSetPowerState(POWER_STATE_OFF)).Times(1);
    EXPECT_CALL(mymock, RteSetAutoOffState(TRUE)).Times(1);
    autoOff();
}

/*!
 * @rst
 *
 * .. test:: AutoOffTest.StaysOnOneTickBeforeTimeout
 *    :id: TS_AO-002
 *    :tests: SWDD_AO-001
 *
 * @endrst
 */
TEST_F(AutoOffTest, StaysOnOneTickBeforeTimeout)
{
    NiceMock<class_mockup> mymock;
    idle(mymock);

    EXPECT_CALL(mymock, RteSetPowerState(_)).Times(0);
    run(AUTO_OFF_TIMEOUT_TICKS - 1);
}

/*!
 * @rst
 *
 * .. test:: AutoOffTest.UpKeyResetsTimer
 *    :id: TS_AO-003
 *    :tests: SWDD_AO-002
 *
 * @endrst
 */
TEST_F(AutoOffTest, UpKeyResetsTimer)
{
    NiceMock<class_mockup> mymock;
    EXPECT_CALL(mymock, RteSetPowerState(_)).Times(0);

    idle(mymock);
    run(AUTO_OFF_TIMEOUT_TICKS - 1);

    setInputs(mymock, POWER_STATE_ON, true, false, false);
    autoOff();

    idle(mymock);
    run(AUTO_OFF_TIMEOUT_TICKS - 1);
}

/*!
 * @rst
 *
 * .. test:: AutoOffTest.DownKeyResetsTimer
 *    :id: TS_AO-004
 *    :tests: SWDD_AO-002
 *
 * @endrst
 */
TEST_F(AutoOffTest, DownKeyResetsTimer)
{
    NiceMock<class_mockup> mymock;
    EXPECT_CALL(mymock, RteSetPowerState(_)).Times(0);

    idle(mymock);
    run(AUTO_OFF_TIMEOUT_TICKS - 1);

    setInputs(mymock, POWER_STATE_ON, false, true, false);
    autoOff();

    idle(mymock);
    run(AUTO_OFF_TIMEOUT_TICKS - 1);
}

/*!
 * @rst
 *
 * .. test:: AutoOffTest.HeldKeyKeepsLightOn
 *    :id: TS_AO-005
 *    :tests: SWDD_AO-002
 *
 * @endrst
 */
TEST_F(AutoOffTest, HeldKeyKeepsLightOn)
{
    NiceMock<class_mockup> mymock;
    EXPECT_CALL(mymock, RteSetPowerState(_)).Times(0);

    setInputs(mymock, POWER_STATE_ON, true, false, false);
    run(2 * AUTO_OFF_TIMEOUT_TICKS);
}

/*!
 * @rst
 *
 * .. test:: AutoOffTest.PowerKeyEventResetsTimer
 *    :id: TS_AO-006
 *    :tests: SWDD_AO-002
 *
 * @endrst
 */
TEST_F(AutoOffTest, PowerKeyEventResetsTimer)
{
    NiceMock<class_mockup> mymock;
    EXPECT_CALL(mymock, RteSetPowerState(_)).Times(0);

    idle(mymock);
    run(AUTO_OFF_TIMEOUT_TICKS - 1);

    setInputs(mymock, POWER_STATE_ON, false, false, true);
    autoOff();

    idle(mymock);
    run(AUTO_OFF_TIMEOUT_TICKS - 1);
}

/*!
 * @rst
 *
 * .. test:: AutoOffTest.TimerResetsWhileOff
 *    :id: TS_AO-007
 *    :tests: SWDD_AO-003
 *
 * @endrst
 */
TEST_F(AutoOffTest, TimerResetsWhileOff)
{
    NiceMock<class_mockup> mymock;
    EXPECT_CALL(mymock, RteSetPowerState(_)).Times(0);

    idle(mymock);
    run(AUTO_OFF_TIMEOUT_TICKS - 1);

    setInputs(mymock, POWER_STATE_OFF, false, false, false);
    autoOff();

    idle(mymock);
    run(AUTO_OFF_TIMEOUT_TICKS - 1);
}

/*!
 * @rst
 *
 * .. test:: AutoOffTest.RestartsAfterPowerOn
 *    :id: TS_AO-008
 *    :tests: SWDD_AO-001, SWDD_AO-003
 *
 * @endrst
 */
TEST_F(AutoOffTest, RestartsAfterPowerOn)
{
    NiceMock<class_mockup> mymock;
    idle(mymock);

    // Run to the timeout: the light goes off.
    EXPECT_CALL(mymock, RteSetPowerState(POWER_STATE_OFF)).Times(1);
    EXPECT_CALL(mymock, RteSetAutoOffState(TRUE)).Times(1);
    run(AUTO_OFF_TIMEOUT_TICKS);
    Mock::VerifyAndClearExpectations(&mymock);

    // The power state is OFF, then the user switches it ON again.
    setInputs(mymock, POWER_STATE_OFF, false, false, false);
    autoOff();
    idle(mymock);

    // The next switch-off comes only after another full timeout.
    EXPECT_CALL(mymock, RteSetPowerState(_)).Times(0);
    run(AUTO_OFF_TIMEOUT_TICKS - 1);
    Mock::VerifyAndClearExpectations(&mymock);

    EXPECT_CALL(mymock, RteSetPowerState(POWER_STATE_OFF)).Times(1);
    EXPECT_CALL(mymock, RteSetAutoOffState(TRUE)).Times(1);
    autoOff();
}

/*!
 * @rst
 *
 * .. test:: AutoOffTest.ActivityClearsAutoOffState
 *    :id: TS_AO-009
 *    :tests: SWDD_AO-002, SWDD_AO-004
 *
 * @endrst
 */
TEST_F(AutoOffTest, ActivityClearsAutoOffState)
{
    NiceMock<class_mockup> mymock;
    idle(mymock);

    EXPECT_CALL(mymock, RteSetPowerState(POWER_STATE_OFF)).Times(1);
    EXPECT_CALL(mymock, RteSetAutoOffState(TRUE)).Times(1);
    run(AUTO_OFF_TIMEOUT_TICKS);
    Mock::VerifyAndClearExpectations(&mymock);

    // One activity after the timeout clears the auto-off state.
    setInputs(mymock, POWER_STATE_OFF, true, false, false);
    EXPECT_CALL(mymock, RteSetAutoOffState(FALSE)).Times(1);
    autoOff();
}

/*!
 * @rst
 *
 * .. test:: AutoOffTest.NoStateChangeWithoutTimeout
 *    :id: TS_AO-010
 *    :tests: SWDD_AO-004
 *
 * @endrst
 */
TEST_F(AutoOffTest, NoStateChangeWithoutTimeout)
{
    NiceMock<class_mockup> mymock;
    idle(mymock);

    EXPECT_CALL(mymock, RteSetAutoOffState(_)).Times(0);
    run(AUTO_OFF_TIMEOUT_TICKS - 1);
}
