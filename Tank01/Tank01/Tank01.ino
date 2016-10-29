#include <TimeAlarms.h>
#include <TimeLib.h>
#include <Time.h>
#include <SPI.h>

#define WATER_TANK_NODE
#define BOREWELL_NODE
#define OVERHEAD_TANK_01_NODE

#define MY_RADIO_NRF24
#define MY_REPEATER_FEATURE
#define MY_NODE_ID OVERHEAD_TANK_01_NODE_ID
#define MY_DEBUG

#include <MyNodes.h>
#include <MySensors.h>
#include <MyConfig.h>

#define APPLICATION_NAME "Tank 01"
#define APPLICATION_VERSION "25Sep2016"

#define DEFAULT_LOW_LEVEL 2
AlarmId heartbeatTimer;
AlarmId waterLowLevelRequestTimer;
AlarmId waterLevelRisingTimer;
AlarmId waterLevelFallingTimer;

byte waterLowLevel;
byte waterLowLevelRequestCount;
boolean waterLowLevelReceived;
boolean sendWaterLowLevelRequest;

MyMessage borewellMessage(, V_STATUS);
void before()
{
	for (byte sensorIndex = 0; sensorIndex < MAX_SENSORS; sensorIndex++)
		pinMode(sensorPinArray[sensorIndex], INPUT_PULLUP);

}

void setup()
{
	heartbeatTimer = Alarm.timerRepeat(HEARTBEAT_INTERVAL, sendHeartbeat);
	waterLevelFallingTimer = Alarm.timerRepeat(10 * ONE_MINUTE, sendWaterLevel);
	waterLowLevel = 0;
	waterLowLevelRequestCount = 0;
	waterLowLevelReceived = false;
	sendWaterLowLevelRequest = true;
	borewellMessage.setDestination(BOREWELL_RELAY_NODE_ID);

}

void presentation()
{
	sendSketchInfo(APPLICATION_NAME, APPLICATION_VERSION);
}

void loop()
{
	if (sendWaterLowLevelRequest)
	{
		sendWaterLowLevelRequest = false;
		request(, V_VAR1);
		waterLowLevelRequestTimer = Alarm.timerOnce(REQUEST_INTERVAL, checkWaterLowLevelRequestStatus);
		waterLowLevelRequestCount++;
		if (waterLowLevelRequestCount == 10)
		{
			MyMessage waterLowLevelMessage(, V_VAR1);
			send(waterLowLevelMessage.set(DEFAULT_LOW_LEVEL));

		}
	}
	Alarm.delay(1);
}
void receive(const MyMessage &message)
{
	if (message.type == V_VAR1)
	{
		if (waterLowLevelReceived)
		{
			waterLowLevel = message.getByte() / 20;
			if (waterLowLevel < 1)
				waterLowLevel = 1;
		}
		else
		{
			waterLowLevel = message.getByte() / 20;
			if (waterLowLevel < 1)
				waterLowLevel = 1;
			waterLowLevelReceived = true;
			Alarm.free(waterLowLevelRequestTimer);
			sendWaterLowLevelRequest = false;
			request(, V_VAR1);
		}
	}
}

void sendWaterLevel()
{
	byte decimalValue = 0;
	for (byte sensorIndex = 0; sensorIndex < MAX_SENSORS; sensorIndex++)
	{
		sensorArray[sensorIndex] = digitalRead(sensorPinArray[sensorIndex]);
		if (sensorArray[sensorIndex] == HIGH)
			decimalValue = decimalValue + pow(2, sensorIndex);

	}
	switch (decimalValue)
	{
	case 0:
		break;
	case 1:
		break;
	case 3:
		break;
	case 7:
		break;
	case 15:
		break;
	case 31:
		break;
	case 63:
		break;
	}

	if (sensorArray[waterLowLevel] == HIGH)
	{
		
	
	}

	

}

void checkWaterLowLevelRequestStatus()
{
	if (!waterLowLevelReceived)
		sendWaterLowLevelRequest = true;
}