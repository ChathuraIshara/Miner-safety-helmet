#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 4); // Set the I2C address and dimensions of your display

// Setup the radio module
RF24 radio(7, 8); // CE, CSN
const uint64_t pipe = 0xE8E8F0F0E1LL; // Pipe address

// Define the struct to hold temperature and CO values
struct SensorData {
  float temperature;
  float CO_ppm;
  float other_value;
};






SensorData sensorData; // Create an instance of the struct

void setup() {
  Serial.begin(9600);

  // Setup the radio module
  radio.begin();
  radio.openReadingPipe(1, pipe);
  radio.startListening();

  lcd.begin(); // Initialize the LCD
  lcd.backlight(); // Turn on the backlight
  lcd.setCursor(0, 1); // Set the cursor to the beginning of the first line
  lcd.print("______MINOR_____"); // Print a message
  lcd.setCursor(-4, 2); // Set the cursor to the beginning of the first line
  lcd.print("_SAFETY HELMET__");
  delay(3000);
  lcd.clear();
}

void loop() {
  if (radio.available()) {
    radio.read(&sensorData, sizeof(sensorData));


      // Your code logic here
  float temperature,mq4,mq7;
  temperature=sensorData.temperature;
  mq7=sensorData.CO_ppm;
  mq4=sensorData.other_value;
  lcd.setCursor(0, 0); // Set the cursor to the beginning of the second line
  lcd.print("_MINE CONDITION_");
  
  lcd.setCursor(0, 1); // Set the cursor to the beginning of the second line
  lcd.print("Temperature:");
  lcd.print(temperature);
  lcd.setCursor(-4, 2); // Set the cursor to the beginning of the second line
  lcd.print("Methane PPM:");
  lcd.print(mq4);
  lcd.setCursor(-4, 3); // Set the cursor to the beginning of the second line
  lcd.print("CO         :");
  lcd.print(mq7);
  
  delay(1000); // Delay for 1 second
    
  }
}
