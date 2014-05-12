/*
 * Timer.h
 *
 *  Created on: 13.08.2013
 *      Author: niklausd
 */

#ifndef TIMER_H_
#define TIMER_H_

/**
 * @see TimerContext::handleTick()
 */
void scheduleTimers();

/**
 * Adapter Interface, will notify timeExpired() event.
 * Implementations derived from this interface can be injected into a Timer object.
 * The Timer then will call out the specific adapter's timeExpired() method.
 */
class TimerAdapter
{
public:
  /**
   * Time expired event. To be implemented by specific Timer Adapter class.
   */
  virtual void timeExpired() = 0;
};

/**
 * Universal Timer.
 *
 * Features:
 * - intended use: non-busy wait for time spans in a range of 10 to hundreds of milliseconds, such as:
 *   - debouncing push-button and switch signals
 *   - blink some LEDs
 *   - schedule some sequences where time accuracy is not crucial
 * - configurable to be either
 *   - recurring (timer automatically restarts after the interval) or
 *   - non-recurring (timer stops after timeout period is over)
 * - timer interval/timeout time configurable ([ms])
 * - automatically attaches to TimerContext's linked list of Timer objects. As long as the
 *   TimerContext::handleTick() will be called (use global function scheduleTimers() to do so),
 *   this will periodically update the timers' state and thus perform the timers' expire evaluation
 * - based on millis() function (number of milliseconds since the Arduino board began
 *   running the current program), handles unsigned long int overflows correctly
 *   (occurring around every 50 hours)
 *
 * Integration:
 *
 *       #include "Timer.h"
 *
 *       void loop()
 *       {
 *         // Kick the timer(s)
 *         scheduleTimers();
 *
 *         // .. do something else (more useful than busy waiting)
 *       }
 *
 */
class Timer
{
  friend class TimerContext;

public:
  /**
   * Timer constructor.
   * @param adapter TimerAdapter, is able to emit a timer expired event to any specific listener, default: 0 (no event will be sent)
   * @param isRecurring Operation mode, true: recurring, false: non-recurring, default: false
   * @param timeMillis Timer interval/timeout time [ms], >0: timer starts automatically after creation, others: timer stopped after creation, default: 0
   */
  Timer(TimerAdapter* adapter = 0, bool isRecurring = false, unsigned int timeMillis = 0);

  /**
   * Timer destructor.
   * Will detach itself from TimerContext.
   */
  virtual ~Timer();

  /**
   * Attach specific TimerAdapter, acts as dependency injection. @see TimerAdapter interface.
   * @param adapter Specific TimerAdapter
   */
  void attachAdapter(TimerAdapter* adapter);

  /**
   * Timer Adapter accessor method.
   * @return TimerAdapter object pointer or 0 if no adapter is attached.
   */
  TimerAdapter* adapter();

protected:
  /**
   * Get next Timer object pointer out of the linked list containing timers.
   * @return Timer object pointer or 0 if current object is the trailing list element.
   */
  Timer* next();

  /**
   * Set next Timer object of the linked list containing timers.
   * @param timer Timer object pointer to be set as the next element of the list.
   */
  void setNext(Timer* timer);

public:
  /**
   * Start or restart the timer with a specific time out or interval time.
   * @param timeMillis Time out or interval time to be set for the timer [ms]; 0 will cancel the timer, @see cancelTimer().
   */
  void startTimer(unsigned int timeMillis);

  /**
   * Start or restart the timer.
   * If the timer has been canceled before, this will have no effect -
   * in order to start the timer again, the startTimer(timeMillis) method with specific time value parameter has to be used instead.
   */
  void startTimer();

  /**
   * Cancel the timer and stop. No time expired event will be sent out after the specified time would have been elapsed.
   * Subsequent isTimerExpired() queries will return false.
   */
  void cancelTimer();

  /**
   * Poll method to get the timer expire status, recalculates whether the timer has expired before.
   * This method could be used in a pure polling mode, where tick() has not to get called
   * (by the TimerContext::handleTick() method), but also a mixed operation in combination with
   * calling tick() periodically is possible.
   * Subsequent isTimerExpired() queries will return false after the first one returned true,
   * as long as the time did not expire again in case of a recurring timer.
   * @return true if the timer has expired.
   */
  bool isTimerExpired();

  /**
   * Indicates whether the timer is currently running.
   * @return true if timer is running.
   */
  bool isRunning();

  /**
   * Kick the Timer.
   * Recalculates whether the timer has expired.
   */
  void tick();

private:
  /**
   * Internal tick method, evaluates the expired state.
   */
  void internalTick();

  /**
   * Starts time interval measurement, calculates the expiration trigger time.
   * Manages to avoid Arduino millis() overflow issues occurring around every 50 hours.
   */
  void startInterval();

public:
  /**
   * Constant for isRecurring parameter of the constructor (@see Timer()), to create a one shot timer.
   */
  static const bool IS_NON_RECURRING;

  /**
   * Constant for isRecurring parameter of the constructor (@see Timer()), to create a recurring timer.
   */
  static const bool IS_RECURRING;

private:
  bool m_isRecurring; /// Timer mode flag, true: timer will automatically restart after expiration.
  bool m_isExpiredFlag; /// Timer expiration flag.
  unsigned long m_currentTimeMillis; /// interval time measurement base, updated every internalTick(), called either by tick() or by isTimerExpired()
  unsigned long m_triggerTimeMillis; ///
  unsigned long m_triggerTimeMillisUpperLimit;
  unsigned long m_delayMillis;
  TimerAdapter* m_adapter;
  Timer* m_next;

private: // forbidden default functions
  Timer& operator = (const Timer& src); // assignment operator
  Timer(const Timer& src);              // copy constructor
};

#endif /* TIMER_H_ */
