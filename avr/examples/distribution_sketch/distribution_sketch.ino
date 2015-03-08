#define ICSC_COLLISION_DETECTION
#include <ICSC.h>
unsigned char number_of_stations = 1;
#define RS485_BUS_SPEED 115200

#include <Adafruit_NeoPixel.h>
#define PIXEL_PIN 6 //pin of RGB LEDs (chainduino PRIMO 3.1 uses 6, chainduino PRIMO 3.0 used 13)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(2, PIXEL_PIN, NEO_GRB + NEO_KHZ800); //there are 2 RGB LEDs on chainduino PRIMO/MEGA
#define LED_ERROR strip.Color(25, 0, 0)
#define LED_FACTORY strip.Color(5, 1, 0)
#define LED_INITIALIZING strip.Color(5, 25, 0)
#define LED_MASTER strip.Color(5, 5, 5)
#define LED_SLAVE strip.Color(0, 5, 0)
#define LED_RED strip.Color(100, 0, 0)
#define LED_GREEN strip.Color(0, 10, 0)
#define LED_BLUE strip.Color(0, 0, 50)
#define LED_YELLOW strip.Color(5, 4, 0)
#define LED_BLACK strip.Color(0, 0, 0)

#include <EEPROM.h>
#define FACTORY_RESET_PIN 8 // ground this pin to begin initialization sequence

// Machine State
#define STATE_FACTORY  0
#define STATE_MASTER   1
#define STATE_SLAVE    2
#define STATE_CHECK_TRAFFIC 3
#define STATE_GET_ADDRESS 4

#define STATE_ERROR_CANNOT_GET_ADDRESS 99
int systemState = STATE_SLAVE;

// Timeout Events
#define CHECK_FOR_TRAFFIC_DELAY 10000 // allow 10 sec idle time
#define PULSE_WIDTH 250
int twice_pulse_count = 0;
unsigned long stateTimeoutAt = millis();

void setup() {
  // put your setup code here, to run once:
  //configure FACTORY_RESET_PIN as an input and enable the internal pull-up resistor
  pinMode(FACTORY_RESET_PIN, INPUT_PULLUP);
  int factoryResetJumper = digitalRead(FACTORY_RESET_PIN);
  // Fetch previous station number
  byte station = EEPROM.read(0);
  if (factoryResetJumper == LOW) {  // If jumper in place, forget previous station
    systemState = STATE_FACTORY;
    station = 255;
    }
  else if (station == 1) systemState = STATE_MASTER;
  else systemState = STATE_SLAVE;

  ICSC.begin(station, RS485_BUS_SPEED, &Serial, 2);
//  ICSC.registerCommand('N', &NightLight);
//  ICSC.registerCommand('O', &LightOff);
  if (systemState == STATE_MASTER) ICSC.registerCommand('H', &MonitorHeartBeat);
  else ICSC.registerCommand('H', &HeartBeat);

  // Initialize the status LEDs
  strip.begin();
  strip.setPixelColor(0, LED_ERROR);
  strip.setPixelColor(1, LED_BLACK);
  strip.show(); // Initialize the pixels
}

void loop() {
  // put your main code here, to run repeatedly:
  unsigned long currentTime = millis();
  switch (systemState) {
    case STATE_FACTORY:
        { int factoryResetJumper = digitalRead(FACTORY_RESET_PIN);
          if (factoryResetJumper == HIGH) {
            systemState = STATE_CHECK_TRAFFIC;
            stateTimeoutAt = currentTime + CHECK_FOR_TRAFFIC_DELAY;
            }
         }
        break;
     case STATE_CHECK_TRAFFIC:
        if (currentTime > stateTimeoutAt) {  // No traffic become Master
          systemState = STATE_MASTER;
          // This is a new Master -- Initialize view of world
          ICSC.setStation(1);
          EEPROM.write(0,1);
          // reinitialize ICSC
          ICSC.unregisterCommand('H');
          ICSC.registerCommand('H', &MonitorHeartBeat);
          // FINISH THIS STUB
          stateTimeoutAt = currentTime -1;
          }
        break;
    case STATE_GET_ADDRESS:
        if (currentTime > stateTimeoutAt) {  // No node addresses offered
          systemState = STATE_ERROR_CANNOT_GET_ADDRESS;
          // FILL IN THIS STUB
          }
        break;
    case STATE_MASTER:
      if (currentTime > stateTimeoutAt) {
        if (twice_pulse_count > 0) {
          if (twice_pulse_count % 2 ) strip.setPixelColor(1, LED_BLACK);
          else strip.setPixelColor(1, LED_BLUE);
          stateTimeoutAt += PULSE_WIDTH;
          twice_pulse_count--;
          }
        else {
          stateTimeoutAt += CHECK_FOR_TRAFFIC_DELAY*2/3;
          twice_pulse_count = 2 * number_of_stations;
          ICSC.broadcast('H', (char)number_of_stations+1);
          }
        }
      break;
    default:
      ;
    }

  // Check for RS-485 traffic
  ICSC.process(); //for all nodes

  // update the status indicators
  switch (systemState) {
      case STATE_CHECK_TRAFFIC:
        strip.setPixelColor(1, LED_YELLOW);
        strip.setPixelColor(0, LED_FACTORY);
        break;
      case STATE_GET_ADDRESS:
        strip.setPixelColor(1, LED_GREEN);
      case STATE_FACTORY:
        strip.setPixelColor(0, LED_FACTORY);
        break;
      case STATE_MASTER:
        strip.setPixelColor(0, LED_MASTER);
        break;
      case STATE_SLAVE:
        strip.setPixelColor(0, LED_SLAVE);
        break;
      default:
        strip.setPixelColor(0, LED_RED);
  }
  strip.show();
}

void HeartBeat(unsigned char src, char command, unsigned char len, char *data)
{
  // Heartbeat also provides the number of active stations
  switch (systemState) {
    case STATE_CHECK_TRAFFIC:
    case STATE_GET_ADDRESS:
      strip.setPixelColor(1, LED_RED);
      strip.show();
      systemState = STATE_SLAVE;
      ICSC.setStation((unsigned char)*data);
      EEPROM.write(0,(char)ICSC.getStation());
      // reinitialize ICSC
      ICSC.broadcast('H', (char)ICSC.getStation()+1);
      strip.setPixelColor(1, LED_BLACK);
      strip.show();
      break;
    default:
      // Is there anything to do ?
      strip.setPixelColor(1, LED_GREEN);
      strip.show();
      delay(100);
      strip.setPixelColor(1, LED_BLACK);
      strip.show();
	    ;
  }
}

void MonitorHeartBeat(unsigned char src, char command, unsigned char len, char *data)
{
  strip.setPixelColor(0, LED_RED); // Temporary
  unsigned char reporting_station = (unsigned char)*data-1;
  if (reporting_station == 0) {
//    strip.setPixelColor(0, LED_RED);
    strip.show();
    delay(2000);
    reporting_station = 99;
    }
  if (reporting_station > number_of_stations) number_of_stations = reporting_station;
  twice_pulse_count = 2 * number_of_stations;
  strip.setPixelColor(1, LED_GREEN);
  strip.show();
}
