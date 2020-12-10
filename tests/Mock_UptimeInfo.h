#pragma once

#include "UptimeInfo.h"

class Mock_UptimeInfo : public UptimeInfoAdapter
{
private:
    unsigned long m_Millis;

public:
    Mock_UptimeInfo(unsigned long millis = 0) : m_Millis(millis){};

    void setTMillis(unsigned long millis)
    {
        m_Millis = millis;
    };
    void incrementTMillis()
    {
        m_Millis++;
    };

    // UptimeInfo interface
    unsigned long tMillis()
    {
        return m_Millis;
    };
};
