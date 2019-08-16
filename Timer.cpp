/*
 * Timer.cpp
 *
 *  Created on: 13.08.2013
 *      Author: niklausd
 */

#include <limits.h>
#include "UptimeInfo.h"
#include "TimerContext.h"
#include "Timer.h"

void scheduleTimers()
{
  TimerContext::instance()->handleTick();
}

void delayAndSchedule(unsigned long delayMillis)
{
  // create a one-shot timer on the fly
  Timer delayTimer(0, Timer::IS_NON_RECURRING, (delayMillis));

  // wait until the timer expires
  while (!delayTimer.isTimerExpired())
  {
    // schedule the timer above and all the other timers, so they will still run in 'parallel'
    scheduleTimers();
  }
}

const bool Timer::IS_NON_RECURRING = false;
const bool Timer::IS_RECURRING     = true;

Timer::Timer(TimerAdapter* adapter, bool isRecurring, unsigned long timeMillis)
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
  TimerContext::instance()->attach(this);

  if (0 < m_delayMillis)
  {
    startTimer();
  }
}

Timer::~Timer()
{
  TimerContext::instance()->detach(this);
}

void Timer::attachAdapter(TimerAdapter* adapter)
{
  m_adapter = adapter;
}

TimerAdapter* Timer::adapter()
{
  return m_adapter;
}

Timer* Timer::next()
{
  return m_next;
}

void Timer::setNext(Timer* timer)
{
  m_next = timer;
}


bool Timer::isTimerExpired()
{
  internalTick();
  bool isExpired = m_isExpiredFlag;
  m_isExpiredFlag = false;
  return isExpired;
}

bool Timer::isRunning()
{
  return m_isRunning;
}

void Timer::tick()
{
  internalTick();
}

void Timer::cancelTimer()
{
  m_isRunning = false;
  m_isExpiredFlag = false;
}

void Timer::startTimer(unsigned long timeMillis)
{
  m_isRunning = true;
  m_delayMillis = timeMillis;
  m_currentTimeMillis = UptimeInfo::Instance()->tMillis();
  startInterval();
}

void Timer::startTimer()
{
  m_isRunning = true;
  m_currentTimeMillis = UptimeInfo::Instance()->tMillis();
  startInterval();
}

void Timer::startInterval()
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

void Timer::internalTick()
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
