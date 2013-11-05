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
 * Timer Context.
 *
 * - is like a very simple scheduler.
 * - holds a list of registered @see Timer objects,
 *   the Timers automatically attach themselves to this on their creation
 *   and automatically detach themselves on their destruction.
 * - is a Singleton
 * - has to be kicked (@see TimerContext::handleTick()) on regular intervals,
 *   i.e. in the Arduino main loop() function:
 *
 *   void loop()
 *   {
 *     // Kick the timer(s)
 *     TimerContext::instance()->handleTick();
 *
 *     // .. do something else (more useful than busy waiting ;-))
 *   }
 */
class TimerContext
{
public:
  /**
   * Create and/or return singleton instance of Timer Context.
   * @return Pointer to singleton TimerContext object pointer.
   */
  static TimerContext* instance();

  /**
   * Destructor.
   */
  virtual ~TimerContext();

  /**
   * Add a Timer object to the single linked list.
   * @param Timer object pointer.
   */
  void attach(Timer* timer);

  /**
   * Remove specified Timer object from the single linked list.
   * @param Timer object pointer.
   */
  void detach(Timer* timer);

  /**
   * Kick all attached Timer objects (calls the @see Timer::tick() method).
   */
  void handleTick();

private:
  /**
   * Constructor.
   */
  TimerContext();

private:
  static TimerContext* s_instance; /// Timer Context singleton instance variable.
  Timer* m_timer; /// Root node of single linked list containing the timers to be kicked.

private: // forbidden default functions
  TimerContext& operator = (const TimerContext& );  // assignment operator
  TimerContext(const TimerContext& src);            // copy constructor
};

#endif /* TIMERCONTEX_H_ */
