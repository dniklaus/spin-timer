#pragma once

#include "gmock/gmock.h"
#include "SpinTimer.h"

class Mock_SpinTimerAction : public SpinTimerAction
{
public:
    // SpinTimerAction Interface
    MOCK_METHOD(void, timeExpired, (), (override));
};
