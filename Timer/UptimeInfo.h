/*
 * UptimeInfo.h
 *
 *  Created on: 01.10.2013
 *      Author: niklausd
 */

#ifndef UPTIMEINFO_H_
#define UPTIMEINFO_H_

/**
 * Helper class to use the appropriate time base depending on platfrom.
 * Supported platforms:
 * - Arduino (AVR and SAM)
 * - POSIX
 */
class UptimeInfo
{
public:
  UptimeInfo() { }
  virtual ~UptimeInfo() { }

  /**
   * Returns the number of milliseconds since the program started.
   * @return Number of milliseconds since the program started.
   */
  static unsigned long tMillis();
};

#endif /* UPTIMEINFO_H_ */
