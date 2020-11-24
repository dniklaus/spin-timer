/*
 * SpinTimer.h
 *
 *  Created on: 13.08.2013
 *      Author: niklausd
 */

#ifndef SPINTIMER_H_
#define SPINTIMER_H_

/**
 * Schedule all timers, check their expiration states.
 * @see TimerContext::handleTick()
 */
void scheduleTimers();

/**
 * Delay the caller by the mentioned time while all timers are kept being scheduled in the meanwhile.
 * @param delayMillis Time to wait in [ms]
 *
 * This function is kept for backward compatibility, you can use the arduino delay() function instead.
 */
void delayAndSchedule(unsigned long delayMillis);

/**
 * Adapter Interface, will notify timeExpired() event.
 * Implementations derived from this interface can be injected into a Timer object.
 * The Timer then will call out the specific adapter's timeExpired() method.
 */
class SpinTimerAdapter
{
public:
  /**
   * Time expired event. To be implemented by specific Timer Adapter class.
   */
  virtual void timeExpired() = 0;

protected:
  SpinTimerAdapter() { }

public:
  virtual ~SpinTimerAdapter() { }

private:  // forbidden functions
  SpinTimerAdapter(const SpinTimerAdapter& src);              // copy constructor
  SpinTimerAdapter& operator = (const SpinTimerAdapter& src); // assignment operator
};

/**
 * Universal Timer.
 *
 * Features:
 * - intended use: encapsulate recurring and non-recurring timed actions with a non-busy wait approach for time spans in a range of 0 to thousands of milliseconds, such as:
 *   - debouncing push-button and switch signals (recommended interval: 50 ms)
 *   - blink some LEDs
 *   - schedule some sequences where time accuracy is not crucial
 * - configurable to be either
 *   - recurring (timer automatically restarts after the interval) or
 *   - non-recurring (timer stops after timeout period is over)
 * - timer interval/timeout time configurable ([ms])
 * - automatically attaches to SpinTimerContext's linked list of SpinTimer objects. As long as the
 *   SpinTimerContext::handleTick() will be called (use global function scheduleTimers() to do so),
 *   this will periodically update the timers' states and thus perform the timers' expire evaluations
 * - based on system uptime (number of milliseconds since the system began running the current program,
 *   i.e. Arduino: millis() function or STM32: HAL_GetTick() function),
 * - handles system time overflows correctly (unsigned long int type, occurring around every 50 hours)
 *
 * Integration:
 *
 * (shown on the basis of a simple Arduino Framework application toggling the board's built-in LED)
 *
 * - Include
 *
 *       #include "SpinTimer.h"
 *       #include "Arduino.h"
 *
 * - Timer interval constant definition
 *
 *       const unsigned int BLINK_TIME_MILLIS = 200;
 *
 * - specific SpinTimerAdapter implementation, periodically toggling the built-in LED
 *
 *       class BlinkTimerAdapter : public SpinTimerAdapter
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
 *       // The setup function is called once at startup of the sketch
 *       void setup()
 *       {
 *         pinMode(LED_BUILTIN, OUTPUT);
 *         new SpinTimer(new BlinkTimerAdapter(), SpinTimer::IS_RECURRING, BLINK_TIME_MILLIS);
 *       }
 *
 * - Loop: call scheduleTimers()
 *
 *       // The loop function is called in an endless loop
 *       void loop()
 *       {
 *         scheduleTimers();
 *       }
 *
 * .
 */
class SpinTimer
{
  friend class SpinTimerContext;

public:
  /**
   * Timer constructor.
   * @param adapter SpinTimerAdapter, is able to emit a timer expired event to any specific listener, default: 0 (no event will be sent)
   * @param isRecurring Operation mode, true: recurring, false: non-recurring, default: false
   * @param timeMillis Timer interval/timeout time [ms], >0: timer starts automatically after creation, 0: timer remains stopped after creation (timer will expire as soon as possible when started with startTimer()), default: 0
   */
  SpinTimer(SpinTimerAdapter* adapter = 0, bool isRecurring = false, unsigned long timeMillis = 0);

  /**
   * Timer destructor.
   * Will detach itself from TimerContext.
   */
  virtual ~SpinTimer();

  /**
   * Attach specific SpinTimerAdapter, acts as dependency injection. @see SpinTimerAdapter interface.
   * @param adapter Specific SpinTimerAdapter
   */
  void attachAdapter(SpinTimerAdapter* adapter);

  /**
   * SpinTimer Adapter accessor method.
   * @return SpinTimerAdapter object pointer or 0 if no adapter is attached.
   */
  SpinTimerAdapter* adapter();

protected:
  /**
   * Get next SpinTimer object pointer out of the linked list containing timers.
   * @return SpinTimer object pointer or 0 if current object is the trailing list element.
   */
  SpinTimer* next();

  /**
   * Set next SpinTimer object of the linked list containing timers.
   * @param timer SpinTimer object pointer to be set as the next element of the list.
   */
  void setNext(SpinTimer* timer);

public:
  /**
   * Start or restart the timer with a specific time out or interval time.
   * @param timeMillis Time out or interval time to be set for the timer [ms]; 0 will make the timer expire as soon as possible.
   */
  void start(unsigned long timeMillis);

  /**
   * Start or restart the timer.
   * The timer will expire after the specified time set with the constructor or start(timeMillis) before.
   */
  void start();

  /**
   * Cancel the timer and stop. No time expired event will be sent out after the specified time would have been elapsed.
   * Subsequent isExpired() queries will return false.
   */
  void cancel();

  /**
   * Poll method to get the timer expire status, recalculates whether the timer has expired before.
   * This method could be used in a pure polling mode, where tick() has not to get called
   * (by the TimerContext::handleTick() method), but also a mixed operation in combination with
   * calling tick() periodically is possible.
   * Subsequent isExpired() queries will return false after the first one returned true,
   * as long as the time did not expire again in case of a recurring timer.
   * @return true if the timer has expired.
   */
  bool isExpired();

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
   * Manages to avoid unsigned long int overflow issues occurring around every 50 hours.
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
  bool m_isRunning; /// Timer is running flag, true: timer is running, false: timer is stopped.
  bool m_isRecurring; /// Timer mode flag, true: timer will automatically restart after expiration.
  bool m_isExpiredFlag; /// Timer expiration flag.
  bool m_willOverflow;  /// UptimeInfo::Instance()->tMillis() will overflow during new started interval.
  unsigned long m_currentTimeMillis; /// interval time measurement base, updated every internalTick(), called either by tick() or by isExpired()
  unsigned long m_triggerTimeMillis;
  unsigned long m_triggerTimeMillisUpperLimit;
  unsigned long m_delayMillis;
  SpinTimerAdapter* m_adapter;
  SpinTimer* m_next;

private: // forbidden default functions
  SpinTimer& operator = (const SpinTimer& src); // assignment operator
  SpinTimer(const SpinTimer& src);              // copy constructor
};

#endif /* SPINTIMER_H_ */
