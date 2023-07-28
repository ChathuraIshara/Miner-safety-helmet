#include <RF24.h>
#include <SPI.h>
#include <RF24Audio.h>
#include "printf.h" // General includes for radio and audio lib
#include <ezButton.h>
#include <nRF24L01.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

ezButton toggleSwitch(5);

RF24 radio(7, 8); // Set radio up using pins 7 (CE) 8 (CS)
RF24Audio rfAudio(radio, 1);

const int spin1 = 9;
const int spin2 = 10;


const unsigned long duration = 5000; // Duration in milliseconds (5 seconds)
unsigned long startTime;
void(* resetFunc) (void) = 0;

const int buttonPin = 2; // Replace with the actual pin number of the button
bool buttonPressed = false;
bool audioInitialized = false;

const int switchrepeaterstate = 5;

LiquidCrystal_I2C lcd(0x27, 16, 4); // Set the I2C address and dimensions of your display


// Define the struct to hold temperature and CO values
struct SensorData {
  float temperature;
  float CO_ppm;
  float other_value;
};

float tempAudio;
float CoAudio;
float CH4Audio;

SensorData sensorData; // Create an instance of the struct

void setup() {

  Serial.begin(9600);
  radio.begin();
  pinMode(LED_BUILTIN, OUTPUT);
  startTime = millis(); // Set the start time
  pinMode(switchrepeaterstate, INPUT_PULLUP);
  toggleSwitch.setDebounceTime(50);
  lcdsetup();

  // digitalWrite(5,HIGH);
}

void lcdsetup() {
  lcd.begin(); // Initialize the LCD
  lcd.backlight(); // Turn on the backlight
  lcd.setCursor(0, 1); // Set the cursor to the beginning of the first line
  lcd.print("_____MINOR______"); // Print a message
  lcd.setCursor(-4, 2); // Set the cursor to the beginning of the first line
  lcd.print("__SAFETY HELMET_");
  delay(1000);
  lcd.clear();
}

void lcdloop(float temp, float meth, double co) {
  // Your code logic here
  lcd.setCursor(0, 0); // Set the cursor to the beginning of the second line
  lcd.print("MINE CONDITION");

  lcd.setCursor(0, 1); // Set the cursor to the beginning of the second line
  lcd.print("Temperature:");
  lcd.print(temp);
  lcd.setCursor(-4, 2); // Set the cursor to the beginning of the second line
  lcd.print("Methane    :");
  lcd.print(meth);
  lcd.setCursor(-4, 3); // Set the cursor to the beginning of the second line
  lcd.print("CO         :");
  lcd.print(co);

  //delay(1000); // Delay for 1 second
}

void lcdLoopAudio()
{

  
  lcd.clear();
  lcd.setCursor(2, 0); // Set the cursor to the beginning of the second line
  lcd.print("Audio->");
  lcd.print(119-(millis()/1000));
  lcd.print(" Sec");
  
  


  lcd.setCursor(0, 1); // Set the cursor to the beginning of the second line
  lcd.print("Temperature:");
  lcd.print(tempAudio);
  lcd.setCursor(-4, 2); // Set the cursor to the beginning of the second line
  lcd.print("Methane    :");
  lcd.print(CH4Audio);
  lcd.setCursor(-4, 3); // Set the cursor to the beginning of the second line
  lcd.print("CO         :");
  lcd.print(CoAudio);
  delay(1000);

}


void receiveMessage()
{
  digitalWrite(6,LOW);
  radio.openReadingPipe(1, 0xF0F0F0F0E1LL); // Address to receive from (6-byte alphanumeric)
  radio.startListening();
  if (radio.available()) {


    radio.read(&sensorData, sizeof(sensorData));
    lcdloop(sensorData.temperature, sensorData.other_value, sensorData.CO_ppm);
    tempAudio=sensorData.temperature;
    CoAudio=sensorData.CO_ppm;
    CH4Audio=sensorData.other_value;
    
    
    

  }



}


void audioTransmission()
{
  if (digitalRead(buttonPin) == HIGH) {
    // Button is pressed, start transmitting audio
    if (!buttonPressed) {
      initializeAudio();
      rfAudio.transmit();
      buttonPressed = true;
    }
  } else {
    // Button is not pressed, start receiving audio
    if (buttonPressed) {
      initializeAudio();
      rfAudio.receive();
      buttonPressed = false;
    }
  }

}


void initializeAudio() {
  if (!audioInitialized) {
    rfAudio.begin();
    rfAudio.setVolume(7);
    audioInitialized = true;
  }
}

void receiverMessageWithRepeater()
{
  radio.openReadingPipe(1, 0xF0F0F0F0D2LL); // Address to receive from (6-byte alphanumeric)
  radio.startListening();
  if (radio.available()) {
    char text[32] = ""; // Assuming the message won't exceed 32 characters
    radio.read(&sensorData, sizeof(sensorData));
    lcdloop(sensorData.temperature, sensorData.other_value, sensorData.CO_ppm);
  }

}

void loop() {
  toggleSwitch.loop();
  int state = toggleSwitch.getState();

  if (state == LOW)
  {
    // Serial.println("Inside repeater");
    receiverMessageWithRepeater();
  }
  else
  {
    if (millis() - startTime < 10000) {
      digitalWrite(spin1, LOW);
      digitalWrite(spin2, LOW);

      digitalWrite(LED_BUILTIN, LOW);
      receiveMessage();

    }
    else if (millis() - startTime < 120000) {
      digitalWrite(LED_BUILTIN, HIGH);
      lcdLoopAudio();
      audioTransmission();
    } else {
      resetFunc();

    }


  }


}
