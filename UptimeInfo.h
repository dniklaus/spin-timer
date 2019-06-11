/*
 * UptimeInfo.h
 *
 *  Created on: 01.10.2013
 *      Author: niklausd
 */

#ifndef UPTIMEINFO_H_
#define UPTIMEINFO_H_

#include "Timer.h"

/**
 * Helper class to use the appropriate time base depending on platfrom.
 * Supported platforms:
 * - Arduino (AVR and SAM)
 * - POSIX
 */
class UptimeInfo
{
public:
  static inline UptimeInfo* Instance()
  {
    if (0 == s_instance)
    {
      s_instance = new UptimeInfo();
    }
    return s_instance;
  }

protected:
  UptimeInfo();

public:
  virtual ~UptimeInfo();

  void setAdapter(UptimeInfoAdapter* adapter);

  static inline UptimeInfoAdapter* adapter()
  {
    return s_adapter;
  }

  /**
   * Returns the number of milliseconds since the program started.
   * @return Number of milliseconds since the program started.
   */
  static inline unsigned long tMillis()
  {
    unsigned long ms = 0;
    if (0 != adapter())
    {
      ms = adapter()->tMillis();
    }
    return ms;
  }


private:
  static UptimeInfo*        s_instance;
  static UptimeInfoAdapter* s_adapter;

private: // forbidden functions
  UptimeInfo& operator = (const UptimeInfo& src); // assignment operator
  UptimeInfo(const UptimeInfo& src);              // copy constructor
};

#endif /* UPTIMEINFO_H_ */
