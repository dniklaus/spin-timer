/*
 * TimerContext.h
 *
 *  Created on: 25.09.2013
 *      Author: niklausd
 */

#ifndef TIMERCONTEX_H_
#define TIMERCONTEX_H_

class Timer;

/**
 * Timer Context.
 *
 * Features:
 * - is like a very simple scheduler.
 * - has to be kicked (scheduleTimers() or TimerContext::handleTick()) as often as possible and/or on regular intervals,
 *   i.e. in the Arduino main loop() function:
 *
 *       #include "Timer.h"
 *
 *       void loop()
 *       {
 *         // Kick the timer(s)
 *         // TimerContext::instance()->handleTick();
 *         scheduleTimers();
 *
 *         // .. do something else (more useful than busy waiting)
 *       }
 *
 * - holds a single linked list of registered Timer objects,
 *   the Timers automatically attach themselves to this on their creation
 *   and automatically detach themselves on their destruction.
 * - is a Singleton
 */
class TimerContext
{
public:
  /**
   * Create and/or return singleton instance of Timer Context.
   * @return Pointer to singleton TimerContext object pointer.
   */
  static TimerContext* instance();

  /**
   * Destructor.
   */
  virtual ~TimerContext();

  /**
   * Add a Timer object to the single linked list.
   * @param timer Timer object pointer.
   */
  void attach(Timer* timer);

  /**
   * Remove specified Timer object from the single linked list.
   * @param timer Timer object pointer.
   */
  void detach(Timer* timer);

  /**
   * Kick all attached Timer objects (calls the Timer::tick() method).
   */
  void handleTick();

private:
  /**
   * Constructor.
   */
  TimerContext();

private:
  static TimerContext* s_instance; /// Timer Context singleton instance variable.
  Timer* m_timer; /// Root node of single linked list containing the timers to be kicked.

private: // forbidden default functions
  TimerContext& operator = (const TimerContext& src); // assignment operator
  TimerContext(const TimerContext& src);              // copy constructor
};

#endif /* TIMERCONTEX_H_ */
