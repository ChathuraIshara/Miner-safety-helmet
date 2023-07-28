/* 
Measuring CH4 gas concentration in ppm using an MQ-4 gas sensor
with load resistance of 20K ohm and sensing resistance of 10K ohm-60K ohm
*/

const int MQ_PIN = A2; // analog pin for MQ-4 sensor output
const int RL_VALUE = 20000; // load resistance in ohms
const float RO_CLEAN_AIR_FACTOR = 9.83; // Ro factor for clean air
const float GAS_FACTOR = 1.16; // gas factor for methane

float Ro = 0; // initial Ro value
float Rs = 0; // resistance of the sensor in the presence of gas
float ppm = 0; // gas concentration in ppm

void setup() {
  Serial.begin(9600); // initialize serial communication
  delay(10000); // allow time for the sensor to heat up
  Ro = MQCalibration(); // calibrate the sensor for clean air
}

void loop() {
  Rs = MQRead(MQ_PIN); // read the sensor resistance
  ppm = MQGetGasPercentage(Rs/Ro, GAS_FACTOR); // calculate the gas concentration in ppm
  Serial.print("CH4 concentration: ");
  Serial.print(ppm);
  Serial.println(" ppm");
  delay(1000); // delay before taking next reading
}

float MQResistanceCalculation(int raw_adc) {
  return ((float)RL_VALUE*(1023-raw_adc)/raw_adc);
}

float MQCalibration() {
  int i;
  float val = 0;
  for (i=0;i<50;i++) { // take 50 samples for calibration
    val += MQResistanceCalculation(analogRead(MQ_PIN));
    delay(50);
  }
  val = val/50; // calculate average value
  return val/RO_CLEAN_AIR_FACTOR; // divide by Ro factor for clean air
}

float MQRead(int mq_pin) {
  int i;
  float rs=0;
  for (i=0;i<10;i++) { // take 10 samples for sensor resistance
    rs += MQResistanceCalculation(analogRead(mq_pin));
    delay(10);
  }
  rs = rs/10; // calculate average value
  return rs;
}

float MQGetGasPercentage(float rs_ro_ratio, float gas_factor) {
  return pow(10, (((log10(rs_ro_ratio)-gas_factor)/(-0.85)))); // calculate gas concentration in ppm
}
