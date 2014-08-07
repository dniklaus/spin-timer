arduino-utils-timer
===================

Universal Timer.

Features:

* configurable to be either recurring (timer automatically restarts after the interval) or non-recurring (timer stops after timeout period is over)
* timer interval/timeout time configurable
* attaches automatically to Timer Context which periodically updates all registered timers' states and performs the timer expire evaluation for each registered timer
* based on Arduino millis() function (number of milliseconds since the Arduino board began running the current program), handles unsigned long int overflows correctly
* implements Arduino yield() function in order to keep the timers' scheduling ongoing even while applications and drivers use the Arduino delay() function 

For more details, please refer to the wiki: https://github.com/dniklaus/arduino-utils-timer/wiki
