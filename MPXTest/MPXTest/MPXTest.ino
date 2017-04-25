#include <MPXNNNN.h>
#define MPX5050 0
#define MPX5050_CONSTANT 0.018
#define PRESSURE_SENSOR_PIN A0
#define REFERENCE_VOLTAGE_PIN A1

MPXNNNN pressureSensor(MPX5050, PRESSURE_SENSOR_PIN, REFERENCE_VOLTAGE_PIN, MPX5050_CONSTANT);

void setup() {
	Serial.begin(115200);
	pressureSensor.setErrorCount(25);
	delay(5000);
}

// the loop function runs over and over again until power down or reset
void loop() {
	Serial.print("The pressure in kPa is : ");
	Serial.println(pressureSensor.readMPXNNNNInKPA(),2);
	Serial.print("The water column height (cms) is : ");
	Serial.println(pressureSensor.readMPXNNNNInHeight(),2);
	delay(1000);
}
