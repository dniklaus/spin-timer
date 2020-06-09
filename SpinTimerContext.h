/*
 * TimerContext.h
 *
 *  Created on: 25.09.2013
 *      Author: niklausd
 */

#ifndef TIMERCONTEX_H_
#define TIMERCONTEX_H_

class SpinTimer;

/**
 * Timer Context.
 *
 * Features:
 * - is like a very simple scheduler.
 * - has to be kicked (by calling yield(), scheduleTimers() or TimerContext::handleTick()) as often as possible and/or on regular intervals,
 *   i.e. in the Arduino main loop() function:
 *
 *       #include "Timer.h"
 *
 *       void loop()
 *       {
 *         // Kick the timer(s)
 *         yield();
 *
 *         // .. do something
 *       }
 *
 * - holds a single linked list of registered Timer objects,
 *   the Timers automatically attach themselves to this on their creation
 *   and automatically detach themselves on their destruction.
 * - is a Singleton
 */
class SpinTimerContext
{
  friend class SpinTimer;

public:
  /**
   * Create and/or return singleton instance of Timer Context.
   * @return Pointer to singleton TimerContext object pointer.
   */
  static SpinTimerContext* instance();

  /**
   * Destructor.
   */
  virtual ~SpinTimerContext();

protected:
  /**
   * Add a Timer object to the single linked list.
   * @param timer Timer object pointer.
   */
  void attach(SpinTimer* timer);

  /**
   * Remove specified Timer object from the single linked list.
   * @param timer Timer object pointer.
   */
  void detach(SpinTimer* timer);

public:
  /**
   * Kick all attached Timer objects (calls the Timer::tick() method).
   */
  void handleTick();

private:
  /**
   * Constructor.
   */
  SpinTimerContext();

private:
  static SpinTimerContext* s_instance; /// Timer Context singleton instance variable.
  SpinTimer* m_timer; /// Root node of single linked list containing the timers to be kicked.

private: // forbidden default functions
  SpinTimerContext& operator = (const SpinTimerContext& src); // assignment operator
  SpinTimerContext(const SpinTimerContext& src);              // copy constructor
};

#endif /* TIMERCONTEX_H_ */
