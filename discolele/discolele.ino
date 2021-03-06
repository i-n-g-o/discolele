/*
 * Firmware for discolele for Teensy 3.2
 * 
 * GNU GPL
 */
#include <Adafruit_NeoPixel.h>
#include <Bounce.h>

#include "TouchButton.h"

#define DO_ANIMATION
//#define DO_SERIAL


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

#define SLIDER_NOTE_OFF 80
#define SLIDER_NOTE_1 81
#define SLIDER_NOTE_2 82
#define SLIDER_NOTE_3 83
#define SLIDER_NOTE_4 84
#define SLIDER_NOTE_MAX 85


#define KNOB_CTRL 11
#define SLIDE_CTRL 12

#define KNOB_NOTE_OFF 90
#define KNOB_NOTE_ON 91


// the MIDI channel number to send messages
const int channel = 1;

//--------------------------------
// touch panels
TouchButton touch1 = TouchButton(CAP_1, 1600);
TouchButton touch2 = TouchButton(CAP_2, 1600);
TouchButton touch3 = TouchButton(CAP_3, 1600);
TouchButton touch4 = TouchButton(CAP_4, 1600);

//--------------------------------
// poti
int poti_pin = A7;
int potLast = 0;
boolean potOn;

//--------------------------------
// slider
int slide_pin = A6;
boolean slideOn = false;
int lastSlider = 0;

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
uint16_t animDuration = 300; // milliseconds


