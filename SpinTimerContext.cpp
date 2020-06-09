/*
 * TimerContext.cpp
 *
 *  Created on: 25.09.2013
 *      Author: niklausd
 */

#include "SpinTimerContext.h"

#include "SpinTimer.h"

SpinTimerContext* SpinTimerContext::s_instance = (SpinTimerContext*)0;

SpinTimerContext* SpinTimerContext::instance()
{
  if (0 == s_instance)
  {
    s_instance = new SpinTimerContext();
  }
  return s_instance;
}

void SpinTimerContext::attach(SpinTimer* timer)
{
  if (0 == m_timer)
  {
    m_timer = timer;
  }
  else
  {
    SpinTimer* next = m_timer;
    while (next->next() != 0)
    {
      next = next->next();
    }
    next->setNext(timer);
  }
}

void SpinTimerContext::detach(SpinTimer* timer)
{
  if (m_timer == timer)
  {
    m_timer = timer->next();
  }
  else
  {
    SpinTimer* next = m_timer;
    while ((next != 0) && (next->next() != timer))
    {
      next = next->next();
    }
    if (next != 0)
    {
      next->setNext(timer->next());
    }
  }
}

void SpinTimerContext::handleTick()
{
  SpinTimer* timer = m_timer;
  while (timer != 0)
  {
    timer->tick();
    timer = timer->next();
  }
}

SpinTimerContext::SpinTimerContext()
: m_timer(0)
{ }

SpinTimerContext::~SpinTimerContext()
{ }

