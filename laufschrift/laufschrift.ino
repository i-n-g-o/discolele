/*
 * Firmware for facile-gold laufschrift for Teensy 3.2
 * written by Ingo Randolf 2018
 * 
 * GNU GPL
 */

#include <SPI.h>        //SPI.h must be included as DMD is written by SPI (the IDE complains otherwise)
#include <DMD.h>        //
#include <TimerOne.h>   //
#include "SystemFont5x7.h"
#include "Arial_Black_16_ISO_8859_1.h"

/*
 * using Teensy 3.2
 */
#define DO_SERIAL


//Fire up the DMD library as dmd
#define DISPLAYS_ACROSS 5
#define DISPLAYS_DOWN 1
DMD dmd(DISPLAYS_ACROSS, DISPLAYS_DOWN);

#include "DMDMessage.h"

/*--------------------------------------------------------------------------------------
  Interrupt handler for Timer1 (TimerOne) driven DMD refresh scanning, this gets
  called at the period set in Timer1.initialize();
--------------------------------------------------------------------------------------*/
void ScanDMD()
{
  dmd.scanDisplayBySPI();  
}


int total_width = DMD_PIXELS_ACROSS * DISPLAYS_ACROSS;
int total_height = DMD_PIXELS_DOWN * DISPLAYS_DOWN;

byte stripState = 0;
long pause_started = 0;
unsigned long animStart = 0;
uint16_t animDuration = 300; // milliseconds


//------------------------------------------------
// texte (1)
//------------------------------------------------
DMDMessage passion_msg("PASSION");
DMDMessage thenight_msg("THE NIGHT");
DMDMessage dance3_msg("DANCE DANCE DANCE");
DMDMessage josephine_msg("JOSEPHINE");
DMDMessage habituelle01_msg("on s'est aime dans les effluves du snack de la piscine. derriere le vestiaire de neptune dans les banlieues by night");
DMDMessage habituelle02_msg("test");
DMDMessage champagne_msg("CHAMPAGNE");
DMDMessage habiteulle_msg("HABITUELLE");

//------------------------------------------------
//
//------------------------------------------------




/*--------------------------------------------------------------------------------------
  setup
  Called by the Arduino architecture before the main loop begins
--------------------------------------------------------------------------------------*/
void setup(void)
{
  pinMode(10, INPUT_PULLUP);  
  pinMode(13, OUTPUT);

#ifdef DO_SERIAL
  Serial.begin(115200);
  while(!Serial) {
    digitalWrite(13, LOW);
    delay(50);
    digitalWrite(13, HIGH);
    delay(50);
  }
  Serial.println("init");
#endif

  delay(500);
  digitalWrite(13, LOW);
  
  //initialize TimerOne's interrupt/CPU usage used to scan and refresh the display
  Timer1.initialize( 3000 );           //period in microseconds to call Scandmd. Anything longer than 5000 (5ms) and you can see flicker.
  Timer1.attachInterrupt( ScanDMD );   //attach the Timer1 interrupt to ScanDMD which goes to dmd.scanDisplayBySPI()

  SPI.begin();
  SPI.setBitOrder(MSBFIRST);  //
  SPI.setDataMode(SPI_MODE0); // CPOL=0, CPHA=0
  SPI.setClockDivider(SPI_CLOCK_DIV2);

  //clear/init the DMD pixels held in RAM  
  dmd.clearScreen( true );   //true is normal (all pixels off), false is negative (all pixels on)      
  dmd.selectFont(Arial_Black_16_ISO_8859_1);

  // setup midi
  // add all handlers
  usbMIDI.setHandleNoteOn(OnNoteOn);
  usbMIDI.setHandleControlChange(OnControlChange);

  //------------------------------------------------
  // ------->>>> (2) calculation
  //------------------------------------------------
  //------------------------------------------------  
  // calculate width  
  passion_msg.calculcate(dmd, total_width);
  thenight_msg.calculcate(dmd, total_width);
  dance3_msg.calculcate(dmd, total_width);
  josephine_msg.calculcate(dmd, total_width);
  champagne_msg.calculcate(dmd, total_width);
  habituelle01_msg.calculcate(dmd, total_width);
  habituelle02_msg.calculcate(dmd, total_width);
  habiteulle_msg.calculcate(dmd, total_width);
}

