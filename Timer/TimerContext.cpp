/*
  Copyright (c) 2014 D. Niklaus.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

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

