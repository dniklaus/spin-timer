#pragma once

#include "SpinTimer.h"
#include <functional>

class MySpinTimerAction : public SpinTimerAction
{
private:
    std::function<void(void)> m_Callback;

public:
    MySpinTimerAction(std::function<void(void)> callback) : m_Callback(callback){};
    
    void timeExpired()
    {
        m_Callback();
    }
};