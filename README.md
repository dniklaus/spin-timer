spin-timer
===================

Universal Timer with 1 millisecond resolution, supporting OOP principles.



# Features

* configurable to be either recurring (timer automatically restarts after the interval) or non-recurring (timer stops after timeout period is over)
* timer interval/timeout time configurable
* attaches automatically in the background to a Timer Context which periodically updates all registered timers' states and performs the timer expire evaluation for each registered timer
* based on system uptime (number of milliseconds since the system began running the current program, 
  i.e. Arduino: millis() function or STM32: HAL_GetTick() function);
  the source of the uptime info [ms] can be overridden by injecting a specific implementation when working with other frameworks than with Arduino
* handles system time overflows correctly (unsigned long int type, occurring around every 50 hours)


# Integration

Here the integration of a Timer is shown with a simple Arduino Sketch toggling the Arduino board's built-in LED (blink):

* include the library

  ```C++
  #include <Arduino.h>
  #include <SpinTimer.h>
  ```

* timer interval constant definition

  ```C++
  const unsigned int BLINK_TIME_MILLIS = 200;
  ```

* specific `SpinTimerAdapter` implementation, periodically toggling the Arduino built-in LED

  ```C++
  class BlinkTimerAdapter : public SpinTimerAdapter
  {
  public:
    void timeExpired()
    {
      digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    }
  };
  ```

* setup: set LED pin to output; create recurring Timer, inject specific TimerAdapter

  ```c++
  //The setup function is called once at startup of the sketch
  void setup()
  {
    pinMode(LED_BUILTIN, OUTPUT);
    new SpinTimer(new BlinkTimerAdapter(), Timer::IS_RECURRING, BLINK_TIME_MILLIS);
  }
  ```

* loop: call `scheduleTimers()` function

  ```C++
  // The loop function is called in an endless loop
  void loop()
  {
    scheduleTimers();
  }
  ```


## Platform specific Uptime Info Adapter injection

When using the Timer library with an Arduino Framework environment the uptime milliseconds counter info default implementation (`millis()` function) is automatically engaged and nothing has to be done.

If you are using other environments (i.e. when running in an STM32Cube system using STM32 HAL), a specific `UptimeInfoAdapter` implementation has to be used and injected. The following example shows the specific `STM32UptimeInfoAdapter`implementation of `UptimeInfoAdapter`as to be found in the Examples folder:

```C++
/*
 * STM32UptimeInfoAdapter.h
 *
 *  Created on: 30.07.2019
 *      Author: dniklaus
 */

#ifndef STM32UPTIMEINFOADAPTER_H_
#define STM32UPTIMEINFOADAPTER_H_

#include "stm32l4xx_hal.h"   // maybe shall be changed according to your specific STM32 MCU chip in use
#include "UptimeInfo.h"

class STM32UptimeInfoAdapter: public UptimeInfoAdapter
{
public:
  inline unsigned long tMillis()
  {
    unsigned long ms = HAL_GetTick();
    return ms;
  }
};

#endif /* STM32UPTIMEINFOADAPTER_H_ */

```



Here is how the `STM32UptimeInfoAdapter` implementation is injected into the `UptimeInfo` instance:

```C++
// ..
#include "STM32UptimeInfoAdapter.h"

int main(void)
{
  HAL_Init();
  SystemClock_Config();
  // ..
  UptimeInfo::Instance()->setAdapter(new STM32UptimeInfoAdapter());  

  // ..
    
  while (1)
  {
    scheduleTimers();
  }    
}
```



# API

This section describes the Timer library Application Programming Interface.
## SpinTimer
* *Constructor*: `SpinTimer(SpinTimerAdapter* adapter = 0, bool isRecurring = false, unsigned long timeMillis = 0)`
  Will attach itself to the `SpinTimerContext` (which normally keeps being hidden to the application).
  * Parameter `adapter`: `SpinTimerAdapter` to be injected, is able to emit a timer expired event to any specific listener, default: 0 (no event will be sent)
  * Parameter `isRecurring`: Operation mode, true: recurring, false: non-recurring, default: false
  * Parameter `timeMillis`: Timer interval/timeout time [ms], >0: timer starts automatically after creation, 0: timer remains stopped after creation (timer will expire as soon as possible when started with startTimer()), default: 0
* *Attach specific SpinTimerAdapter*, acts as dependency injection. `void attachAdapter(SpinTimerAdapter* adapter)`
  * Parameter `adapter`: Specific `SpinTimerAdapter` implementation
* *Timer Adapter get accessor* method. `SpinTimerAdapter* adapter()`
   * Returns `SpinTimerAdapter`: Object pointer or 0 if no adapter is attached.
* *Start or restart the timer* with a specific time out or interval time. `void start(unsigned long timeMillis)`
   * Parameter `timeMillis`: Time out or interval time to be set for the timer [ms]; 0 will make the timer expire as soon as possible.
* *Start or restart the timer*. `void start()`
   * The timer will expire after the specified time set with the constructor or `start(timeMillis)` before.
* *Cancel the timer and stop*. `void cancel()`
  * No time expired event will be sent out after the specified time would have been elapsed.
  * Subsequent `isExpired()` queries will return false.

* Poll method to *get the timer expire status*, recalculates whether the timer has expired before. `bool isExpired()`
  * This method could be used in a pure polling mode, where `tick()` has not to get called (by the `SpinTimerContext::handleTick()` method), but also a mixed operation in combination with calling `tick()` periodically is possible.
  * Subsequent `isExpired()` queries will return false after the first one returned true, as long as the time did not expire again in case of a recurring timer.
  * Returns `true` if the timer has expired.
  
* Indicates whether the timer is currently *running*. `bool isRunning()`
  * Returns `true` if timer is running.

* Kick the Timer. `void tick()`
   * Recalculates whether the timer has expired.

* Constant for `isRecurring` parameter of the constructor to create a one shot timer.
  `static const bool IS_NON_RECURRING = false`

* Constant for `isRecurring` parameter of the constructor to create a recurring timer.
  `static const bool IS_RECURRING = true`


## SpinTimerAdapter
* Adapter Interface, will notify `timeExpired()` event.
* Implementations derived from this interface can be injected into a SpinTimer object.
  * the SpinTimer then will call out the specific adapter's `timeExpired()` method.
  Interface sending out a `timerExpired()` event.
* *Time expired event*. To be implemented by specific `SpinTimerAdapter` class. `virtual void timeExpired() = 0`

## UptimeInfoAdapter

* Uptime Info Adapter Interface, will call out to `tMillis()` method to get current milliseconds counter value.
* Implementations derived from this interface can be injected into the `UptimeInfo` singleton object.
* Default implementation `DefaultUptimeInfoAdapter` for Arduino Framework environments is engaged automatically
* Call out to get current milliseconds. To be implemented by specific `UptimeInfoAdapter` class. `virtual unsigned long tMillis() = 0`



# Notes
This repository has been forked from  https://github.com/dniklaus/wiring-timer (Release 2.9.0) and with renamed Classes:
* Timer -> SpinTimer
* TimerAdapter -> SpinTimerAdapter
* TimerContext -> SpinTimerContext

The following timer methods have also been renamed:
* isTimerExpired() -> isExpired()
* cancelTimer() -> cancel()
* startTimer(unsigned long timeMillis) -> start(unsigned long timeMillis)
* startTimer() -> start()

