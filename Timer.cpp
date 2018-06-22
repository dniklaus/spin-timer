/*
 * Timer.cpp
 *
 *  Created on: 13.08.2013
 *      Author: niklausd
 */

#include <limits.h>
#include "UptimeInfo.h"
#include "Timer.h"
#include "TimerContext.h"

void scheduleTimers()
{
  TimerContext::instance()->handleTick();
}

#ifndef ESP8266
void yield()
{
  TimerContext::instance()->handleTick();
}
#endif

void delayAndSchedule(unsigned int delayMillis)
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
: m_isRecurring(isRecurring)
, m_isExpiredFlag(false)
, m_currentTimeMillis(0)
, m_triggerTimeMillis(0)
, m_triggerTimeMillisUpperLimit(ULONG_MAX)
, m_delayMillis(0)
, m_adapter(adapter)
, m_next(0)
{
  TimerContext::instance()->attach(this);

  if (0 < timeMillis)
  {
    startTimer(timeMillis);
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
  return (0 != m_delayMillis);
}

void Timer::tick()
{
  internalTick();
}

void Timer::cancelTimer()
{
  m_delayMillis = 0;
  m_isExpiredFlag = false;
}

void Timer::startTimer(unsigned long timeMillis)
{
  m_delayMillis = timeMillis;
  if (m_delayMillis > 0)
  {
    m_currentTimeMillis = UptimeInfo::tMillis();
    startInterval();
  }
  else
  {
    cancelTimer();
  }
}

void Timer::startTimer()
{
  if (m_delayMillis > 0)
  {
    m_currentTimeMillis = UptimeInfo::tMillis();
    startInterval();
  }
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
  m_currentTimeMillis = UptimeInfo::tMillis();

  // check if interval is over, as long as (m_delayMillis > 0)
  if ((m_delayMillis > 0) && (m_triggerTimeMillis < m_currentTimeMillis) && (m_currentTimeMillis < m_triggerTimeMillisUpperLimit))
  {
    if (m_isRecurring)
    {
      startInterval();
    }
    else
    {
      m_delayMillis = 0;
    }

    m_isExpiredFlag = true;
    if (0 != m_adapter)
    {
      m_adapter->timeExpired();
    }
  }
}
