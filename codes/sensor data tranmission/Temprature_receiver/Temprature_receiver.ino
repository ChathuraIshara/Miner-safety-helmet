#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

// Setup the radio module
RF24 radio(7, 8); // CE, CSN
const uint64_t pipe = 0xE8E8F0F0E1LL; // Pipe address

void setup() {
  Serial.begin(9600);

  // Setup the radio module
  radio.begin();
  radio.openReadingPipe(1, pipe);
  radio.startListening();
}

void loop() {
  if (radio.available()) {
    // Receive temperature value from NRF module
    float temperature;
    radio.read(&temperature, sizeof(temperature));

    // Print temperature value to serial monitor
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println(" degC");
  }
}
