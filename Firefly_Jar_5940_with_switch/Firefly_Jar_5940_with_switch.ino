/*
 * Firefly Jar by KeirFox
 */


/*
 * Added button to pin 0 to change modes -- writes to EEPROM to preserve setting during power loss.
 */
 

#include "Tlc5940.h"
#include <EEPROM.h>
#define NO_OF_PINS  16
#define buttonPin   0
#define EEPROMlocation 1


int mode;
int maxMode = 3;

void setup() {
  Tlc.init(0); // Initiates the TLC5940 and set all channels off
  pinMode(buttonPin, INPUT);  
}
 
void loop() {


  mode = EEPROM.read(EEPROMlocation);
  
  if ((mode < 0) || (mode > maxMode))  {
    EEPROM.write(EEPROMlocation, 0);
    mode = 0;
  }

  int buttonState;
  
  int MAX_LED_BRIGHTNESS = 4095;    // 4095
  int MAX_FADERS = 16;              // 16
  int MIN_IDLE_BRIGHTNESS = 50;      // 0
  int MAX_IDLE_BRIGHTNESS = 300;    // 300
  int brightnessUpDelta = 300;      // 300
  int brightnessDownDelta = 100;    // 100
  int activeFaders = 0;             // 0
  int wiggleLethargy = 50;          // 50
  int fireflySleepinessLevel = 20;  // 15
  bool flare = true;
  
  int outputPins[NO_OF_PINS] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
  int brightness[NO_OF_PINS] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
  int fadeState[NO_OF_PINS] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};  // 0 = idle, 1 = fade down, 2 = fade up, 3 = idle down, 4 = idle up


  // Set initial brightness to random
  for(int i = 0; i < NO_OF_PINS; i++) {
    brightness[i] = random(MIN_IDLE_BRIGHTNESS, MAX_IDLE_BRIGHTNESS);
  }


  while (true) {


    buttonState = digitalRead(buttonPin);
  
    // Increment our mode
    if (buttonState == LOW) // button pressed
      {
        
        if (mode == maxMode) {mode = 0;}
        else {mode++;}

        EEPROM.write(EEPROMlocation, mode);

        delay(500);
        
        for(int i = 0; i < NO_OF_PINS; i++) { 
        brightness[i] = 1;
    }
      }

    switch (mode) { // These are all the lighting profiles you can switch to. 
      case 0: // Full brightness with firefly "flares"
          MAX_LED_BRIGHTNESS = 4095;    // 4095
          MAX_FADERS = 16;              // 16
          MIN_IDLE_BRIGHTNESS = 50;      // 0
          MAX_IDLE_BRIGHTNESS = 300;    // 300
          brightnessUpDelta = 300;      // 300
          brightnessDownDelta = 100;    // 100
          activeFaders = 0;             // 0
          wiggleLethargy = 50;          // 50
          fireflySleepinessLevel = 20;  // 15
          flare = true;
          break;
      case 1: // Around half brightness with firefly "flares"
          MAX_LED_BRIGHTNESS = 300;    // 4095
          MAX_FADERS = 16;              // 16
          MIN_IDLE_BRIGHTNESS = 50;      // 0
          MAX_IDLE_BRIGHTNESS = 300;    // 300
          brightnessUpDelta = 30;      // 300
          brightnessDownDelta = 10;    // 100
          activeFaders = 0;             // 0
          wiggleLethargy = 100;          // 50
          fireflySleepinessLevel = 50;  // 15
          flare = true;
          break;
      case 2: // Full brightness with light "warble", no flares.
          MAX_LED_BRIGHTNESS = 4095;    // 4095
          MAX_FADERS = 16;              // 16
          MIN_IDLE_BRIGHTNESS = 50;      // 0
          MAX_IDLE_BRIGHTNESS = 700;    // 300
          brightnessUpDelta = 300;      // 300
          brightnessDownDelta = 100;    // 100
          activeFaders = 0;             // 0
          wiggleLethargy = 50;          // 50
          fireflySleepinessLevel = 20;  // does not apply here as 'flare' is false
          flare = false;
          break;
      case 3: // Half brightness with light "warble", no flares.
          MAX_LED_BRIGHTNESS = 300;    // 4095
          MAX_FADERS = 16;              // 16
          MIN_IDLE_BRIGHTNESS = 50;      // 0
          MAX_IDLE_BRIGHTNESS = 300;    // 300
          brightnessUpDelta = 30;      // 300
          brightnessDownDelta = 10;    // 100
          activeFaders = 0;             // 0
          wiggleLethargy = 100;          // 50
          fireflySleepinessLevel = 100;  // does not apply here as 'flare' is false
          flare = false;
          break;
    }



    
  
     // Random selection
     if (activeFaders < MAX_FADERS) {
        int selectAPin = random(0,fireflySleepinessLevel);
        if (selectAPin == 1) {
          int randomPin = random(0, NO_OF_PINS);
          if (fadeState[randomPin] == 0 || fadeState[randomPin] == 4) {
            fadeState[randomPin] = 2;
            activeFaders = activeFaders + 1;
          }
        }
     }
      
    for(int i = 0; i < NO_OF_PINS; i++) {  
      
      switch (fadeState[i]) {
        
        case 0:   // Idle
          if (random(0,wiggleLethargy) == 1) {
            fadeState[i] = 4;
          }
          break;
        
        case 1:   // Fade down
          if (flare) {
            if (brightness[i] <= MIN_IDLE_BRIGHTNESS) { // At the bottom!
              fadeState[i] = 0;
              activeFaders = activeFaders - 1;
            }
            else {
              brightness[i] = brightness[i] - brightnessDownDelta;
              if (brightness[i] < MIN_IDLE_BRIGHTNESS) {
                brightness[i] = MIN_IDLE_BRIGHTNESS;
              }
            }
          }
          break;
        
        case 2:   // Fade up
          if (flare) {
            if (brightness[i] >= MAX_LED_BRIGHTNESS) { // Maxed out!
              fadeState[i] = 1;
            }
            else {
              brightness[i] = brightness[i] + brightnessUpDelta;
              if (brightness[i] > MAX_LED_BRIGHTNESS) {
                brightness[i] = MAX_LED_BRIGHTNESS;
              }
            }
          }
          break;
        
        case 3:   // Idle down
          if (brightness[i] <= MIN_IDLE_BRIGHTNESS) {
            fadeState[i] = 0;
          }
          else {
            brightness[i] = brightness[i] - 1;
          }
          break;
        
        case 4:   // Idle up
          if (brightness[i] >= MAX_IDLE_BRIGHTNESS) {
            fadeState[i] = 3;
          }
          else {
            brightness[i] = brightness[i] + 1;
          }
          break;
      }

      Tlc.set(outputPins[i], brightness[i]);
      Tlc.update();
      delay(1);
    }
  }
}
