/*
 * UptimeInfo.cpp
 *
 *  Created on: 01.10.2013
 *      Author: niklausd
 */
#include "UptimeInfo.h"

#ifdef ARDUINO
#include "Arduino.h"
#else
#include <sys/time.h>
#endif

class DefaultUptimeInfoAdapter : public UptimeInfoAdapter
{
public:
  inline unsigned long tMillis()
  {
#ifdef ARDUINO
    return millis();
#else
/**
 * @see http://stackoverflow.com/a/1952423
 */
    struct timeval tp;
    gettimeofday(&tp, 0);
    unsigned long ms = tp.tv_sec * 1000 + tp.tv_usec / 1000;
    return ms;
#endif
  }
};

UptimeInfo* UptimeInfo::s_instance = 0;
UptimeInfoAdapter* UptimeInfo::s_adapter = 0;

UptimeInfo::UptimeInfo()
{
  s_adapter = new DefaultUptimeInfoAdapter();
}

UptimeInfo::~UptimeInfo()
{
  delete s_adapter;
  s_adapter = 0;
}

void UptimeInfo::setAdapter(UptimeInfoAdapter* adapter)
{
  s_adapter = adapter;
}
