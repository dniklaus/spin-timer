/*
 * Timer.cpp
 *
 *  Created on: 13.08.2013
 *      Author: niklausd
 */

#include "UptimeInfo.h"

#include "Timer.h"
#include "TimerAdapter.h"
#include "TimerContext.h"

const bool Timer::IS_NON_RECURRING = false;
const bool Timer::IS_RECURRING     = true;

Timer::Timer(TimerAdapter* adapter, bool isRecurring, unsigned int timeMillis)
: m_isRecurring(isRecurring)
, m_isExpiredFlag(false)
, m_currentTimeMillis(0)
, m_triggerTimeMillis(0)
, m_triggerTimeMillisUpperLimit(0xFFFFFFFF)
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

void Timer::startTimer(unsigned int timeMillis)
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
  unsigned long deltaTime = 0xFFFFFFFF - m_currentTimeMillis;
  if (deltaTime < m_delayMillis)
  {
    // overflow will occur
    m_triggerTimeMillis = m_delayMillis - deltaTime - 1;
    m_triggerTimeMillisUpperLimit = m_currentTimeMillis;
  }
  else
  {
    m_triggerTimeMillis = m_currentTimeMillis + m_delayMillis - 1;
    m_triggerTimeMillisUpperLimit = 0xFFFFFFFF;
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

    m_isExpiredFlag = true;
    if (0 != m_adapter)
    {
      m_adapter->timeExpired();
    }
  }
}
