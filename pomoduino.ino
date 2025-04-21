#include "SevSeg.h"

SevSeg sevseg;
bool ledState = false;
int buttonThreshold = 512;

unsigned long ledPreviousMillis = 0;
unsigned long ledBlinkInterval = 1000;
unsigned long patternPreviousMillis = 0;
unsigned long patternInterval = 55;
unsigned long lastButtonPressTime = 0;
unsigned long debounceDelay = 50;

int sequenceStep = 0;

enum LedColor {
  GREEN, 
  BLUE,
  OFF
};


int convertToMMSS(int totalSeconds) {
    int minutes = totalSeconds / 60;
    int seconds = totalSeconds % 60;

    // Convert to MMSS format: e.g., 2 minutes 5 seconds -> 0205
    int mmss = minutes * 100 + seconds;
    return mmss;
}

int checkButtonPress(){
  if (analogRead(A0) > 512) {  
    if (millis() - lastButtonPressTime > debounceDelay) {
      lastButtonPressTime = millis();
      byte blankDigits[] = {0, 0, 0, 0};
      sevseg.setSegments(blankDigits);
      return 1;
    }
  }
  return 0;
}

int checkSecondElapsed(unsigned long millis_old){
  if (millis() - millis_old > 1000){
    return 1;
  }
  return 0;
}

int countdownMinutes(int minutes){
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

int checkMillisElapsed(unsigned long millis_old, int ms) {
  if (millis() - millis_old >= ms) {
    return 1;
  }
  return 0;
}

void iterateWaitingPattern() {
    byte steps[] = {
    0b00000011, // a + b
    0b01000011, // a + b + g
    0b01000010, // b + g
    0b01010010, // b + g + e
    0b01010000, // g + e
    0b01011000, // g + e + d
    0b00011000, // e + d
    0b00011100, // e + d + c
    0b00001100, // d + c
    0b01001100, // d + c + g
    0b01000100, // c + g
    0b01100100, // c + g + f
    0b01100000, // g + f
    0b01100001, // g + f + a
    0b00100001  // f + a
  };
  int totalSteps = sizeof(steps) / sizeof(steps[0]);
  byte segmentPattern = steps[sequenceStep];

  byte segments[4] = {
    segmentPattern,
    segmentPattern,
    segmentPattern,
    segmentPattern
  };

  sevseg.setSegments(segments);
  sevseg.refreshDisplay();

  sequenceStep = (sequenceStep + 1) % totalSteps;
}

void waitForButtonPress() {
  bool buttonPressed = false;

  while (!buttonPressed) {
    unsigned long currentMillis = millis();
    sevseg.refreshDisplay();

    if (checkButtonPress()) {
      if (currentMillis - lastButtonPressTime > debounceDelay) {
        buttonPressed = true;
        lastButtonPressTime = currentMillis;
      }
    }

    if (currentMillis - ledPreviousMillis >= ledBlinkInterval) {
      ledPreviousMillis = currentMillis;
      ledState = !ledState;
      setAllLights(ledState ? HIGH : LOW);
    }

    if (currentMillis - patternPreviousMillis >= patternInterval) {
      patternPreviousMillis = currentMillis;
      iterateWaitingPattern();
    }
  }
}

void delaySevSeg(int milliseconds) {
  unsigned long millis_old = millis();
  int ms_elapsed = 0;
  while (ms_elapsed < milliseconds) {
    if (checkMillisElapsed(millis_old, 1)) {
      ms_elapsed++;
      millis_old = millis();
    }
    sevseg.refreshDisplay();
  }
}

void setup() {
  pinMode(0, OUTPUT);
  pinMode(1, OUTPUT);
  byte numDigits = 4;
  byte digitPins[] = {2, 3, 4, 5};
  byte segmentPins[] = {6, 7, 8, 9, 10, 11, 12, 13};
  bool resistorsOnSegments = false;
  byte hardwareConfig = COMMON_ANODE;
  bool updateWithDelays = false;
  bool leadingZeros = false;
  bool disableDecPoint = false;
  
  sevseg.begin(hardwareConfig, numDigits, digitPins, segmentPins, resistorsOnSegments,
  updateWithDelays, leadingZeros, disableDecPoint);
  sevseg.setBrightness(50);
}

void loop() {
  countdownMinutes(25);
  delaySevSeg(250);
  waitForButtonPress();
  delaySevSeg(250);
  countdownMinutes(5);
  delaySevSeg(250);
  waitForButtonPress();
  delaySevSeg(250);
}
