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
    uint32_t count1 = 0;
    uint32_t count2 = 0;
    uint32_t count3 = 0;

    SpinTimer spinTimer1sec(1000, new MySpinTimerAction([&count1]() { count1++; }), true, true);
    SpinTimer spinTimer2sec(2000, new MySpinTimerAction([&count2]() { count2++; }), true, true);
    SpinTimer spinTimer5sec(5000, new MySpinTimerAction([&count3]() { count3++; }), true, true);

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