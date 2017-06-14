/*
 * UptimeInfo.cpp
 *
 *  Created on: 01.10.2013
 *      Author: niklausd
 */

#include "UptimeInfo.h"

#ifdef ARDUINO
#include "Arduino.h"

unsigned long UptimeInfo::tMillis()
{
  return millis();
}

#else
#include <sys/time.h>

/**
 * @see http://stackoverflow.com/a/1952423
 */
unsigned long UptimeInfo::tMillis()
{
  struct timeval tp;
  gettimeofday(&tp, 0);
  unsigned long ms = tp.tv_sec * 1000 + tp.tv_usec / 1000;
  return ms;
}

#endif
