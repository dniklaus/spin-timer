wiring-timer
===================

Universal Timer based on Arduino millis() function, supporting OOP principles and interoperating with Arduino yield() and delay() functions.

# Features

* configurable to be either recurring (timer automatically restarts after the interval) or non-recurring (timer stops after timeout period is over)
* timer interval/timeout time configurable
* attaches automatically to Timer Context which periodically updates all registered timers' states and performs the timer expire evaluation for each registered timer
* based on Arduino millis() function (number of milliseconds since the Arduino board began running the current program), handles unsigned long int overflows correctly
* implements Arduino yield() function in order to keep the timers' scheduling ongoing even while applications and drivers use the Arduino delay() function 

# Integration
Here the integration of a Timer is hown with a simple Arduino Sketch toggling the Arduino board's built-in LED (blink):

* Include the library

      #include <Timer.h>

* Timer interval constant definition

      const unsigned int BLINK_TIME_MILLIS = 200;

* specific `TimerAdapter` implementation, periodically toggling the Arduino built-in LED

      class BlinkTimerAdapter : public TimerAdapter
      {
      public:
        void timeExpired()
        {
          digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
        }
      };

* Setup: set LED pin to output; create recurring Timer, inject specific TimerAdapter

      //The setup function is called once at startup of the sketch
      void setup()
      {
        pinMode(LED_BUILTIN, OUTPUT);
        new Timer(new BlinkTimerAdapter(), Timer::IS_RECURRING, BLINK_TIME_MILLIS);
      }

* Loop: call `yield()`, the Arduino scheduler function

      // The loop function is called in an endless loop
      void loop()
      {
        yield();
      }

* Loop: or alternatively call Arduino `delay()` function

      // The loop function is called in an endless loop
      void loop()
      {
        delay(10);
      }


# API

This section describes the Timer library Application Programming Interface.
## Timer
* *Constructor*: `Timer(TimerAdapter* adapter = 0, bool isRecurring = false, unsigned long timeMillis = 0)`
  Will attach itself to the `TimerContext` (which normally keeps being hidden to the application).
  * Parameter `adapter`: `TimerAdapter` to be injected, is able to emit a timer expired event to any specific listener, default: 0 (no event will be sent)
  * Parameter `isRecurring`: Operation mode, true: recurring, false: non-recurring, default: false
  * Parameter `timeMillis`: Timer interval/timeout time [ms], >0: timer starts automatically after creation, others: timer remains stopped after creation, default: 0
* *Attach specific TimerAdapter*, acts as dependency injection. `void attachAdapter(TimerAdapter* adapter)`
  * Parameter `adapter`: Specific `TimerAdapter` implementation
* *Timer Adapter get accessor* method. `TimerAdapter* adapter()`
   * Returns `TimerAdapter`: Object pointer or 0 if no adapter is attached.
* *Start or restart the timer* with a specific time out or interval time. `void startTimer(unsigned long timeMillis)`
   * Parameter `timeMillis`: Time out or interval time to be set for the timer [ms]; 0 will cancel the timer.
* *Start or restart the timer*. `void startTimer()`
   * If the timer has been canceled before, this will have no effect - in order to start the timer again, the `startTimer(timeMillis)` method with specific time value parameter has to be used instead.
* *Cancel the timer and stop*. `void cancelTimer()`
  * No time expired event will be sent out after the specified time would have been elapsed.
  * Subsequent `isTimerExpired()` queries will return false.

* Poll method to *get the timer expire status*, recalculates whether the timer has expired before. `bool isTimerExpired()`
  * This method could be used in a pure polling mode, where `tick()` has not to get called (by the `TimerContext::handleTick()` method), but also a mixed operation in combination with calling `tick()` periodically is possible.
  * Subsequent `isTimerExpired()` queries will return false after the first one returned true, as long as the time did not expire again in case of a recurring timer.
  * Returns `true` if the timer has expired.
  
* Indicates whether the timer is currently *running*. `bool isRunning()`
  * Returns `true` if timer is running.

* Kick the Timer. `void tick()`
   * Recalculates whether the timer has expired.

* Constant for `isRecurring` parameter of the constructor to create a one shot timer.
  `static const bool IS_NON_RECURRING = false`

* Constant for `isRecurring` parameter of the constructor to create a recurring timer.
  `static const bool IS_RECURRING = true`


## TimerAdapter
* Adapter Interface, will notify `timeExpired()` event.
* Implementations derived from this interface can be injected into a Timer object.
* The Timer then will call out the specific adapter's timeExpired() method.
Interface sending out a `timerExpired()` event.
* *Time expired event*. To be implemented by specific Timer Adapter class. `virtual void timeExpired() = 0`


## Notes
This repository is a renamed clone of https://github.com/dniklaus/arduino-utils-timer (Release 2.3.0).

For more details, please refer to the wiki: https://github.com/dniklaus/arduino-utils-timer/wiki/Timer
