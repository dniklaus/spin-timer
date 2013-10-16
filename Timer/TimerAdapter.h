/*
 * TimerAdapter.h
 *
 *  Created on: 13.08.2013
 *      Author: niklausd
 */

#ifndef TIMERADAPTER_H_
#define TIMERADAPTER_H_

/*
 *
 */
class TimerAdapter
{
public:
  TimerAdapter();
  virtual ~TimerAdapter();
  virtual void timeExpired() = 0;

private: // forbidden default functions
  TimerAdapter& operator = (const TimerAdapter& ); // assignment operator
  TimerAdapter(const TimerAdapter& src); // copy constructor
};

#endif /* TIMERADAPTER_H_ */
