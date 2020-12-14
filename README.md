# SpinTimer

Universal Timer with 1 millisecond resolution, supporting OOP principles.

## Status

![build-test](https://github.com/dniklaus/spin-timer/workflows/build-test/badge.svg)
![unit-tests](https://github.com/dniklaus/spin-timer/workflows/unit-tests/badge.svg)

## Features

* configurable to be either recurring (timer automatically restarts after the interval) or non-recurring (timer stops after timeout period is over)
* timer interval/timeout time configurable
* attaches automatically in the background to a SpinTimerContext which periodically updates all registered timers' states and performs the timer expire evaluation for each registered timer
* based on system uptime (number of milliseconds since the system began running the current program, 
  i.e. Arduino: millis() function or STM32: HAL_GetTick() function);
  the source of the uptime info [ms] can be overridden by injecting a specific implementation when working with other frameworks than with Arduino
* handles system time overflows correctly (unsigned long int type, occurring around every 50 hours)


## Integration

Here the integration of a SpinTimer is shown with a simple Arduino Sketch toggling the Arduino board's built-in LED (blink):

* include the library

  ```C++
  #include <Arduino.h>
  #include <SpinTimer.h>
  ```

* timer interval constant definition

  ```C++
  const unsigned int BLINK_TIME_MILLIS = 200;
  ```

* specific `SpinTimerAction` implementation, periodically toggling the Arduino built-in LED

  ```C++
  class BlinkTimerAction : public SpinTimerAction
  {
  public:
    void timeExpired()
    {
      digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    }
  };
  ```

* setup: set LED pin to output; create recurring Timer, inject specific SpinTimerAction

  ```c++
  //The setup function is called once at startup of the sketch
  void setup()
  {
    pinMode(LED_BUILTIN, OUTPUT);
    new SpinTimer(BLINK_TIME_MILLIS, new BlinkTimerAction(), SpinTimer::IS_RECURRING, SpinTimer::IS_AUTOSTART);
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


### Platform specific Uptime Info Adapter injection

When using the SpinTimer library with an Arduino Framework environment the uptime milliseconds counter info default implementation (`millis()` function) is automatically engaged and nothing has to be done.

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



## API

This section describes the SpinTimer library Application Programming Interface.

### SpinTimer

* *Constructor*: `SpinTimer(unsigned long timeMillis, SpinTimerAction* action = 0, bool isRecurring = false, bool isAutostart = false)`
  Will attach itself to the `SpinTimerContext` (which normally keeps being hidden to the application).
  * Parameter `timeMillis`: Timer interval/timeout time [ms], >0: timer starts automatically after creation, 0: timer remains stopped after creation (timer will expire as soon as possible when started with start()), default: 0
  * Parameter `action`: `SpinTimerAction` to be injected, is able to emit a timer expired event to any specific listener, default: 0 (no event will be sent)
  * Parameter `isRecurring`: Operation mode, true: recurring, false: non-recurring, default: false
  * Parameter `isAutostart`: Autostart mode, true: autostart enabled, false: autostart disabled, default: false
* *Attach specific SpinTimerAction*, acts as dependency injection. `void attachAction(SpinTimerAction* action)`
  * Parameter `action`: Specific `SpinTimerAction` implementation
* *Timer Action get accessor* method. `SpinTimerAction* action()`
   * Returns `SpinTimerAction`: Object pointer or 0 if no action is attached.
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


### SpinTimerAction
* Action Interface, will notify `timeExpired()` event.
* Implementations derived from this interface can be injected into a SpinTimer object.
  * the SpinTimer then will call out the specific action's `timeExpired()` method.
  Interface sending out a `timerExpired()` event.
* *Time expired event*. To be implemented by specific `SpinTimerAction` class. `virtual void timeExpired() = 0`

### UptimeInfoAdapter

* Uptime Info Adapter Interface, will call out to `tMillis()` method to get current milliseconds counter value.
* Implementations derived from this interface can be injected into the `UptimeInfo` singleton object.
* Default implementation `DefaultUptimeInfoAdapter` for Arduino Framework environments is engaged automatically
* Call out to get current milliseconds. To be implemented by specific `UptimeInfoAdapter` class. `virtual unsigned long tMillis() = 0`

### Class diagram

![SpinTimer Class Diagram](docs/pic/spintimer_class-diagram.bmp)

[//]: # (\image html pic/spintimer_class-diagram.bmp)

## Example Implementation

This example shows how to use the SpinTimer library. The source code can be found in the Example folder.

![Blink Example Class Diagram](docs/pic/spintimer_blink-example_class-diagram.bmp)

[//]: # (\image html pic/spintimer_blink-example_class-diagram.bmp)

![Blink Example Object Diagram](docs/pic/spintimer_blink-example_object-diagram.bmp)

[//]: # (\image html pic/spintimer_blink-example_object-diagram.bmp)

![Blink Example Sequence Diagram](docs/pic/spintimer_blink-example_sequence-diagram.bmp)

[//]: # (\image html pic/spintimer_blink-example_sequence-diagram.bmp)

## Notes
This repository has been forked from  https://github.com/dniklaus/wiring-timer (Release 2.9.0) and with renamed Classes:
* Timer -> SpinTimer
* TimerAdapter -> SpinTimerAction
* TimerContext -> SpinTimerContext

The following timer methods have also been renamed:
* isTimerExpired() -> isExpired()
* cancelTimer() -> cancel()
* startTimer(unsigned long timeMillis) -> start(unsigned long timeMillis)
* startTimer() -> start()
