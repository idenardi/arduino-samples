#include <Keyboard.h>

const int buttonPin = 4;          // input pin for pushbutton
int previousButtonState = HIGH;   // for checking the state of a pushButton

void setup() {
  // make the pushButton pin an input:
  pinMode(buttonPin, INPUT);
  // initialize control over the keyboard:
  Keyboard.begin();
}

void loop() {
  // read the pushbutton:
  int buttonState = digitalRead(buttonPin);
  // if the button state has changed,
  if ((buttonState != previousButtonState) && (buttonState == HIGH)) {
    // username
    Keyboard.print("username");
    // tab
    Keyboard.write(9);
    // password
    Keyboard.print("password");
    // return
    Keyboard.write(176);
    delay(2000);
  }
  
  // save the current button state for comparison next time:
  previousButtonState = buttonState;
}
