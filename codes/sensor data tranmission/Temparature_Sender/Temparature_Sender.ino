#include <RF24.h>
#include <SPI.h>
#include <RF24Audio.h>
#include "printf.h" // General includes for radio and audio lib

RF24 radio(7,8); // Set radio up using pins 7 (CE) 8 (CS)
RF24Audio rfAudio(radio, 1);

const int buttonPin = 2; // Replace with the actual pin number of the button
bool buttonPressed = false;

void setup() { 
  Serial.begin(115200);
  
  printf_begin();
  radio.begin();
  radio.printDetails();
  
  rfAudio.begin();
  rfAudio.setVolume(7);
}

void loop() {
  // Check if the button is pressed
  if (digitalRead(buttonPin) == HIGH) {
    // Button is pressed, start transmitting audio
    if (!buttonPressed) {
      rfAudio.transmit();
      buttonPressed = true;
    }
  } else {
    // Button is not pressed, start receiving audio
    if (buttonPressed) {
      rfAudio.receive();
      buttonPressed = false;
    }
  }
}
