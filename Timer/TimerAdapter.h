/*
 * TimerAdapter.h
 *
 *  Created on: 13.08.2013
 *      Author: niklausd
 */

#ifndef TIMERADAPTER_H_
#define TIMERADAPTER_H_

/**
 * Adapter Interface, will notify timeExpired() event.
 * Implementations derived from this interface can be injected into a Timer object.
 * The Timer then will call out the specific adapter's timeExpired() method.
 */
class TimerAdapter
{
public:
  virtual ~TimerAdapter();

protected:
  /**
   * Constructor, protected since this is an Interface.
   */
  TimerAdapter();

public:
  /**
   * Time expired event. To be implemented by specific Timer Adapter class.
   */
  virtual void timeExpired() = 0;

private: // forbidden default functions
  TimerAdapter& operator = (const TimerAdapter& ); // assignment operator
  TimerAdapter(const TimerAdapter& src); // copy constructor
};

#endif /* TIMERADAPTER_H_ */
