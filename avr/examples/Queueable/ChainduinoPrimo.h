#include <Adafruit_NeoPixel.h>

class pixels {
public:
  pixels(){};
private:
#define PIXEL_PIN 6 //pin of RGB LEDs (chainduino PRIMO 3.1 uses 6, chainduino PRIMO 3.0 used 13)
  Adafruit_NeoPixel communicator = Adafruit_NeoPixel(2, PIXEL_PIN, NEO_GRB + NEO_KHZ800); //there are 2 RGB LEDs on chainduino PRIMO/MEGA
  bool dirty = true;
};

class Indicator {
public:
  Indicator(){};
  virtual void activate();
  virtual void deactivate();
  virtual bool begin();
};

class ActiveHighLed :Indicator {
public:
  ActiveHighLed(byte pin) { _pin = pin; };
  bool begin() {
    pinMode(_pin,OUTPUT);
    deactivate();
  }
  void activate() {
    digitalWrite(_pin, HIGH);
  }
  void deactivate() {
    digitalWrite(_pin, LOW);
  }
private:
  byte _pin;
};

class ActiveLowLed :Indicator {
public:
  ActiveLowLed(byte pin) { _pin = pin; };
  bool begin() {
    pinMode(_pin,OUTPUT);
    deactivate();
  }
  void activate() {
    digitalWrite(_pin, LOW);
  }
  void deactivate() {
    digitalWrite(_pin, HIGH);
  }
private:
  byte _pin;
};

class Color {
public:
  Color(byte R, byte G, byte B) {

  }
private:

};

class ColoredLed :Indicator {
public:
  ColoredLed(byte pin) { _pin = pin; };
  bool begin() {
    pinMode(_pin,OUTPUT);
    deactivate();
  }
  void activate() {
    digitalWrite(_pin, LOW);
  }
  void deactivate() {
    digitalWrite(_pin, HIGH);
  }
  bool setColor() {};
private:
  byte _pin;
};

ActiveHighLed Heart(13);