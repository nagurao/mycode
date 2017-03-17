#include "MPXNNNN.h"

MPXNNNN::MPXNNNN(byte devType, uint8_t pin, uint8_t refPin)
{
	_devType = devType;
	_pin = pin;
	_refPin = refPin;
	pinMode(refPin, INPUT);
	pinMode(pin, INPUT);
	switch (devType)
	{
	case MPX5050: _deviceScaleFactor = 0.018;
		break;
	}
	analogReference(DEFAULT);
	_errorCount = 0;
}

MPXNNNN::MPXNNNN(byte devType, uint8_t pin, uint8_t refPin, float deviceScaleFactor)
{
	
	_devType = devType;
	_pin = pin;
	_refPin = refPin;
	_deviceScaleFactor = deviceScaleFactor;
	pinMode(refPin, INPUT);
	pinMode(pin, INPUT);
	analogReference(DEFAULT);
	_errorCount = 0;
}

float MPXNNNN::readMPXNNNNInKPA()
{
	return converttoKiloPascal(analogRead(_pin));
}

float MPXNNNN::readMPXNNNNInHeight()
{
	return convertToWaterHeightInCMS(converttoKiloPascal(analogRead(_pin)));
}

float MPXNNNN::readMPXNNNNAvgInKPA(byte sampleCount, int msDelay)
{
	float reading;
	float average;
	for (byte i = 0; i < sampleCount; i++)
	{
		reading += analogRead(_pin);
		delay(msDelay);
	}
	average = reading / sampleCount;
	return(converttoKiloPascal(average));
}

float MPXNNNN::readMPXNNNNAvgInHeight(byte sampleCount, int msDelay)
{
	float kPa = readMPXNNNNAvgInKPA(sampleCount, msDelay);
	return convertToWaterHeightInCMS(kPa);
}

float MPXNNNN::converttoKiloPascal(float reading)
{
	_analogRefVolts = supplyVoltage();
	float vOut = ((reading - _errorCount) * _analogRefVolts) / A2D_FULL_SCALE;
	if (vOut < 0.01)
		vOut = 0.00;
	float kValue = (0.04 * _analogRefVolts);
	float diffVolts = vOut - kValue;
	if (diffVolts < 0.00)
		diffVolts = vOut;
	return (diffVolts / (_analogRefVolts * _deviceScaleFactor));
}

float MPXNNNN::convertToWaterHeightInCMS(float pressure)
{
	return(pressure * SCALE_HEIGHT);
}

float MPXNNNN::supplyVoltage()
{
	return (analogRead(_refPin) * VSS / A2D_FULL_SCALE);
}

void MPXNNNN::setErrorCount(byte errorCount)
{
	_errorCount = errorCount;
}
