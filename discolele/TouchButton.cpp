#include "TouchButton.h"

TouchButton::TouchButton(int _btn) :
  btnPin(_btn),
  thresh(1000)
{}

TouchButton::TouchButton(int _btn, int _thresh) :
  btnPin(_btn),
  thresh(_thresh)
{}

bool TouchButton::update() {

  bool result = false;
  int val = touchRead(btnPin);

  if (val > thresh && !lastState) {
    lastState = true;
    result = true;
  } else if (val < thresh && lastState) {
    lastState = false;
    result = true;
  }

  return result;
}

int TouchButton::getValue() {
  return touchRead(btnPin);
}

