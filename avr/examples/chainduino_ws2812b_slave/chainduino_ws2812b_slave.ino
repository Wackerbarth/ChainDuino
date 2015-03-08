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
int alternate = 0;

void setup()
{
  ICSC.begin(5, 115200, &Serial, 2);
  ICSC.registerCommand('R', &redFade);
  ICSC.registerCommand('G', &greenFade);
  ICSC.registerCommand('B', &blueFade);
  ICSC.registerCommand('W', &whiteFade);

  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}

void loop()
{
  ICSC.process();
  unsigned long currentMillis = millis();
  if(currentMillis - previousMillis > interval) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;
    red = 5;
    green = 5;
    blue = 5;
  }
  strip.setPixelColor(0, red, green, blue);
  strip.show();
}

void redFade(unsigned char src, char command, unsigned char len, char *data)
{
  unsigned long currentMillis = millis();
  red = hi;
  green = lo;
  blue = lo;
  previousMillis = currentMillis;
}

void blueFade(unsigned char src, char command, unsigned char len, char *data)
{
  unsigned long currentMillis = millis();
  red = lo;
  green = lo;
  blue = hi;
  previousMillis = currentMillis;
}
void greenFade(unsigned char src, char command, unsigned char len, char *data)
{
  unsigned long currentMillis = millis();
  red = lo;
  green = hi;
  blue = lo;
  previousMillis = currentMillis;
}

void whiteFade(unsigned char src, char command, unsigned char len, char *data)
{
  unsigned long currentMillis = millis();
  red = hi;
  green = hi;
  blue = hi;
  previousMillis = currentMillis;
}


