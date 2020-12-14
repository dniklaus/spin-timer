#include <SpinTimer.h>

void toggleLed(int ledPin)
{
  bool isLedOn = digitalRead(ledPin);
  digitalWrite(ledPin, !isLedOn);
}

const unsigned int  BLINK_TIME_MILLIS = 200;

class BlinkTimerAction : public SpinTimerAction
{
public:
  void timeExpired()
  {
    toggleLed(LED_BUILTIN);
  }
};

// The setup function is called once at startup of the sketch
void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  new SpinTimer(BLINK_TIME_MILLIS, new BlinkTimerAction(), SpinTimer::IS_RECURRING, SpinTimer::IS_AUTOSTART);
}

// The loop function is called in an endless loop
void loop()
{
  scheduleTimers();
}
  