/*--------------------------------------------------------------------------------------
  loop
  Arduino architecture main loop
--------------------------------------------------------------------------------------*/
void loop(void)
{
  //------------------------------------------------
  // read midi signals
  while (usbMIDI.read()) {
    // ignore incoming messages
  }
  
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

    //------------------------------------------------
    // ------->>>> (3a1) / (3b1)
    // call animations here!
    switch(stripState) {

      case 69:
        off();
        break;

      //--------------------------
      // blinking
      case 35:
        blinkMessage(passion_msg, _now, _delta);
        break;
      case 36:
        blinkMessage(thenight_msg, _now, _delta);        
        break;      
      case 38:
        blinkMessage(josephine_msg, _now, _delta);        
        break;
      case 42:
        blinkMessage(dance3_msg, _now, _delta);        
        break;      
      case 46:
        blinkMessage(habituelle01_msg, _now, _delta);        
        break;
      case 60:
        blinkMessage(champagne_msg, _now, _delta);        
        break;
      case 64:
        blinkMessage(habiteulle_msg, _now, _delta);        
        break;

      //--------------------------
      // rolling
      case 37:
        rolling_text(_now, _delta);
        break;
      case 39:
        rolling_text(_now, _delta);
        break;      
      case 44:
        rolling_text(_now, _delta);
        break;      
      case 62:
        rolling_text(_now, _delta);
        break;        
      case 66:
        rolling_text(_now, _delta);
        break;

        
      case 65:        
        break;
      case 67:        
        break;
         
//      default:
//        stripState = 0;
//        stripOff();
//        break;
    }
  }  
}

//------------------------------------------------
// animation functions
//------------------------------------------------
void off() {
  dmd.clearScreen(true);
}


//------------------------------------------------
// ------->>>> (3a2)
//------------------------------------------------
//------------------------------------------------
// blink animatinos - copy one

void blinkMessage(DMDMessage& msg, unsigned long _now, long _delta) {

  long d = _delta % (long)animDuration;

  if (d < animDuration/2) {
    msg.drawCenter(dmd, GRAPHICS_NORMAL);
  } else {
    dmd.clearScreen(true);
  }
}

//------------------------------------------------
// rolling text - don't touch
//------------------------------------------------
void rolling_text(unsigned long _now, long _delta) {

  if (_delta >= animDuration/30) {
    animStart = millis();
    dmd.stepMarquee(-1,0);
  }
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

    //------------------------------------------------
    // ------->>>> (3b2)
    //------------------------------------------------
    //------------------------------------------------
    // draw scroll text to end of display
    case 39:
      passion_msg.drawMarquee(dmd);
      stripState = note;
      pause_started = 0;
      animStart = millis();
      break;
    case 37:
      josephine_msg.drawMarquee(dmd);
      stripState = note;
      pause_started = 0;
      animStart = millis();
      break;
    case 44:
      habituelle01_msg.drawMarquee(dmd);
      stripState = note;
      pause_started = 0;
      animStart = millis();
      break;
    case 62:
      habituelle02_msg.drawMarquee(dmd);
      stripState = note;
      pause_started = 0;
      animStart = millis();
      break;
    case 66:
      habituelle02_msg.drawMarquee(dmd);
      stripState = note;
      pause_started = 0;
      animStart = millis();
      break;
    //------------------------------------------------
    //------------------------------------------------
    default:
      dmd.clearScreen(true);
      // animations
      stripState = note;
      pause_started = 0;
      animStart = millis();
  }
}


//-------------------------------------------------------------
void OnControlChange(byte channel, byte control, byte value) {

  if (channel != 2) {
    return;
  }

  value = map(value, 0, 127, 0, 255);

  switch (control) {

    // speed
    case 8:
      animDuration = map(value, 0, 255, 2, 2000);
      break;
  }
}

