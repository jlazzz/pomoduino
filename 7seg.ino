/* SevSeg Counter Example
 
 Copyright 2020 Dean Reading
 
 This example demonstrates a very simple use of the SevSeg library with a 4
 digit display. It displays a counter that counts up, showing deci-seconds.
 */

#include "SevSeg.h"
SevSeg sevseg; //Instantiate a seven segment controller object


int convertToMMSS(int totalSeconds) {
    int minutes = totalSeconds / 60;
    int seconds = totalSeconds % 60;

    // Convert to MMSS format: e.g., 2 minutes 5 seconds -> 0205
    int mmss = minutes * 100 + seconds;
    return mmss;
}

int checkButtonPress(){
  if (analogRead(A0) > 512){
    return 1;
  }
  return 0;
}
int checkSecondElapsed(unsigned long millis_old){
  if (millis() - millis_old > 1000){
    return 1;
  }
  return 0;
}

int countDownMinutes(int minutes){
  int seconds_remaining = minutes * 60;
  unsigned long millis_old = millis();
  
  while (seconds_remaining > 0){
    sevseg.setNumber(convertToMMSS(seconds_remaining), 2);
    sevseg.refreshDisplay(); // Run every loop iteration
    
    if (checkSecondElapsed(millis_old)){
      millis_old = millis();
      seconds_remaining--;
    }

    if (checkButtonPress()){
      break;
    }
  }
}

int setAllLights(uint8_t state){
  digitalWrite(0, state);
  digitalWrite(1, state);
}
int waitForButtonPress(){
  bool button_state = 0;
  while(button_state == 0){
    setAllLights(HIGH);
    for (int i = 0; i < 250; i++){
      delay(1);
      if (analogRead(A0) > 512){
        button_state = 1;
        break;
      }
    }
    setAllLights(LOW);
    for (int i = 0; i < 250; i++){
      delay(1);
      if (analogRead(A0) > 512){
        button_state = 1;
        break;
      }
    }
  }
}
void delaySevSeg(int seconds){
  unsigned long millis_old = millis();
  int seconds_elapsed = 0;
  while(seconds_elapsed < seconds){
    if (checkSecondElapsed(millis_old)){
      seconds_elapsed++;
    }
    sevseg.refreshDisplay(); // Must run repeatedly
  }
}

void setup() {
  pinMode(0, OUTPUT);
  pinMode(1, OUTPUT);
  byte numDigits = 4;
  byte digitPins[] = {2, 3, 4, 5};
  byte segmentPins[] = {6, 7, 8, 9, 10, 11, 12, 13};
  bool resistorsOnSegments = false; // 'false' means resistors are on digit pins
  byte hardwareConfig = COMMON_ANODE; // See README.md for options
  bool updateWithDelays = false; // Default 'false' is Recommended
  bool leadingZeros = false; // Use 'true' if you'd like to keep the leading zeros
  bool disableDecPoint = false; // Use 'true' if your decimal point doesn't exist or isn't connected
  
  sevseg.begin(hardwareConfig, numDigits, digitPins, segmentPins, resistorsOnSegments,
  updateWithDelays, leadingZeros, disableDecPoint);
  sevseg.setBrightness(90);
}

void loop() {
  countDownMinutes(25);
  //sevseg.setNumber(1, 2);
  delaySevSeg(1);
  //waitForButtonPress();
  //delaySevSeg(1000);
  countDownMinutes(5);
  //sevseg.setNumber(0, 2);
  delaySevSeg(1);
  //waitForButtonPress();
  //delaySevSeg(1000);
}

/// END ///