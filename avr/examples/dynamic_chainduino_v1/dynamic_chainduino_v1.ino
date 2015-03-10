#include <ICSC.h>

//NEOPIXEL Stuff
#ifdef HAS_NEOPIXEL_ARRAY
  //There are 2 RGB LEDs on chainduino PRIMO/MEGA
  #include <Adafruit_NeoPixel.h>
  static Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMBER_OF_NEOPIXEL_ELEMENTS, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);
#endif

// TIME
unsigned long currentMillis = millis();
  
//STATION and POLLING settings (most apply to master station 1)
unsigned int station = 3;  // change station number here (BEFORE uploading to each node)
unsigned int stations = 3; // number of stations
unsigned int POLLstep = 0; // current polling step
unsigned int POLLsteps = 1; // total number of polling steps
 //delay between polling for data from nodes
   #define POLLspeed 100
long POLLtime; //time most recent POLL was run
long PULSEtime; //record last time heartbeat received from master
 //how often to blink good heartbeat status
   #define PULSEgoodspeed 5000 
 //how often to blink FLAT heartbeat status
   #define PULSEflatspeed 2500
boolean PULSEflat = false; //controls the start of the red flashing if pulse dies
boolean PULSEdetect = false; //controls the start of the green flashing if pulse detected (only if LEDs are off)
//end station and polling settings

//LED variables
long LEDbirth; // will store last time LED was updated (color set)
unsigned int LEDlifespan = 500; // how long the LED stays on
unsigned int LEDstepspeed = 50; // speed of which to update the LED
unsigned int LEDstepsize = 5;
long LEDsteptime; // will store last time color step was sent to the LED
int Rset = 0;
int Gset = 0;
int Bset = 0;
int Rnow = 0;
int Gnow = 0;
int Bnow = 0;
//end LED stuff

//LED white color temps for reference
//colorWipe(strip.Color(255, 255, 255), 100); // White 6000k
//colorWipe(strip.Color(255, 250, 254), 100); // White 5200k
//colorWipe(strip.Color(255, 241, 224), 1000); // White 3200k halogen
//colorWipe(strip.Color(255, 214, 170), 100); // White 2850k
//colorWipe(strip.Color(255, 197, 143), 100); // White 2600k
//colorWipe(strip.Color(255, 148, 47), 100); // White 1900k candle

unsigned int timeSince(unsigned long ref) {
  return (static_cast<unsigned int>(currentMillis - ref));
}

// void HeartBeat(unsigned char src, char command, unsigned char len, char *data);

void setup()
{
  ICSC.begin(station, 115200, &Serial, 2);
//  ICSC.registerCommand('N', &NightLight);
//  ICSC.registerCommand('O', &LightOff);
  ICSC.registerCommand('H', &HeartBeat);

  // Initialize the life clocks
  currentMillis = millis();
  PULSEtime = currentMillis;
  POLLtime = currentMillis;
  LEDsteptime = currentMillis;
  LEDbirth = currentMillis;
  strip.begin();
  if (station == 1) 
    strip.setPixelColor(0, 12, 7, 2);
  else
    strip.setPixelColor(0, 5, 10, 0);
  
  strip.show(); // Initialize all pixels
}