Adafruit_NeoPixel shooting = Adafruit_NeoPixel(5, 0, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel shooting1 = Adafruit_NeoPixel(5, 0, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel shootingSinglePixel = Adafruit_NeoPixel(1, 0, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel shootingSinglePixel2 = Adafruit_NeoPixel(1, 0, NEO_GRB + NEO_KHZ800);

// colors
byte color1_r = 255;
byte color1_g = 255;
byte color1_b = 255;
uint32_t color1 = strip.Color(color1_r, color1_g, color1_b);
byte color2_r = 0;
byte color2_g = 0;
byte color2_b = 0;
uint32_t color2 = strip.Color(color2_r, color2_g, color2_b);

long pause_started = 0;

boolean animation_flip = false;

void initAllShootings() {
  initShoot();
  initShoot1();
  initShootingSinglePixel();
  initShootingSinglePixel2();
}

void initShoot() {
  shooting.clear();

//  uint32_t c1 = shooting.Color(150, 20, 20);
//  uint32_t c2 = shooting.Color(255, 255, 0);

//    shooting.setPixelColor(0, 150, 20, 20);
//  shooting.setPixelColor(1, 180, 50, 80);
//  shooting.setPixelColor(2, 220, 100, 40);
//  shooting.setPixelColor(3, 80, 150, 40);
//  shooting.setPixelColor(4, 200, 150, 0);
  
  shooting.setPixelColor(0, color1);
  shooting.setPixelColor(1, color1);
  shooting.setPixelColor(2, color1);
  shooting.setPixelColor(3, color1);
  shooting.setPixelColor(4, color1);
  
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

//  uint32_t c1 = shooting.Color(150, 20, 20);
//  uint32_t c2 = shooting.Color(255, 255, 0);
  
  shooting1.setPixelColor(0, color1);
  shooting1.setPixelColor(1, color1);
  shooting1.setPixelColor(2, color1);
  shooting1.setPixelColor(3, color1);
  shooting1.setPixelColor(4, color1);
  
//  for (int i=0; i<5; i++) {    
//    shooting1.setPixelColor(i, 255, 0, 0);
//  }
}


void initShootingSinglePixel() {
  shootingSinglePixel.clear();
  shootingSinglePixel.setPixelColor(0, color1);
}

void initShootingSinglePixel2() {
  shootingSinglePixel2.clear();
  shootingSinglePixel2.setPixelColor(0, color2);
}


void setup() {

//  pinMode(CAP_1, INPUT_PULLUP);
//  pinMode(CAP_2, INPUT_PULLUP);
//  pinMode(CAP_3, INPUT_PULLUP);
//  pinMode(CAP_4, INPUT_PULLUP);

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

  initAllShootings();

  // init neopixel strip
  strip.begin();
  strip.clear();
  strip.show();

#ifdef DO_SERIAL
  Serial.begin(9600);
  Serial.println("init");
#endif
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
    //usbMIDI.sendControlChange(KNOB_CTRL, map(pot_val, 0, 1023, 0, 127), channel);    
  }

  if (!potOn && potLast > 500) {
    potOn = true;
    usbMIDI.sendNoteOn(KNOB_NOTE_ON, 99, channel);
    
  } else if (potOn && potLast <= 500) {
    potOn = false;
    usbMIDI.sendNoteOn(KNOB_NOTE_OFF, 99, channel);
  }
  

  //------------------------------------------------
  // read slider
  int slide_val = analogRead(slide_pin);
  if (slide_val > 150) {
    slideOn = true;
    slide_val = map(slide_val, 188, 1023, 1, 127);

    if (potOn) {
      // knob is "on"
      if (slide_val > 1) {
        if (slide_val < 12) {
          if (lastSlider != SLIDER_NOTE_1) {
            lastSlider = SLIDER_NOTE_1;
            usbMIDI.sendNoteOn(SLIDER_NOTE_1, 99, channel);
          }      
        } else if (slide_val < 22) {
          if (lastSlider != SLIDER_NOTE_2) {
            lastSlider = SLIDER_NOTE_2;
            usbMIDI.sendNoteOn(SLIDER_NOTE_2, 99, channel);
          }      
        } else if (slide_val < 40) {
          if (lastSlider != SLIDER_NOTE_3) {
            lastSlider = SLIDER_NOTE_3;
            usbMIDI.sendNoteOn(SLIDER_NOTE_3, 99, channel);
          }      
        } else if (slide_val < 100) {
          if (lastSlider != SLIDER_NOTE_4) {
            lastSlider = SLIDER_NOTE_4;
            usbMIDI.sendNoteOn(SLIDER_NOTE_4, 99, channel);
          }      
        } else if (lastSlider != SLIDER_NOTE_MAX) {
          lastSlider = SLIDER_NOTE_MAX;
          usbMIDI.sendNoteOn(SLIDER_NOTE_MAX, 99, channel);
        }
      }
    } else {
      // knob off
      if (slide_val >= 100 && lastSlider != SLIDER_NOTE_MAX) {
        lastSlider = SLIDER_NOTE_MAX;
        usbMIDI.sendNoteOn(SLIDER_NOTE_MAX, 99, channel);
      }
    }
    
    //usbMIDI.sendControlChange(SLIDE_CTRL, constrain(slide_val, 1, 127), channel);    
  } else if (slideOn) {
    slideOn = false;
    //usbMIDI.sendControlChange(SLIDE_CTRL, 0, channel);
    lastSlider = SLIDER_NOTE_OFF;
    usbMIDI.sendNoteOn(SLIDER_NOTE_OFF, 99, channel);
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

  if (pause_started == 0) {

    unsigned long _now = millis();

    long _delta = 0;
    if (_now < animStart) {
      // type overflow
      _delta = (((unsigned long)-1) - animStart) + _now;
    } else {
      _delta = _now - animStart;
    }
    
    switch(stripState) {

      case 69:
        stripOff();
        break;
      case 35: // E-6
        laserShootLoop(_now, _delta);
        break;
      case 36: // F-6
        blinkLoop(_now, _delta);
        break;
      case 42: // FIS-6
        laserShot(_now, _delta);
        break;
      case 39: // G-6
        kittAnimation(_now, _delta);
        break;
      case 37: // GIS-6
        randomBrightness(_now, _delta);
        break;
      case 38: // A-6
        fullColor(_now, _delta);
        break;
      case 46:
        colorfade(_now, _delta);
        break;
      case 44:
        breathe(_now, _delta);
        break;
      case 60:
        randomWithRandomColor(_now, _delta);
        break;
      case 62:
        colorFadeLoop(_now, _delta);
        break;
      case 64:
        reverseBreathe(_now, _delta);
        break;
      case 65:
        kittSinglePixel(_now, _delta);
        break;
      case 67:
        kittDualPixel(_now, _delta);
        break;
         
//      default:
//        stripState = 0;
//        stripOff();
//        break;
    }

    animLast = _now;
  }
#endif

}


//------------------------------------------------
// handle midi signals
//------------------------------------------------
void OnNoteOn(byte channel, byte note, byte velocity) {
#ifdef DO_SERIAL
  Serial.print("note: ");
  Serial.print(channel);
  Serial.print(" - ");
  Serial.print(note);
  Serial.print(" - ");
  Serial.println(velocity); 
#endif

  // light only channel 2
  if (channel != 2) {
    return;
  }

  switch(note) {
//    case 0:
//      stripOff();
//      break;
//    case 1:
//      stripColor(255, 0, 0);
//      break;
//    case 2:
//      stripColor(0, 255, 0);
//      break;
//    case 3:
//      stripColor(0, 0, 255);
//      break;
//    case 4:
//      stripColor(255, 255, 255);
//      break;
//
//    case 13:
//      initShoot();
//      break;

    // speed
//    case 24: // C-0
//      animDuration = animDuration1;
//      break;
//    case 25: // C#-0
//      animDuration = animDuration2;
//      break;
//    case 26: // D-0
//      animDuration = animDuration3;
//      break;

    case 71:
      if (pause_started == 0) {
        pause_started = millis();
      }      
      break;
    case 72:
      if (pause_started > 0) {
        //adjust
        long pause_duration = millis() - pause_started;       
        animStart += pause_duration;
        pause_started = 0;
      }
      break;

    default:
      // animations
      stripState = note;
      pause_started = 0;
      stripOff();
      animStart = millis();
      animLast = animStart;
      animation_flip = false;     
  }
}

void OnNoteOff(byte channel, byte note, byte velocity) {
}

void OnVelocityChange(byte channel, byte note, byte velocity) {
}

void OnControlChange(byte channel, byte control, byte value) {
  //animDuration = (control << 8) + value;
  
  // light only channel 2
  if (channel != 2) {
    return;
  }

  value = map(value, 0, 127, 0, 255);

  switch (control) {

    // intensity
    case 4:
      strip.setBrightness(value);
      initAllShootings();
      shooting.setBrightness(value);
      shooting1.setBrightness(value);
      shootingSinglePixel.setBrightness(value);
      shootingSinglePixel2.setBrightness(value);
      break;

    // speed
    case 8:
      animDuration = map(value, 0, 255, 2, 2000);
      break;


    // color 1
    case 1: // R      
      color1_r = value;
      color1 = strip.Color(color1_r, color1_g, color1_b);     
      initAllShootings();
      break;
    case 2: // G
      color1_g = value;
      color1 = strip.Color(color1_r, color1_g, color1_b);
      initAllShootings();
      break;
    case 3: // B
      color1_b = value;
      color1 = strip.Color(color1_r, color1_g, color1_b);
      initAllShootings();
      break;

    // color 2
    case 5: // R      
      color2_r = value;
      color2 = strip.Color(color2_r, color2_g, color2_b);
      initShootingSinglePixel2();
      break;
    case 6: // G
      color2_g = value;
      color2 = strip.Color(color2_r, color2_g, color2_b);
      initShootingSinglePixel2();
      break;
    case 7: // B
      color2_b = value;
      color2 = strip.Color(color2_r, color2_g, color2_b);
      initShootingSinglePixel2();
      break;   
  }
  
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


/**
 * swipe
 */
void laserShootLoop(unsigned long _now, long _delta) {

  long d = _delta % (long)animDuration;
  float f = (float)d / animDuration;
  
  int num = (int)(f * (float)NUMPIXELS);

  for(int i=0; i<NUMPIXELS; i++) {
    if (i < num) {
      strip.setPixelColor(i, color1);
    } else {
      strip.setPixelColor(i, color2);
    }
  }

  strip.show();
}


/**
 * full color1
 */
void fullColor(unsigned long _now, long _delta) {

  for(int i=0; i<NUMPIXELS; i++) {
    strip.setPixelColor(i, color1);
  }
  
  strip.show();
}


/**
 * full fade from color1 to color2
 */
void colorfade(unsigned long _now, long _delta) {

  if (_delta >= animDuration) {
    for(int i=0; i<NUMPIXELS; i++) {
      strip.setPixelColor(i, color2);
    }
    strip.show();
    
    return;
  }
  
//  long d = _delta % (long)animDuration;
  float f = (float)_delta / animDuration;

  long time_thresh = _delta % 50;
  
  byte r = (byte)((float)color1_r*(1.0-f) + (float)color2_r*f);
  byte g = (byte)((float)color1_g*(1.0-f) + (float)color2_g*f);
  byte b = (byte)((float)color1_b*(1.0-f) + (float)color2_b*f); 
  
  for(int i=0; i<NUMPIXELS; i++) {
    strip.setPixelColor(i, r, g, b);
  }

  strip.show();
}



/**
 * full fade in out
 */
void colorFadeLoop(unsigned long _now, long _delta) {

  if (_delta >= animDuration) {
//    for(int i=0; i<NUMPIXELS; i++) {
//      if (animation_flip) {
//        strip.setPixelColor(i, color1);
//      } else {
//        strip.setPixelColor(i, color2);
//      }
//    }
//    strip.show();

    animStart = millis();
    animLast = animStart;

    animation_flip = !animation_flip;
    
    return;
  }
  
  long d = _delta % (long)animDuration;

  float f = (float)d / animDuration;
  if (animation_flip) {
    f = 1.0 - ((float)d / animDuration);
  }

  float f1 = f*f;

  long time_thresh = _delta % 50;
  
  byte r = (byte)((float)color1_r*(f1) + (float)color2_r*(1.0-f1));
  byte g = (byte)((float)color1_g*(f1) + (float)color2_g*(1.0-f1));
  byte b = (byte)((float)color1_b*(f1) + (float)color2_b*(1.0-f1)); 

  int half = NUMPIXELS/2-1;
  float half_f = (NUMPIXELS/2 * f) - 3;
  
  for (int i=0; i<NUMPIXELS; i++) {
    strip.setPixelColor(i, r, g, b);
  }

  strip.show();
}



/**
 * breathe
 */
void breathe(unsigned long _now, long _delta) {

  if (_delta >= animDuration) {
//    for(int i=0; i<NUMPIXELS; i++) {
//      if (animation_flip) {
//        strip.setPixelColor(i, color1);
//      } else {
//        strip.setPixelColor(i, color2);
//      }
//    }
//    strip.show();

    animStart = millis();
    animLast = animStart;

    animation_flip = !animation_flip;
    
    return;
  }
  
  long d = _delta % (long)animDuration;

  float f = (float)d / animDuration;
  if (animation_flip) {
    f = 1.0 - ((float)d / animDuration);
  }

  float f1 = f*f*f*f;

  long time_thresh = _delta % 50;
  
  byte r = (byte)((float)color1_r*(f1) + (float)color2_r*(1.0-f1));
  byte g = (byte)((float)color1_g*(f1) + (float)color2_g*(1.0-f1));
  byte b = (byte)((float)color1_b*(f1) + (float)color2_b*(1.0-f1)); 

  int half = NUMPIXELS/2-1;
  float half_f = (NUMPIXELS/2 * f) - 3;
  
  for (int i=0; i<NUMPIXELS; i++) {

    if (i < (half - half_f ) || i > (half + half_f-1)) {
      strip.setPixelColor(i, 0, 0, 0);
    } else {

      float v = (float)(half_f - abs(half - i)) / (float)half_f;
      
      strip.setPixelColor(i, r*v, g*v, b*v);
    }
    
    
  }

  strip.show();
}


/**
 * reverse breathe
 */
void reverseBreathe(unsigned long _now, long _delta) {

  if (_delta >= animDuration) {
//    for(int i=0; i<NUMPIXELS; i++) {
//      if (animation_flip) {
//        strip.setPixelColor(i, color1);
//      } else {
//        strip.setPixelColor(i, color2);
//      }
//    }
//    strip.show();

    animStart = millis();
    animLast = animStart;

    animation_flip = !animation_flip;
    
    return;
  }
  
  long d = _delta % (long)animDuration;

  float f = (float)d / animDuration;
  if (animation_flip) {
    f = 1.0 - ((float)d / animDuration);
  }

  float f1 = f*f;

  long time_thresh = _delta % 50;
  
  byte r = (byte)((float)color1_r*(f1) + (float)color2_r*(1.0-f1));
  byte g = (byte)((float)color1_g*(f1) + (float)color2_g*(1.0-f1));
  byte b = (byte)((float)color1_b*(f1) + (float)color2_b*(1.0-f1)); 

  int half = NUMPIXELS/2;
  float half_f = (NUMPIXELS/2 * f) - 3;
  
  for (int i=0; i<NUMPIXELS; i++) {

    if (i < (half - half_f ) || i > (half + half_f)) {
      float v = (float)(half_f - abs(half - i)) / (float)half_f;      
      strip.setPixelColor(i, r*v, g*v, b*v);
    } else {
      strip.setPixelColor(i, 0, 0, 0);
    }
    
    
  }

  strip.show();
}



/**
 * full blink
 */
void blinkLoop(unsigned long _now, long _delta) {

  float duration = animDuration;

  long d = _delta % (long)duration;

  uint32_t c = color1;
  if (d < (duration/2)) {
    c = color2;
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

/**
 * shoot
 */
void laserShot(unsigned long _now, long _delta) {

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

/**
 * K.I.T.T animation
 */
void kittAnimation(unsigned long _now, long _delta) {

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

/**
 * K.I.T.T single pixel
 */
void kittSinglePixel(unsigned long _now, long _delta) {

  _delta = _delta % (2*animDuration);

  Adafruit_NeoPixel& obj = shootingSinglePixel;
 
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


/**
 * K.I.T.T dual pixel
 */
void kittDualPixel(unsigned long _now, long _delta) {

//  long d = _delta % (long)(2*animDuration);
//  float percent2 = (float)d / animDuration;

  _delta = _delta % (2*animDuration);
  

  Adafruit_NeoPixel& obj = shootingSinglePixel;
  Adafruit_NeoPixel& obj2 = shootingSinglePixel2;
 
  float percent = (float)(_delta % (long)animDuration) / animDuration;
  float percent2 = percent * 2;
  if (percent2 > 1.0) {
    percent2 = 2.0 - percent2;
  }

  int shooting_pc = obj.numPixels();
  int shooting_pc2 = obj2.numPixels();
  int virtualNum = shooting_pc + NUMPIXELS;
  int virtualNum2 = shooting_pc2 + NUMPIXELS;
  
  int off = 0;
  int off2 = 0;
  if (_delta <= animDuration) {
    off = (int)((float)virtualNum * percent) - shooting_pc;
    off2 = (int)((float)virtualNum2 * percent2) - shooting_pc2;
  } else {
    off = (int)((float)virtualNum * (1.0 - percent)) - shooting_pc;
    off2 = (int)((float)virtualNum2 * (1.0 - percent2)) - shooting_pc2;
  }
  
  strip.clear();  
  blit(strip.getPixels(), NUMPIXELS, obj.getPixels(), shooting_pc, off);
  blit(strip.getPixels(), NUMPIXELS, obj2.getPixels(), shooting_pc2, off2);
  strip.show();
}

/**
 * random
 */
void randomBrightness(unsigned long _now, long _delta) {

  if (_delta >= animDuration) {
    animStart = millis();
    animLast = animStart;

    animation_flip = false;
    
    return;
  }

  if (animation_flip) {
    return;
  }

  animation_flip = true;
  
  for(int i=0; i<NUMPIXELS; i++) {
    float r = (float)random(765)/255.;

    if (r <= 1.0) {
      strip.setPixelColor(i, color1_r*r, color1_g*r, color1_b*r);
    } else {
      r = r - 1.0;
      r = r / 2.0;
      strip.setPixelColor(i, color2_r*(r), color2_g*(r), color2_b*(r));
    }
    
    
  }
  strip.show();

}


/**
 * random with random color
 */
void randomWithRandomColor(unsigned long _now, long _delta) {

  if (_delta >= animDuration) {
    animStart = millis();
    animLast = animStart;

    animation_flip = false;
    
    return;
  }

  if (animation_flip) {
    return;
  }

  animation_flip = true;
  
  for(int i=0; i<NUMPIXELS; i++) {
    float r = (float)random(255)/255.;
    strip.setPixelColor(i, random(255)*r, random(255)*r, random(255)*r);
  }
  strip.show();
}
