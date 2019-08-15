/*
 * STM32UptimeInfoAdapter.h
 *
 *  Created on: 30.07.2019
 *      Author: dniklaus
 */

#ifndef STM32UPTIMEINFOADAPTER_H_
#define STM32UPTIMEINFOADAPTER_H_

#include "stm32l4xx_hal.h"
#include "UptimeInfo.h"

class STM32UptimeInfoAdapter: public UptimeInfoAdapter
{
public:
  inline unsigned long tMillis()
  {
    unsigned long ms = HAL_GetTick();
    return ms;
  }
};

#endif /* STM32UPTIMEINFOADAPTER_H_ */
