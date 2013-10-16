/*
 * TimerContext.h
 *
 *  Created on: 25.09.2013
 *      Author: niklausd
 */

#ifndef TIMERCONTEX_H_
#define TIMERCONTEX_H_

class Timer;

/**
 *
 */
class TimerContext
{
public:
  static TimerContext* instance();
  virtual ~TimerContext();
  void attach(Timer* timer);
  void detach(Timer* timer);
  void handleTick();

private:
  TimerContext();

private:
  static TimerContext* s_instance;
  Timer* m_timer;

private: // forbidden default functions
  TimerContext& operator = (const TimerContext& );  // assignment operator
  TimerContext(const TimerContext& src);            // copy constructor
};

#endif /* TIMERCONTEX_H_ */
