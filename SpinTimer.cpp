/*
 * Timer.cpp
 *
 *  Created on: 13.08.2013
 *      Author: niklausd
 */

#include "SpinTimer.h"

#include <limits.h>
#include "UptimeInfo.h"
#include "SpinTimerContext.h"

void scheduleTimers()
{
  SpinTimerContext::instance()->handleTick();
}

void delayAndSchedule(unsigned long delayMillis)
{
  // create a one-shot timer on the fly
  SpinTimer delayTimer(0, SpinTimer::IS_NON_RECURRING, (delayMillis));

  // wait until the timer expires
  while (!delayTimer.isTimerExpired())
  {
    // schedule the timer above and all the other timers, so they will still run in 'parallel'
    scheduleTimers();
  }
}

const bool SpinTimer::IS_NON_RECURRING = false;
const bool SpinTimer::IS_RECURRING     = true;

SpinTimer::SpinTimer(SpinTimerAdapter* adapter, bool isRecurring, unsigned long timeMillis)
: m_isRunning(false)
, m_isRecurring(isRecurring)
, m_isExpiredFlag(false)
, m_currentTimeMillis(0)
, m_triggerTimeMillis(0)
, m_triggerTimeMillisUpperLimit(ULONG_MAX)
, m_delayMillis(timeMillis)
, m_adapter(adapter)
, m_next(0)
{
  SpinTimerContext::instance()->attach(this);

  if (0 < m_delayMillis)
  {
    startTimer();
  }
}

SpinTimer::~SpinTimer()
{
  SpinTimerContext::instance()->detach(this);
}

void SpinTimer::attachAdapter(SpinTimerAdapter* adapter)
{
  m_adapter = adapter;
}

SpinTimerAdapter* SpinTimer::adapter()
{
  return m_adapter;
}

SpinTimer* SpinTimer::next()
{
  return m_next;
}

void SpinTimer::setNext(SpinTimer* timer)
{
  m_next = timer;
}


bool SpinTimer::isTimerExpired()
{
  internalTick();
  bool isExpired = m_isExpiredFlag;
  m_isExpiredFlag = false;
  return isExpired;
}

bool SpinTimer::isRunning()
{
  return m_isRunning;
}

void SpinTimer::tick()
{
  internalTick();
}

void SpinTimer::cancelTimer()
{
  m_isRunning = false;
  m_isExpiredFlag = false;
}

void SpinTimer::startTimer(unsigned long timeMillis)
{
  m_isRunning = true;
  m_delayMillis = timeMillis;
  m_currentTimeMillis = UptimeInfo::Instance()->tMillis();
  startInterval();
}

void SpinTimer::startTimer()
{
  m_isRunning = true;
  m_currentTimeMillis = UptimeInfo::Instance()->tMillis();
  startInterval();
}

void SpinTimer::startInterval()
{
  unsigned long deltaTime = ULONG_MAX - m_currentTimeMillis;
  if (deltaTime < m_delayMillis)
  {
    // overflow will occur
    m_triggerTimeMillis = m_delayMillis - deltaTime - 1;
    m_triggerTimeMillisUpperLimit = m_currentTimeMillis;
  }
  else
  {
    m_triggerTimeMillis = m_currentTimeMillis + m_delayMillis - 1;
    m_triggerTimeMillisUpperLimit = ULONG_MAX;
  }
}

void SpinTimer::internalTick()
{
  m_currentTimeMillis = UptimeInfo::Instance()->tMillis();

  // check if interval is over as long as the timer shall be running
  if (m_isRunning && (m_triggerTimeMillis < m_currentTimeMillis) && (m_currentTimeMillis < m_triggerTimeMillisUpperLimit))
  {
    // interval is over
    if (m_isRecurring)
    {
      // start next interval
      startInterval();
    }
    else
    {
      m_isRunning = false;
    }

    m_isExpiredFlag = true;
    if (0 != m_adapter)
    {
      m_adapter->timeExpired();
    }
  }
}
