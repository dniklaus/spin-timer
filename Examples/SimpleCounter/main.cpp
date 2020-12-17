/**
  ******************************************************************************
  * @file           : main.cpp
  * @brief          : Simple counter example to demonstrate the use of the
  *                   SpinTimer library
  ******************************************************************************
  */

#include <iostream>
#include <thread>
#include <chrono>

#include "SpinTimer.h"
#include "MySpinTimerAction.hpp"

int main(void)
{
    const unsigned long int spinTimer_1s_millis = 1000;
    const unsigned long int spinTimer_2s_millis = 2000;
    const unsigned long int spinTimer_5s_millis = 5000;

    uint32_t count1 = 0;
    uint32_t count2 = 0;
    uint32_t count3 = 0;

    SpinTimer spinTimer1sec(spinTimer_1s_millis, new MySpinTimerAction([&count1]() { count1++; }), SpinTimer::IS_RECURRING, SpinTimer::IS_AUTOSTART);
    SpinTimer spinTimer2sec(spinTimer_2s_millis, new MySpinTimerAction([&count2]() { count2++; }), SpinTimer::IS_RECURRING, SpinTimer::IS_AUTOSTART);
    SpinTimer spinTimer5sec(spinTimer_5s_millis, new MySpinTimerAction([&count3]() { count3++; }), SpinTimer::IS_RECURRING, SpinTimer::IS_AUTOSTART);

    // Forever Loop
    for (;;)
    {
        std::cout << "SpinTimer_1s: " << count1 << "\n";
        std::cout << "SpinTimer_2s: " << count2 << "\n";
        std::cout << "SpinTimer_5s: " << count3 << "\e[A\e[A\r";

        std::cout.flush();

        scheduleTimers();

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    return 0;
}