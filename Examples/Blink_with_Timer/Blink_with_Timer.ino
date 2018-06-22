#include <Timer.h>

void toggleLed(int ledPin)
{
  bool isLedOn = digitalRead(ledPin);
  digitalWrite(ledPin, !isLedOn);
}

const unsigned int  BLINK_TIME_MILLIS = 200;

class BlinkTimerAdapter : public TimerAdapter
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
  new Timer(new BlinkTimerAdapter(), Timer::IS_RECURRING, BLINK_TIME_MILLIS);
}

// The loop function is called in an endless loop
void loop()
{
  #ifdef ESP8266
  scheduleTimers();
  #else
  yield();
  #endif
}
  
