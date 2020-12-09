/*
 * Blink with SpinTimer, special blink pattern: 2 short pulses, long pause, repeat.
 *
 * (C) 2020 Selim Niklaus
 *                   _____       __̣̣___                      _____       __̣̣___ 
 * Blink pattern: __|     |_____|     |____________________|     |_____|     |___
 *                  |200ms|200ms|200ms|       1000ms       |
 *                  |<--->|<--->|<--->|<------------------>|
 */

#define WIRINGTIMER_SUPPRESS_WARNINGS 1
#include <SpinTimer.h>

const unsigned int  BLINK_TIME_MILLIS = 200;
const unsigned int  OFF_TIME_MILLIS = 1000;

Timer blinkTimer(0, Timer::IS_RECURRING, BLINK_TIME_MILLIS);
Timer offTimer(0, Timer::IS_NON_RECURRING);

void toggleLed(int ledPin)
{
  bool isLedOn = digitalRead(ledPin);
  digitalWrite(ledPin, !isLedOn);
}

class BlinkTimerAction : public TimerAction
{
private:
  unsigned int m_count;
  Timer& m_blinkTimer;
  Timer& m_offTimer;

public:
  BlinkTimerAction(Timer& blinkTimer, Timer& offTimer)
  : m_count(0)
  , m_blinkTimer(blinkTimer)
  , m_offTimer(offTimer)
  { }
 
  void timeExpired()
  {
    if (m_count < 3)
    {
      toggleLed(LED_BUILTIN);
      ++m_count;
    }

    if (m_count >= 3)
    {
      m_count = 0;
      m_blinkTimer.cancelTimer();
      m_offTimer.startTimer(OFF_TIME_MILLIS);
    }
  }
};

class OffTimerAction : public TimerAction
{
private:  
  Timer& m_blinkTimer;

public:
  OffTimerAction(Timer& blinkTimer)
  : m_blinkTimer(blinkTimer)
  { }
 
  void timeExpired()
  {
    m_blinkTimer.startTimer();
    toggleLed(LED_BUILTIN);
  }
};

// The setup function is called once at startup of the sketch
void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  blinkTimer.attachAction(new BlinkTimerAction(blinkTimer, offTimer));
  offTimer.attachAction(new OffTimerAction(blinkTimer));
  toggleLed(LED_BUILTIN);
}

// The loop function is called in an endless loop
void loop()
{
  scheduleTimers();
}
