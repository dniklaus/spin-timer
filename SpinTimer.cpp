/*
 * SpinTimer.cpp
 *
 *  Created on: 13.08.2013
 *      Author: niklausd
 */

#include "SpinTimer.h"

#include <limits.h>
#include "UptimeInfo.h"
#include "SpinTimerContext.h"

const bool SpinTimer::IS_NON_RECURRING = false;
const bool SpinTimer::IS_RECURRING     = true;

void scheduleTimers()
{
  SpinTimerContext::instance()->handleTick();
}

void delayAndSchedule(unsigned long delayMillis)
{
  // create a one-shot timer on the fly
  SpinTimer delayTimer(0, SpinTimer::IS_NON_RECURRING, (delayMillis));

  // wait until the timer expires
  while (!delayTimer.isExpired())
  {
    // schedule the timer above and all the other timers, so they will still run in 'parallel'
    scheduleTimers();
  }
}

SpinTimer::SpinTimer(SpinTimerAction* action, bool isRecurring, unsigned long timeMillis)
: m_isRunning(false)
, m_isRecurring(isRecurring)
, m_isExpiredFlag(false)
, m_willOverflow(false)
, m_currentTimeMillis(0)
, m_triggerTimeMillis(0)
, m_triggerTimeMillisUpperLimit(ULONG_MAX)
, m_delayMillis(timeMillis)
, m_action(action)
, m_next(0)
{
  SpinTimerContext::instance()->attach(this);

  if (0 < m_delayMillis)
  {
    start();
  }
}

SpinTimer::~SpinTimer()
{
  SpinTimerContext::instance()->detach(this);
}

void SpinTimer::attachAction(SpinTimerAction* action)
{
  m_action = action;
}

SpinTimerAction* SpinTimer::action()
{
  return m_action;
}

SpinTimer* SpinTimer::next()
{
  return m_next;
}

void SpinTimer::setNext(SpinTimer* timer)
{
  m_next = timer;
}


bool SpinTimer::isExpired()
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

void SpinTimer::cancel()
{
  m_isRunning = false;
  m_isExpiredFlag = false;
}

void SpinTimer::start(unsigned long timeMillis)
{
  m_isRunning = true;
  m_delayMillis = timeMillis;
  m_currentTimeMillis = UptimeInfo::Instance()->tMillis();
  startInterval();
}

void SpinTimer::start()
{
  m_isRunning = true;
  m_currentTimeMillis = UptimeInfo::Instance()->tMillis();
  startInterval();
}

void SpinTimer::startInterval()
{
  unsigned long deltaTime = ULONG_MAX - m_currentTimeMillis;
  m_willOverflow = (deltaTime < m_delayMillis);
  if (m_willOverflow)
  {
    // overflow will occur
    m_triggerTimeMillis = m_delayMillis - deltaTime - 1;
    m_triggerTimeMillisUpperLimit = m_currentTimeMillis;
  }
  else
  {
    m_triggerTimeMillis = m_currentTimeMillis + m_delayMillis;
    m_triggerTimeMillisUpperLimit = ULONG_MAX - deltaTime;
  }
}

void SpinTimer::internalTick()
{
  bool intervalIsOver = false;

  m_currentTimeMillis = UptimeInfo::Instance()->tMillis();

  // check if interval is over as long as the timer shall be running
  if (m_isRunning)
  {
    if (m_willOverflow)
    {
      intervalIsOver = ((m_triggerTimeMillis <= m_currentTimeMillis) && (m_currentTimeMillis < m_triggerTimeMillisUpperLimit));
    }
    else
    {
      intervalIsOver = ((m_triggerTimeMillis <= m_currentTimeMillis) || (m_currentTimeMillis < m_triggerTimeMillisUpperLimit));
    }
    
    if (intervalIsOver)
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
      if (0 != m_action)
      {
        m_action->timeExpired();
      }
    }
  }
}
