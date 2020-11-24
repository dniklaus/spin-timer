/*
 * SpinTimerContext.h
 *
 *  Created on: 25.09.2013
 *      Author: niklausd
 */

#ifndef SPINTIMERCONTEX_H_
#define SPINTIMERCONTEX_H_

class SpinTimer;

/**
 * Spin Timer Context.
 *
 * Features:
 * - is like a very simple scheduler.
 * - has to be kicked (by calling scheduleTimers() or SpinTimerContext::handleTick()) as often as possible and/or on regular intervals,
 *   i.e. in the Arduino main loop() function:
 *
 *       #include "SpinTimer.h"
 *
 *       void loop()
 *       {
 *         // Kick the timer(s)
 *         scheduleTimers();
 *
 *         // .. do something
 *       }
 *
 * - holds a single linked list of registered SpinTimer objects,
 *   the SpinTimers automatically attach themselves to this on their creation
 *   and automatically detach themselves on their destruction.
 * - is a Singleton
 */
class SpinTimerContext
{
  friend class SpinTimer;

public:
  /**
   * Create and/or return singleton instance of SpinTimerContext.
   * @return Pointer to singleton SpinTimerContext object pointer.
   */
  static SpinTimerContext* instance();

  /**
   * Destructor.
   */
  virtual ~SpinTimerContext();

protected:
  /**
   * Add a SpinTimer object to the single linked list.
   * @param timer SpinTimer object pointer.
   */
  void attach(SpinTimer* timer);

  /**
   * Remove specified SpinTimer object from the single linked list.
   * @param timer SpinTimer object pointer.
   */
  void detach(SpinTimer* timer);

public:
  /**
   * Kick all attached SpinTimer objects (calls the SpinTimer::tick() method).
   */
  void handleTick();

private:
  /**
   * Constructor.
   */
  SpinTimerContext();

private:
  static SpinTimerContext* s_instance; /// SpinTimerContext singleton instance variable.
  SpinTimer* m_timer; /// Root node of single linked list containing the timers to be kicked.

private: // forbidden default functions
  SpinTimerContext& operator = (const SpinTimerContext& src); // assignment operator
  SpinTimerContext(const SpinTimerContext& src);              // copy constructor
};

#endif /* SPINTIMERCONTEX_H_ */
