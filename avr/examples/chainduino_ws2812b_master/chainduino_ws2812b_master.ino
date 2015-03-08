#include <ICSC.h>
#include <Adafruit_NeoPixel.h>
#define PIN 6
Adafruit_NeoPixel strip = Adafruit_NeoPixel(1, PIN, NEO_GRB + NEO_KHZ800);

long previousMillis = 0;        // will store last time LED was updated
long interval = 500;           // interval at which to blink (milliseconds)
int lo = 0;
int hi = 127;
int red = 0;
int green = 0;
int blue = 0;
int fadeDir = 1;
int alternate = 1;

void setup()
{
  ICSC.begin(1, 115200, &Serial, 2);
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}

void loop()
{
  unsigned long currentMillis = millis();
  if(currentMillis - previousMillis > interval) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;

    if (alternate == 1) {red=lo; green=hi; blue=lo; }
    if (alternate == 2) {ICSC.send(0, 'G', 0, NULL); }
    //if (alternate == 3) {ICSC.send(0, 'G', 0, NULL); }
    if (alternate == 4) {ICSC.send(2, 'G', 0, NULL); }
    if (alternate == 5) {ICSC.send(3, 'G', 0, NULL); }
    if (alternate == 6) {ICSC.send(4, 'G', 0, NULL); }
    if (alternate == 7) {ICSC.send(5, 'G', 0, NULL); }
    if (alternate == 8) {ICSC.send(6, 'G', 0, NULL); }
    if (alternate == 9) {ICSC.send(7, 'G', 0, NULL); }
    if (alternate == 10) {ICSC.send(8, 'G', 0, NULL); }
    if (alternate == 11) {ICSC.send(9, 'G', 0, NULL); }
    if (alternate == 12) {red=5; green=5; blue=5; }

    if (alternate == 13) {red=lo; green=lo; blue=hi; }
    if (alternate == 14) {ICSC.send(0, 'B', 0, NULL); }
    //if (alternate == 3) {ICSC.send(0, 'B', 0, NULL); }
    if (alternate == 16) {ICSC.send(2, 'B', 0, NULL); }
    if (alternate == 17) {ICSC.send(3, 'B', 0, NULL); }
    if (alternate == 18) {ICSC.send(4, 'B', 0, NULL); }
    if (alternate == 19) {ICSC.send(5, 'B', 0, NULL); }
    if (alternate == 20) {ICSC.send(6, 'B', 0, NULL); }
    if (alternate == 21) {ICSC.send(7, 'B', 0, NULL); }
    if (alternate == 22) {ICSC.send(8, 'B', 0, NULL); }
    if (alternate == 23) {ICSC.send(9, 'B', 0, NULL); }
    if (alternate == 24) {red=5; green=5; blue=5; }

    if (alternate == 25) {red=hi; green=lo; blue=lo; }
    if (alternate == 26) {ICSC.send(0, 'R', 0, NULL); }
    //if (alternate == 3) {ICSC.send(0, 'R', 0, NULL); }
    if (alternate == 28) {ICSC.send(2, 'R', 0, NULL); }
    if (alternate == 29) {ICSC.send(3, 'R', 0, NULL); }
    if (alternate == 30) {ICSC.send(4, 'R', 0, NULL); }
    if (alternate == 31) {ICSC.send(5, 'R', 0, NULL); }
    if (alternate == 32) {ICSC.send(6, 'R', 0, NULL); }
    if (alternate == 33) {ICSC.send(7, 'R', 0, NULL); }
    if (alternate == 34) {ICSC.send(8, 'R', 0, NULL); }
    if (alternate == 35) {ICSC.send(9, 'R', 0, NULL); }
    if (alternate == 36) {red=5; green=5; blue=5; }

    // set the LED with the ledState of the variable:
    strip.setPixelColor(0, red, green, blue);
    strip.show();
    alternate = alternate + 1;
    if (alternate > 36) {alternate = 1;}
  }
}
