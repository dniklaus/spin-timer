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
 * Timer.h
 *
 *  Created on: 13.08.2013
 *      Author: niklausd
 */

#ifndef TIMER_H_
#define TIMER_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Schedule all timers, check their expiration states.
 * @see TimerContext::handleTick()
 */
void scheduleTimers();

/**
 * Schedule all timers, check their expiration states.
 * @see scheduleTimers()
 *
 * The yield() function prototype is declared in arduino.h, and an empty weak implementation is defined in arduino/core/hooks.c.
 * The linker will override the weak by this implementation.
 *
 * Call this function from large loops in order to keep the timers keep being scheduled all the time.
 * The arduino sleep() function calls this as well.
 */
extern void yield();

#ifdef __cplusplus
} // extern "C"
#endif

/**
 * Delay the caller by the mentioned time while all timers are kept being scheduled in the meanwhile.
 * @param delayMillis Time to wait in [ms]
 *
 * This function is kept for backward compatibility, you can use the arduino delay() function instead.
 */
void delayAndSchedule(unsigned int delayMillis);

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

protected:
  TimerAdapter() { }

public:
  virtual ~TimerAdapter() { }

private:  // forbidden functions
  TimerAdapter(const TimerAdapter& src);              // copy constructor
  TimerAdapter& operator = (const TimerAdapter& src); // assignment operator
};

/**
 * Universal Timer.
 *
 * Features:
 * - intended use: encapsulate recurring and non-recurring timed actions with a non-busy wait approach for time spans in a range of 10 to thousands of milliseconds, such as:
 *   - debouncing push-button and switch signals
 *   - blink some LEDs
 *   - schedule some sequences where time accuracy is not crucial
 *   - implements Arduino yield() function in order to keep the timers scheduling ongoing even while applications and drivers use the Arduino delay() function
 * - configurable to be either
 *   - recurring (timer automatically restarts after the interval) or
 *   - non-recurring (timer stops after timeout period is over)
 * - timer interval/timeout time configurable ([ms])
 * - automatically attaches to TimerContext's linked list of Timer objects. As long as the
 *   TimerContext::handleTick() will be called (use global functions yield() or scheduleTimers() to do so),
 *   this will periodically update the timers' states and thus perform the timers' expire evaluations
 * - based on millis() function (number of milliseconds since the Arduino board began
 *   running the current program), handles unsigned long int overflows correctly
 *   (occurring around every 50 hours)
 *
 * Integration:
 *
 * (shown on the basis of a simple application toggling the Arduino board's built-in LED)
 *
 * - Include
 *
 *       #include "Timer.h"
 *
 * - Timer interval constant definition
 *
 *       const unsigned int BLINK_TIME_MILLIS = 200;
 *
 * - specific TimerAdapter implementation, periodically toggling the built-in LED
 *
 *       class BlinkTimerAdapter : public TimerAdapter
 *       {
 *       public:
 *         void timeExpired()
 *         {
 *           digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
 *         }
 *       };
 *
 * - Setup: set LED pin to output; create recurring Timer, inject specific TimerAdapter
 *
 *       //The setup function is called once at startup of the sketch
 *       void setup()
 *       {
 *         pinMode(LED_BUILTIN, OUTPUT);
 *         new Timer(new BlinkTimerAdapter(), Timer::IS_RECURRING, BLINK_TIME_MILLIS);
 *       }
 *
 * - Loop: call yield(), the Arduino scheduler function
 *
 *       // The loop function is called in an endless loop
 *       void loop()
 *       {
 *         yield();
 *       }
 *
 * .
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
  Timer(TimerAdapter* adapter = 0, bool isRecurring = false, unsigned long timeMillis = 0);

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
  void startTimer(unsigned long timeMillis);

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
