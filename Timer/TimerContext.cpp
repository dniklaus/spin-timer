/*
 * TimerContext.cpp
 *
 *  Created on: 25.09.2013
 *      Author: niklausd
 */

#include "TimerContext.h"
#include "Timer.h"

TimerContext* TimerContext::s_instance = (TimerContext*)0;

TimerContext* TimerContext::instance()
{
  if (0 == s_instance)
  {
    s_instance = new TimerContext();
  }
  return s_instance;
}

void TimerContext::attach(Timer* timer)
{
  if (0 == m_timer)
  {
    m_timer = timer;
  }
  else
  {
    Timer* next = m_timer;
    while (next->next() != 0)
    {
      next = next->next();
    }
    next->setNext(timer);
  }
}

void TimerContext::detach(Timer* timer)
{
  if (m_timer == timer)
  {
    m_timer = timer->next();
  }
  else
  {
    Timer* next = m_timer;
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

void TimerContext::handleTick()
{
  Timer* timer = m_timer;
  while (timer != 0)
  {
    timer->tick();
    timer = timer->next();
  }
}

TimerContext::TimerContext()
: m_timer(0)
{ }

TimerContext::~TimerContext()
{ }

