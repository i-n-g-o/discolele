#ifndef TOUCHBUTTON_H
#define TOUCHBUTTON_H

#include <WProgram.h>

class TouchButton {

public:
  TouchButton(int _btn);
  TouchButton(int _btn, int _thresh);

  bool update();

  void setThreashold(int _thresh) { thresh = _thresh; };
  int getThreashold() { return thresh; };

  bool getState() { return lastState; };
  
  int getValue();

private:
  int btnPin;
  bool lastState;

  int thresh;
  
};


#endif
