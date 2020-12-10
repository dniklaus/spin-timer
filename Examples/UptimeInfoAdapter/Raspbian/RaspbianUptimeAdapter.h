/*
 * RaspbianUptimeAdapter.h
 *
 *  Created on: 30.07.2019
 *      Author: dniklaus
 */

#ifndef SRC_TWR_RASPBIANUPTIMEADAPTER_H_
#define SRC_TWR_RASPBIANUPTIMEADAPTER_H_

#include <chrono>
#include "UptimeInfo.h"

class RaspbianUptimeAdapter: public UptimeInfoAdapter
{
private:
  std::chrono::time_point<std::chrono::steady_clock> m_start;

public:
  RaspbianUptimeAdapter()
  : m_start(std::chrono::steady_clock::now())
  { }

  virtual ~RaspbianUptimeAdapter() { }

  inline unsigned long tMillis()
  {
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_start);
    unsigned long timeMillis = elapsed.count();
    return timeMillis;
  }

private: // forbidden default functions
  RaspbianUptimeAdapter(const RaspbianUptimeAdapter &other);
  RaspbianUptimeAdapter& operator=(const RaspbianUptimeAdapter &other);
};

#endif /* SRC_TWR_RASPBIANUPTIMEADAPTER_H_ */
