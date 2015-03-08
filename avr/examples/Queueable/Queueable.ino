#include <Adafruit_NeoPixel.h>

#include "ChainduinoPrimo.h"

int Heartbeat = 500;

void setup() {
  // put your setup code here, to run once:
}

void loop() {
  // put your main code here, to run repeatedly:
  Heart.activate();
  delay(500);
  Heart.deactivate();
  delay(1500);
}
