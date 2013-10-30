/*
 * Timer.h
 *
 *  Created on: 13.08.2013
 *      Author: niklausd
 */

#ifndef TIMER_H_
#define TIMER_H_

class TimerAdapter;

/**
 * Universal Timer.
 *
 * Features:
 * - configurable to be either recurring (timer automatically restarts after the interval) or non-recurring (timer stops after timeout period is over)
 * - timer interval/timeout time configurable
 * - attaches automatically to @see TimerContext, which periodically updates the timer state and performs the timer expire evaluation
 * - based on millis() function (number of milliseconds since the Arduino board began running the current program), handles unsigned long int overflows correctly
 */
class Timer
{
public:
  /**
   * Timer constructor.
   * @param adapter Timer Adapter which allows to emit a timer expired event to any specific listener, default: 0 (no event will be sent)
   * @param isRecurring, operation mode, true: recurring, false: non-recurring, default: false
   * @param timeMillis timer interval/timeout time [ms], >0: timer starts automatically after creation, others: timer stopped after creation, default: 0
   */
  Timer(TimerAdapter* adapter = 0, bool isRecurring = false, unsigned int timeMillis = 0);

  /**
   * Timer destructor.
   * Will detach itself from TimerContext.
   */
  virtual ~Timer();

  /**
   * Attach specific Timer Adapter, acts as dependency injector.
   * @param adapter Specific Timer Adapter
   */
  void attachAdapter(TimerAdapter* adapter);

  /**
   * Timer Adapter accessor method.
   * @return TimerAdapter objec pointer or 0 if no adapter is attached.
   */
  TimerAdapter* adapter();

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

  /**
   * Start or restart the timer with a specific time out or interval time.
   * @param Time out or interval time to be set for the timer [ms]; 0 will cancel the timer, @see cancelTimer().
   */
  void startTimer(unsigned int timeMillis);

  /**
   * Start or restart the timer.
   * If the timer has been cancelled before, this will have no effect -
   * in order to start the timer again, the startTimer() method with specific time value parameter has to be used instead.
   */
  void startTimer();

  /**
   * Cancel the timer and stop. No time expired event will be sent out after the specified time would have been elapsed.
   */
  void cancelTimer();

  bool isTimerExpired();

  void tick();

protected:
  void internalTick();
  void startInterval();

private:
  bool m_isRecurring;
  bool m_isExpiredFlag;
  unsigned long m_currentTimeMillis;
  unsigned long m_triggerTimeMillis;
  unsigned long m_triggerTimeMillisUpperLimit;
  unsigned long m_delayMillis;
  TimerAdapter* m_adapter;
  Timer* m_next;

private: // forbidden default functions
  Timer& operator = (const Timer& );  // assignment operator
  Timer(const Timer& src);            // copy constructor
};

#endif /* TIMER_H_ */
