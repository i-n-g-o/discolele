#include <Adafruit_NeoPixel.h>
#include <Bounce.h>

#include "TouchButton.h"

#define DO_ANIMATION


#define NUMPIXELS      23

//--------------------------------
// pin defines
#define CAP_1 15
#define CAP_2 16
#define CAP_3 18
#define CAP_4 19

//--------------------------------
// MIDI notes and control values
#define BTN_NOTE 60

#define CAP_NOTE_1 61
#define CAP_NOTE_2 62
#define CAP_NOTE_3 63
#define CAP_NOTE_4 64

#define KNOB_CTRL 11
#define SLIDE_CTRL 12


// the MIDI channel number to send messages
const int channel = 1;

//--------------------------------
// touch panels
TouchButton touch1 = TouchButton(CAP_1, 3000);
TouchButton touch2 = TouchButton(CAP_2, 3000);
TouchButton touch3 = TouchButton(CAP_3, 3000);
TouchButton touch4 = TouchButton(CAP_4, 3000);

//--------------------------------
// poti
int poti_pin = A7;
int potLast = 0;

//--------------------------------
// slider
int slide_pin = A6;
boolean slideOn = false;

//--------------------------------
// button
int btn_pin = 2;
Bounce btn = Bounce(btn_pin, 10);  // 10 ms debounce

//--------------------------------
// led strip - on pin 17
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMPIXELS, 17, NEO_GRB + NEO_KHZ800);
byte stripState = 0;
unsigned long animStart = 0;
long animLast = 0;
uint16_t animDuration = 1000; // milliseconds
byte animSpeed = 10;