void loop()
{
  currentMillis = millis();
  ICSC.process(); //for all nodes

  if (timeSince(POLLtime) > POLLspeed) { //specified delay between POLL steps
    POLLtime = currentMillis; //reset last poll time
    //only for master
    if (station == 1) {
      if (POLLstep == 0) {ICSC.send(0, 'H', 0, NULL);} //broadcast hearbeat
      if (POLLstep == 1) {}
      if (POLLstep == 2) {}
      if (POLLstep == 3) {}
    }//end master ONLY stuff

    //next poll step (all nodes)
    POLLstep = POLLstep + 1; //next POLLstep (all nodes)
    if (POLLstep > POLLsteps) {POLLstep = 0;} //reset to first pollstep
  }//end poll

  //LOCAL - kills the LED if the timer is up (starts the dimming process)
  if (timeSince(LEDbirth) > LEDlifespan) { Rset = 0; Gset = 0; Bset = 0; }
  //end of kill LED


  //HEARTBEAT indicator for slaves (slaves flash red if master goes silent too long)
  //must recieve broadcast heartbeat from master once per POLLING loop
  if (station > 1) {
    //only for slaves
    if (timeSince(PULSEtime) > ((POLLsteps+2)*POLLspeed) ) {
      //only runs if hearbeat not recieved in allowed time
      if (!PULSEflat) {PULSEflat = true; Rnow=0; Gnow=0; Bnow=0; Rset=0; Gset=0; Bset=0;}
      if (timeSince(LEDbirth) > PULSEflatspeed) { //only blinks so often
        if ((Rnow+Gnow+Bnow) == 0) {
          //only lights the red LED again if the LED has since faded out (has regular lifespan)
          Rset = 20; Gset = 0; Bset = 0; LEDbirth = currentMillis; LEDlifespan = 1000; LEDstepspeed = 50; LEDstepsize = 1;
        }
      }
    }//end flatline
    if (PULSEdetect) {//heartbeat recently detected
      PULSEdetect = false;
      if ((Rset+Gset+Bset) == 0) {
        //only blinks if the LEDs are idle, not being used
        if (timeSince(LEDbirth) > PULSEgoodspeed) { //only blinks so often
          Rset = 0; Gset = 5; Bset = 0; LEDbirth = currentMillis; LEDlifespan = 30; LEDstepspeed = 10; LEDstepsize = 1;
        }
      }
    }//end detected green blink
  }//end heatbeat indicators


  //LOCAL - updates the actual LED in stages at whatever speed currently set (could be going up or killing it)
  if (timeSince(LEDsteptime) > LEDstepspeed) {
    LEDsteptime = currentMillis; // save the last time you updated the LED
    if (Rnow < Rset) {if (Rset - Rnow > 4) {Rnow = Rnow + LEDstepsize;} else {Rnow = Rnow + 1;} }
    if (Rnow > Rset) {if (Rnow - Rset > 4) {Rnow = Rnow - LEDstepsize;} else {Rnow = Rnow - 1;} }
    if (Gnow < Gset) {if (Gset - Gnow > 4) {Gnow = Gnow + LEDstepsize;} else {Gnow = Gnow + 1;} }
    if (Gnow > Gset) {if (Gnow - Gset > 4) {Gnow = Gnow - LEDstepsize;} else {Gnow = Gnow - 1;} }
    if (Bnow < Bset) {if (Bset - Bnow > 4) {Bnow = Bnow + LEDstepsize;} else {Bnow = Bnow + 1;} }
    if (Bnow > Bset) {if (Bnow - Bset > 4) {Bnow = Bnow - LEDstepsize;} else {Bnow = Bnow - 1;} }
    strip.setPixelColor(1, Rnow, Gnow, Bnow);
    strip.show();
  }
  //end LED update section

}//end main loop | end main loop | end main loop | end main loop | end main loop | end main loop

//void NightLight(unsigned char src, char command, unsigned char len, char *data)
//{
//  unsigned long currentMillis = millis();
//   Rset = 255;
//   Gset = 148;
//   Bset = 47;
//  previousMillisLight = currentMillis;
//  lightinterval = 5000;
//  lightupdate = 50;
//}
//void MasterNightLight()
//{
//  unsigned long currentMillis = millis();
//   Rset = 255;
//   Gset = 148;
//   Bset = 47;
//  previousMillisLight = currentMillis;
//  lightinterval = 5000;
//  lightupdate = 50;
//}
//
//void LightOff(unsigned char src, char command, unsigned char len, char *data)
//{
//  unsigned long currentMillis = millis();
//   Rset = 0;
//   Gset = 0;
//   Bset = 0;
//  previousMillisLight = currentMillis;
//  lightinterval = 100;
//  lightupdate = 5;
//}

void HeartBeat(unsigned char src, char command, unsigned char len, char *data)
{
  PULSEflat = false; //disable the pulse flat flag
  PULSEdetect = true; //trigger the little green blip if LEDs are currently idle (off)
  PULSEtime = currentMillis;
}

