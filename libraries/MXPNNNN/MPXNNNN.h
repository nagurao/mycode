#ifndef MPXNNNN_h
#define MPXNNNN_h

#include <Arduino.h>

#ifndef A2D_FULL_SCALE
#define A2D_FULL_SCALE 1024
#endif

#ifndef VSS
#define VSS 5.0
#endif

#ifndef SCALE_HEIGHT
#define SCALE_HEIGHT 10.1972
#endif

// For other MPXnnnn sensors just add
// new #define statements

#define MPX5050 0

class MPXNNNN
{
public:
	MPXNNNN(byte devType, uint8_t pin, uint8_t refPin);
	MPXNNNN(byte devType, uint8_t pin, uint8_t refPin, float deviceScaleFactor);
	float readMPXNNNNInKPA();
	float readMPXNNNNInHeight();
	float readMPXNNNNAvgInKPA(byte sampleCount, int msDelay);
	float readMPXNNNNAvgInHeight(byte sampleCount, int msDelay);
	void setErrorCount(byte errorCount);
	
private:
	byte _devType;
	uint8_t _pin;
	uint8_t _refPin;
	byte _errorCount;
	float _deviceScaleFactor;
	float _analogRefVolts;
	float converttoKiloPascal(float reading);
	float convertToWaterHeightInCMS(float pressure);
	float supplyVoltage();
};

#endif