Adafruit_NeoPixel shooting = Adafruit_NeoPixel(5, 0, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel shooting1 = Adafruit_NeoPixel(5, 0, NEO_GRB + NEO_KHZ800);

void initShoot() {
  shooting.clear();

  uint32_t c1 = shooting.Color(150, 20, 20);
  uint32_t c2 = shooting.Color(255, 255, 0);
  
  shooting.setPixelColor(0, 150, 20, 20);
  shooting.setPixelColor(1, 180, 50, 80);
  shooting.setPixelColor(2, 220, 100, 40);
  shooting.setPixelColor(3, 80, 150, 40);
  shooting.setPixelColor(4, 200, 150, 0);
  
//  for (int i=0; i<5; i++) {
//    float a = ((float)i) / 4;
//
//    uint32_t c = (c1 * (1.0 - a)) + (c1 * a);
//    
//    shooting.setPixelColor(i, c);
////    shooting.setPixelColor(i, 255, 0, 0);
//  }
}

void initShoot1() {
  
  shooting1.clear();

  uint32_t c1 = shooting.Color(150, 20, 20);
  uint32_t c2 = shooting.Color(255, 255, 0);
  
  shooting1.setPixelColor(0, 0, 20, 0);
  shooting1.setPixelColor(1, 30, 150, 30);
  shooting1.setPixelColor(2, 100, 255, 100);
  shooting1.setPixelColor(3, 30, 150, 30);
  shooting1.setPixelColor(4, 0, 20, 0);
  
//  for (int i=0; i<5; i++) {    
//    shooting1.setPixelColor(i, 255, 0, 0);
//  }
}



void setup() {

  // setup pins
  pinMode(btn_pin, INPUT_PULLUP);

  // add all handlers
  usbMIDI.setHandleNoteOff(OnNoteOff);
  usbMIDI.setHandleNoteOn(OnNoteOn);
  usbMIDI.setHandleVelocityChange(OnVelocityChange);
  usbMIDI.setHandleControlChange(OnControlChange);
  usbMIDI.setHandleProgramChange(OnProgramChange);
  usbMIDI.setHandleAfterTouch(OnAfterTouch);
  usbMIDI.setHandlePitchChange(OnPitchChange);

  initShoot();
  initShoot1();

  // init neopixel strip
  strip.begin();
  strip.clear();
  strip.show();
}

void loop() {

  //------------------------------------------------
  // read touch sensors
  if (touch1.update()) {
    if (touch1.getState()) {
      usbMIDI.sendNoteOn(CAP_NOTE_1, 99, channel);
    } else {
      usbMIDI.sendNoteOff(CAP_NOTE_1, 0, channel);
    }
  }

  if (touch2.update()) {
    if (touch2.getState()) {
      usbMIDI.sendNoteOn(CAP_NOTE_2, 99, channel);
    } else {
      usbMIDI.sendNoteOff(CAP_NOTE_2, 0, channel);
    }
  }

  if (touch3.update()) {
    if (touch3.getState()) {
      usbMIDI.sendNoteOn(CAP_NOTE_3, 99, channel);
    } else {
      usbMIDI.sendNoteOff(CAP_NOTE_3, 0, channel);
    }
  }

  if (touch4.update()) {
    if (touch4.getState()) {
      usbMIDI.sendNoteOn(CAP_NOTE_4, 99, channel);
    } else {
      usbMIDI.sendNoteOff(CAP_NOTE_4, 0, channel);
    }
  }
  

  //------------------------------------------------
  // read knob
  int pot_val = analogRead(poti_pin);
  if (pot_val != potLast) {
    potLast = pot_val;
    usbMIDI.sendControlChange(KNOB_CTRL, map(pot_val, 0, 1023, 0, 127), channel);    
  }

  //------------------------------------------------
  // read slider
  int slide_val = analogRead(slide_pin);
  if (slide_val > 150) {
    slideOn = true;
    slide_val = map(slide_val, 188, 1023, 1, 127);
    usbMIDI.sendControlChange(SLIDE_CTRL, constrain(slide_val, 1, 127), channel);    
  } else if (slideOn) {
    slideOn = false;
    usbMIDI.sendControlChange(SLIDE_CTRL, 0, channel);
  }

  //------------------------------------------------
  // read button
  if (btn.update()) {
    if (btn.fallingEdge()) {
      usbMIDI.sendNoteOn(BTN_NOTE, 99, channel);
    }
    if (btn.risingEdge()) {
      usbMIDI.sendNoteOff(BTN_NOTE, 0, channel);
    }    
  }

  usbMIDI.send_now();

  //------------------------------------------------
  // read midi signals
  while (usbMIDI.read()) {
    // ignore incoming messages
  }


#ifdef DO_ANIMATION
  //------------------------------------------------
  // animate led strip

  if (stripState > 10) {

    unsigned long _now = millis();

    long _delta = 0;
    if (_now < animStart) {
      // type overflow
      _delta = (((unsigned long)-1) - animStart) + _now;
    } else {
      _delta = _now - animStart;
    }
    
    switch(stripState) {
      case 11:
        stripAnim1(_now, _delta);
        break;
      case 12:
        stripAnim2(_now, _delta);
        break;
      case 13:
        stripAnim3(_now, _delta);
        break;
      case 14:
        stripAnim4(_now, _delta);
        break;
      case 15:
        stripAnim5(_now, _delta);
        break;
        
      default:
        stripState = 0;
        stripOff();
        break;
    }

    animLast = _now;
  }
#endif

}


//------------------------------------------------
// handle midi signals
//------------------------------------------------
void OnNoteOn(byte channel, byte note, byte velocity) {

  stripState = note;

  stripOff();

  animStart = millis();
  animLast = animStart;

  switch(stripState) {
    case 0:
      stripOff();
      break;
    case 1:
      stripColor(255, 0, 0);
      break;
    case 2:
      stripColor(0, 255, 0);
      break;
    case 3:
      stripColor(0, 0, 255);
      break;
    case 4:
      stripColor(255, 255, 255);
      break;

    case 13:
      initShoot();
      break;
  }
}

void OnNoteOff(byte channel, byte note, byte velocity) {
}

void OnVelocityChange(byte channel, byte note, byte velocity) {
}

void OnControlChange(byte channel, byte control, byte value) {
  animDuration = (control << 8) + value;
}

void OnProgramChange(byte channel, byte program) {
}

void OnAfterTouch(byte channel, byte pressure) {
}

void OnPitchChange(byte channel, int pitch) {
}


//------------------------------------------------
// strip aniamtions
//------------------------------------------------
void stripOff() {
  strip.clear();
  strip.show();
}

void animOff() {
  stripOff();
  stripState = 0;
}

void stripColor(uint8_t r, uint8_t g, uint8_t b) {
  for(int i=0; i<NUMPIXELS; i++) {
    strip.setPixelColor(i, r, g, b);
  }
  strip.show();
}

void stripAnim1(unsigned long _now, long _delta) {

  long d = _delta % (long)animDuration;
  float f = (float)d / animDuration;
  
  int num = (int)(f * (float)NUMPIXELS);

  for(int i=0; i<NUMPIXELS; i++) {
    if (i < num) {
      strip.setPixelColor(i, 255, 255, 255);
    } else {
      strip.setPixelColor(i, 0, 0, 0);
    }
  }

  strip.show();
}

void stripAnim2(unsigned long _now, long _delta) {

  float duration = animDuration;

  long d = _delta % (long)duration;

  uint32_t c = strip.Color(255, 255, 255);
  if (d < (duration/2)) {
    c = strip.Color(0, 0, 0);
  }

  for(int i=0; i<NUMPIXELS; i++) {
    strip.setPixelColor(i, c);
  }

  strip.show();
  
}

void blit(uint8_t* t, int tpc, uint8_t* s, int spc, long pixel) {

  byte cnum = 3;

  long tl = tpc * cnum;
  long sl = spc * cnum;

  int to = 0;
  if (pixel > 0) {
    to = pixel * cnum;
  }

  int so = 0;
  if (pixel < 0) {
    so += abs(pixel) * cnum;
  }

  int l = sl - so;

  // we should sanity check here...?

  if (l > 0) {
    memcpy(t + to, s + so, l);
  }
}

void stripAnim3(unsigned long _now, long _delta) {

  if (_delta > animDuration) {
    animOff();
    return;
  }

  Adafruit_NeoPixel& obj = shooting;
  
  float percent = (float)(_delta % (long)animDuration) / animDuration; 

  int shooting_pc = obj.numPixels();
  int virtualNum = shooting_pc + NUMPIXELS;
  
  int off = (int)((float)virtualNum * percent) - shooting_pc;

  strip.clear();
  blit(strip.getPixels(), NUMPIXELS, obj.getPixels(), shooting_pc, off);
  strip.show();
}

void stripAnim4(unsigned long _now, long _delta) {

//  if (_delta > 2*animDuration) {
//    animOff();
//    return;
//  }

  _delta = _delta % (2*animDuration);

  Adafruit_NeoPixel& obj = shooting1;
 
  float percent = (float)(_delta % (long)animDuration) / animDuration; 

  int shooting_pc = obj.numPixels();
  int virtualNum = shooting_pc + NUMPIXELS;
  
  int off = 0;
  if (_delta <= animDuration) {
    off = (int)((float)virtualNum * percent) - shooting_pc;
  } else {
    off = (int)((float)virtualNum * (1.0 - percent)) - shooting_pc;
  }
  
  strip.clear();  
  blit(strip.getPixels(), NUMPIXELS, obj.getPixels(), shooting_pc, off);
  strip.show();  
}

void stripAnim5(unsigned long _now, long _delta) {

  if (_delta > animDuration) {
    animOff();
    return;
  }

  long d = _delta % 50;
  
  if (d < 10) {
    for(int i=0; i<NUMPIXELS; i++) {
      strip.setPixelColor(i, random(255), 0, 0);
    }
    strip.show();
  }

}
