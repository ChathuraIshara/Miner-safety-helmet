#include <RF24.h>
#include <SPI.h>
#include <RF24Audio.h>
#include <ezButton.h>
#include "printf.h" // General includes for radio and audio lib
/////////////////////////////
#include <nRF24L01.h>
#include <DHT.h>
/////////////////////////////

ezButton toggleSwitch(5);

RF24 radio(7, 8); // Set radio up using pins 7 (CE) 8 (CS)
RF24Audio rfAudio(radio, 1);

const unsigned long duration = 5000; // Duration in milliseconds (5 seconds)
unsigned long startTime;
void (*resetFunc)(void) = 0;

const int buttonPin = 2; // Replace with the actual pin number of the button
bool buttonPressed = false;
bool audioInitialized = false;

const int switchrepeater = 5;


///////////////////////////////////////////



// Setup the DHT22 sensor
#define DHTPIN 4 // Digital pin connected to the DHT sensor
#define DHTTYPE DHT22 // DHT22 (AM2302)
DHT dht(DHTPIN, DHTTYPE);

// Setup the MQ7 CO sensor
const int mq7Pin = A1;
float m = -0.6527;       // Slope
float b = 1.30;          // Y-Intercept
float R0Co = 21.91;        // Sensor Resistance in fresh air from previous code


///////////////////////////setup the ch4 senor
const int MQ4Sensor = A2;
float R0 = 10.0;

/////////////////////////////////////

// Struct to hold the temperature and CO values
struct SensorData {
  float temperature;
  float co_ppm;
  float other_value;
};




///////////////////////////////////////////






void setup() {
  // Initialization code
  Serial.begin(9600);
  radio.begin();
  pinMode(LED_BUILTIN, OUTPUT);
  startTime = millis(); // Set the start time
  radio.openWritingPipe(0xF0F0F0F0E1LL); // Address to send to (6-byte alphanumeric)
  pinMode(switchrepeater, INPUT_PULLUP);
  toggleSwitch.setDebounceTime(50);

  // Setup the DHT22 sensor
  dht.begin();
  pinMode(MQ4Sensor, INPUT);
  pinMode(mq7Pin, INPUT);

}



void sendMessage() {

  float methaneppm = mq4Value(); // calculate the gas concentration in ppm
  /////////////////////////



  // Read the temperature data from the DHT22 sensor
  float temperature = dht.readTemperature();

  // Read the CO data from the MQ7 CO sensor
  float co_ppm =readCOConcentration();

  // Create a struct to hold the temperature and CO values
  SensorData data;
  data.temperature = temperature;
  data.co_ppm = co_ppm;
  data.other_value = methaneppm;

  //   Send the temperature and CO data over the radio module
  radio.write(&data, sizeof(data));

}

//MQ7function

float readCOConcentration() {
  float sensor_volt;    // Define a variable for sensor voltage
  float RS_gas;         // Define a variable for sensor resistance
  float ratio;          // Define variable for ratio
  int sensorValue = analogRead(mq7Pin); // Read analog values of the sensor

  sensor_volt = sensorValue * (5.0 / 1023.0); // Convert analog values to voltage

  RS_gas = ((5.0 * 10.0) / sensor_volt) - 10.0; // Get value of RS in a gas


  ratio = RS_gas / R0Co; // Get ratio RS_gas/RS_air


  float ppm_log = (log10(ratio) - b) / m; // Get ppm value in linear scale according to the ratio
  float ppm = pow(10, ppm_log); // Convert ppm value to log scale

  return ppm;
}

//mq4 function
float mq4Value()
{
  float sensor_volt;
  float RS_gas;  // Get value of RS in a GAS
  float ratio;   // Get ratio RS_gas/RS_air

  sensor_volt = analogRead(MQ4Sensor);
  sensor_volt = sensor_volt / 1024 * 5.0;

  RS_gas = (5.0 - sensor_volt) / sensor_volt; // Calculate RS in GAS
  ratio = RS_gas / R0;                     // ratio = RS/R0

  float ppm = pow(ratio, -1.85);
  return ppm;

}







////////////////////////////////
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

void loop() {
  toggleSwitch.loop();
  int state = toggleSwitch.getState();
  if (state == LOW)
  {

    sendMessage();


  }
  else
  {
    if (millis() - startTime < 10000) {
      digitalWrite(LED_BUILTIN, LOW);
      sendMessage();
    } else if (millis() - startTime < 120000) {
      digitalWrite(LED_BUILTIN, HIGH);
      audioTransmission();
    } else {
      resetFunc();
    }

  }



}
