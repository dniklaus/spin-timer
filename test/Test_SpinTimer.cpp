#include <gtest/gtest.h>
#include <tuple>

#include "SpinTimer.h"
#include "UptimeInfo.h"
#include "Mock_UptimeInfo.h"
#include "Mock_SpinTimerAction.h"

using ::testing::AtLeast;

////////////////////////////////////////////////////////////////////////////////////////////////////
// Helper Classes

// First: delayMillis Second: startMillis
typedef std::tuple<unsigned long int, unsigned long int> SpinTimerSingleShotTestParam;

class SpinTimerSingleShot : public ::testing::TestWithParam<SpinTimerSingleShotTestParam>
{
public:
  SpinTimerSingleShot() : timer(10, nullptr, SpinTimer::IS_NON_RECURRING, SpinTimer::IS_NON_AUTOSTART){};

protected:
  SpinTimer timer;
};

typedef struct SpinTimerRecurringTestParam
{
  unsigned long int delayMillis;
  unsigned long int startMillis;
  unsigned long int numOfLoops;
  SpinTimerRecurringTestParam(unsigned long int delay, unsigned long int start, unsigned long int loops) : delayMillis(delay), startMillis(start), numOfLoops(loops){};
} SpinTimerRecurringTestParam;

class SpinTimerRecurring : public ::testing::TestWithParam<SpinTimerRecurringTestParam>
{
public:
  SpinTimerRecurring() : timer(10, nullptr, SpinTimer::IS_RECURRING, SpinTimer::IS_NON_AUTOSTART){};

protected:
  SpinTimer timer;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// Timer Creation Tests

TEST(SpinTimer, timer_create_allDefaults_test)
{
  const unsigned long int delayMillis = 10;

  SpinTimer timer(delayMillis);
  EXPECT_FALSE(timer.isRunning());
  EXPECT_FALSE(timer.isExpired());
  EXPECT_EQ(timer.getInterval(), delayMillis);
  EXPECT_EQ(timer.action(), nullptr);
}

TEST(SpinTimer, timer_create_autostart_test)
{
  const unsigned long int delayMillis = 10;

  SpinTimer timer(delayMillis, nullptr, SpinTimer::IS_NON_RECURRING, SpinTimer::IS_AUTOSTART);
  EXPECT_TRUE(timer.isRunning());
  EXPECT_FALSE(timer.isExpired());
  EXPECT_EQ(timer.getInterval(), delayMillis);
  EXPECT_EQ(timer.action(), nullptr);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Timer Start Polling Tests

TEST_P(SpinTimerSingleShot, timer_polling_start_tests)
{
  unsigned long int delayMillis = std::get<0>(GetParam());
  unsigned long int startMillis = std::get<1>(GetParam());
  unsigned long int expEndMillis = startMillis + delayMillis;

  Mock_UptimeInfo uptimeInfo(startMillis);
  uptimeInfo.setTMillis(startMillis);
  UptimeInfo::Instance()->setAdapter(&uptimeInfo);

  timer.start(delayMillis);
  EXPECT_EQ(timer.getInterval(), delayMillis);
  EXPECT_TRUE(timer.isRunning());

  while (!timer.isExpired())
  {
    EXPECT_TRUE(timer.isRunning());
    uptimeInfo.incrementTMillis();
  }

  EXPECT_EQ(timer.isExpired(), false);
  EXPECT_EQ(timer.isRunning(), false);
  EXPECT_EQ(uptimeInfo.tMillis(), expEndMillis);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Timer Tick and Callback Tests

TEST_P(SpinTimerSingleShot, timer_tickAndCallback_tests)
{
  unsigned long int delayMillis = std::get<0>(GetParam());
  unsigned long int startMillis = std::get<1>(GetParam());
  unsigned long int expEndMillis = startMillis + delayMillis;

  Mock_UptimeInfo uptimeInfo(startMillis);
  uptimeInfo.setTMillis(startMillis);
  UptimeInfo::Instance()->setAdapter(&uptimeInfo);
  
  Mock_SpinTimerAction timerAction;
  timer.attachAction(&timerAction);

  EXPECT_CALL(timerAction, timeExpired)
      .Times(AtLeast(1));

  timer.start(delayMillis);
  EXPECT_EQ(timer.getInterval(), delayMillis);
  EXPECT_TRUE(timer.isRunning());

  while (uptimeInfo.tMillis() != expEndMillis)
  {
    EXPECT_TRUE(timer.isRunning());
    timer.tick();
    uptimeInfo.incrementTMillis();
  }

  EXPECT_TRUE(timer.isExpired());
  EXPECT_FALSE(timer.isExpired());
  EXPECT_FALSE(timer.isRunning());
  EXPECT_EQ(uptimeInfo.tMillis(), expEndMillis);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Timer Recurring Tests

TEST_P(SpinTimerRecurring, timer_recurringTimer_test)
{
  unsigned long int delayMillis = GetParam().delayMillis;
  unsigned long int startMillis = GetParam().startMillis;
  unsigned long int numOfLoops = GetParam().numOfLoops;
  unsigned long int expEndMillis = startMillis + numOfLoops * delayMillis;

  Mock_UptimeInfo uptimeInfo(startMillis);
  uptimeInfo.setTMillis(startMillis);
  UptimeInfo::Instance()->setAdapter(&uptimeInfo);

  Mock_SpinTimerAction timerAction;
  timer.attachAction(&timerAction);

  EXPECT_CALL(timerAction, timeExpired)
      .Times(AtLeast(numOfLoops));

  timer.start(delayMillis);
  EXPECT_EQ(timer.getInterval(), delayMillis);
  EXPECT_TRUE(timer.isRunning());

  while (uptimeInfo.tMillis() != expEndMillis)
  {
    EXPECT_TRUE(timer.isRunning());
    timer.tick();
    uptimeInfo.incrementTMillis();
  }

  EXPECT_TRUE(timer.isExpired());
  EXPECT_FALSE(timer.isExpired());
  EXPECT_TRUE(timer.isRunning());
  EXPECT_EQ(uptimeInfo.tMillis(), expEndMillis);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Setup tests for test cases with multiple parameters

INSTANTIATE_TEST_CASE_P(
    SpinTimer,
    SpinTimerSingleShot,
    ::testing::Values(
        // DelayMillis = 10
        std::make_tuple(10, 0),                  // Start with zero
        std::make_tuple(10, ULONG_MAX),          // Start with max value
        std::make_tuple(10, ULONG_MAX - 1),      // Start with max value minus one
        std::make_tuple(10, ULONG_MAX - 10),     // Start with max value minus delay
        std::make_tuple(10, ULONG_MAX - 10 + 1), // Start with max value minus delay plus one
        // DelayMillis = 0
        std::make_tuple(0, 0),                // Start with zero
        std::make_tuple(0, ULONG_MAX),        // Start with max value
        std::make_tuple(0, ULONG_MAX - 1),    // Start with max value minus one
        std::make_tuple(0, ULONG_MAX - 0),    // Start with max value minus delay
        std::make_tuple(0, ULONG_MAX - 0 + 1) // Start with max value minus delay plus one
        ));

INSTANTIATE_TEST_CASE_P(
    SpinTimer,
    SpinTimerRecurring,
    ::testing::Values(
        // DelayMillis | StartMillis | NumberOfLoops
        SpinTimerRecurringTestParam(10, 0, 1),
        SpinTimerRecurringTestParam(10, 0, 10),
        SpinTimerRecurringTestParam(10, 0, 100),
        SpinTimerRecurringTestParam(10, ULONG_MAX, 500),
        SpinTimerRecurringTestParam(10, ULONG_MAX-1, 500)
        ));