#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

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
}

void loop() {
  if (radio.available()) {
    radio.read(&sensorData, sizeof(sensorData));

    // Print the received data
    Serial.print("Temperature: ");
    Serial.print(sensorData.temperature);
    Serial.print(" degC\tCO: ");
    Serial.print(sensorData.CO_ppm);
    Serial.println(" ppm");
    Serial.print(sensorData.other_value );
    Serial.println(" demo");
  }
}
