/*
 * Blink with SpinTimer, special blink pattern: 2 short pulses, long pause, repeat.
 *
 * (C) 2020 Selim Niklaus
 *                   _____       __̣̣___                      _____       __̣̣___ 
 * Blink pattern: __|     |_____|     |____________________|     |_____|     |___
 *                  |200ms|200ms|200ms|       1000ms       |
 *                  |<--->|<--->|<--->|<------------------>|
 */

#include <SpinTimer.h>

const unsigned int  BLINK_TIME_MILLIS = 200;
const unsigned int  OFF_TIME_MILLIS = 1000;

SpinTimer blinkTimer(BLINK_TIME_MILLIS, 0, SpinTimer::IS_RECURRING, SpinTimer::IS_AUTOSTART);
SpinTimer offTimer(OFF_TIME_MILLIS, 0, SpinTimer::IS_NON_RECURRING, SpinTimer::IS_NON_AUTOSTART);

void toggleLed(int ledPin)
{
  bool isLedOn = digitalRead(ledPin);
  digitalWrite(ledPin, !isLedOn);
}

class BlinkTimerAction : public SpinTimerAction
{
private:
  unsigned int m_count;
  SpinTimer& m_blinkTimer;
  SpinTimer& m_offTimer;

public:
  BlinkTimerAction(SpinTimer& blinkTimer, SpinTimer& offTimer)
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
      m_blinkTimer.cancel();
      m_offTimer.start();
    }
  }
};

class OffTimerAction : public SpinTimerAction
{
private:  
  SpinTimer& m_blinkTimer;

public:
  OffTimerAction(SpinTimer& blinkTimer)
  : m_blinkTimer(blinkTimer)
  { }
 
  void timeExpired()
  {
    m_blinkTimer.start();
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
