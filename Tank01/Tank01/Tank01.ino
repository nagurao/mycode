#include <TimeAlarms.h>
#include <TimeLib.h>
#include <Time.h>
#include <SPI.h>

#define WATER_TANK_NODE
#define BOREWELL_NODE
#define OVERHEAD_TANK_01_NODE
#define NODE_INTERACTS_WITH_RELAY
#define NODE_INTERACTS_WITH_LCD

#define MY_RADIO_NRF24
#define MY_REPEATER_FEATURE
#define MY_NODE_ID OVERHEAD_TANK_01_NODE_ID
#define MY_DEBUG

#include <MyNodes.h>
#include <MySensors.h>
#include <MyConfig.h>

#define APPLICATION_NAME "Tank 01"
#define APPLICATION_VERSION "06Nov2016"

AlarmId heartbeatTimer;
AlarmId waterLowLevelRequestTimer;
AlarmId waterLevelRisingTimer;
AlarmId waterLevelFallingTimer;
AlarmId waterDefaultLevelTimer;

int prevWaterLevelValue;
int currWaterLevelValue;
byte waterLowLevelIndex;
byte waterLowLevelInPercent;
byte waterLowLevelRequestCount;
boolean waterLowLevelReceived;
boolean sendWaterLowLevelRequest;
boolean borewellOn;
boolean borewellMotorStatusReceived;

MyMessage waterLevelMessage(CURR_WATER_LEVEL_ID,V_VOLUME);
MyMessage waterLowLevelMessage(WATER_LOW_LEVEL_IND_ID, V_VAR1);
MyMessage lcdWaterLevelMessage(CURR_WATER_LEVEL_ID, V_VOLUME);

MyMessage borewellOnMessage(BORE_ON_RELAY_ID, V_STATUS);
MyMessage borewellOffMessage(BORE_OFF_RELAY_ID, V_STATUS);
MyMessage thingspeakMessage(WIFI_NODEMCU_ID, V_CUSTOM);

void before()
{
	for (byte sensorIndex = 0; sensorIndex < MAX_SENSORS; sensorIndex++)
		pinMode(sensorPinArray[sensorIndex], INPUT_PULLUP);
}

void setup()
{
	prevWaterLevelValue = 200;
	currWaterLevelValue = 0;
	waterLowLevelIndex = 0;
	waterLowLevelInPercent = 0;
	waterLowLevelRequestCount = 0;
	waterLowLevelReceived = false;
	sendWaterLowLevelRequest = true;
	borewellOn = false;
	borewellMotorStatusReceived = false;
	lcdWaterLevelMessage.setDestination(LCD_NODE_ID);
	lcdWaterLevelMessage.setSensor(CURR_WATER_LEVEL_ID);
	thingspeakMessage.setDestination(THINGSPEAK_NODE_ID);
	thingspeakMessage.setType(V_CUSTOM);
	thingspeakMessage.setSensor(WIFI_NODEMCU_ID);

	heartbeatTimer = Alarm.timerRepeat(HEARTBEAT_INTERVAL, sendHeartbeat);
	waterDefaultLevelTimer = Alarm.timerRepeat(DEFAULT_LEVEL_POLL_DURATION, getWaterLevel);
	waterLevelFallingTimer = Alarm.timerRepeat(FALLING_LEVEL_POLL_DURATION, getWaterLevel);
	waterLevelRisingTimer = Alarm.timerRepeat(RISING_LEVEL_POLL_DURATION, getWaterLevel);
}

void presentation()
{
	sendSketchInfo(APPLICATION_NAME, APPLICATION_VERSION);
	present(CURR_WATER_LEVEL_ID, S_WATER, "Tank 01 Water Level");
	wait(WAIT_50MS);
	present(WATER_LOW_LEVEL_IND_ID, S_CUSTOM, "Low Water Level %");
}

void loop()
{
	if (sendWaterLowLevelRequest)
	{
		sendWaterLowLevelRequest = false;
		request(WATER_LOW_LEVEL_IND_ID, V_VAR1);
		waterLowLevelRequestTimer = Alarm.timerOnce(REQUEST_INTERVAL, checkWaterLowLevelRequestStatus);
		waterLowLevelRequestCount++;
		if (waterLowLevelRequestCount == 10)
		{
			MyMessage waterLowLevelMessage(WATER_LOW_LEVEL_IND_ID, V_VAR1);
			send(waterLowLevelMessage.set(DEFAULT_LOW_LEVEL));
		}
	}
	Alarm.delay(1);
}
void receive(const MyMessage &message)
{
	switch (message.type)
	{
	case V_VAR1:	
		waterLowLevelInPercent = message.getByte();
		waterLowLevelIndex = (waterLowLevelInPercent / 20) ? MAX_SENSORS - (waterLowLevelInPercent / 20) - 1 : (MAX_SENSORS - 1);
		send(waterLowLevelMessage.set(message.getByte()));
		if (!waterLowLevelReceived)
		{
			waterLowLevelReceived = true;
			Alarm.free(waterLowLevelRequestTimer);
			sendWaterLowLevelRequest = false;
			request(WATER_LOW_LEVEL_IND_ID, V_VAR1);
		}
		break;
	case V_STATUS:
		borewellOn = (message.getInt()) ? RELAY_ON : RELAY_OFF;
		borewellMotorStatusReceived = true;
		break;
	}
}

void getWaterLevel()
{
	currWaterLevelValue = 0;
	for (byte sensorIndex = 0; sensorIndex < MAX_SENSORS; sensorIndex++)
	{
		sensorArray[sensorIndex] = 0;
		sensorArray[sensorIndex] = digitalRead(sensorPinArray[sensorIndex]);
		byte power = binToDecArray[sensorIndex] * sensorArray[sensorIndex];
		currWaterLevelValue = currWaterLevelValue + power;
		Alarm.delay(WAIT_5MS);
	}

	switch (currWaterLevelValue)
	{
	case 0:
		borewellOffMessage.setDestination(BOREWELL_RELAY_NODE_ID);
		send(borewellOffMessage.set(RELAY_ON));
		sendWaterLevel(LEVEL_110);
		Alarm.disable(waterDefaultLevelTimer);
		Alarm.disable(waterLevelRisingTimer);
		Alarm.enable(waterLevelFallingTimer);
		break;
	case 1:
		sendWaterLevel(LEVEL_100);
		break;
	case 3:
		sendWaterLevel(LEVEL_80);
		break;
	case 7:
		sendWaterLevel(LEVEL_60);
		break;
	case 15:
		sendWaterLevel(LEVEL_40);
		break;
	case 31:
		sendWaterLevel(LEVEL_20);
		break;
	case 63:
		sendWaterLevel(LEVEL_0);
		break;
	}

	if (sensorArray[waterLowLevelIndex] == HIGH)
	{
		if (!borewellOn && borewellMotorStatusReceived)
		{
			MyMessage borewellOnMessage(BORE_ON_RELAY_ID, V_STATUS);
			borewellOnMessage.setDestination(BOREWELL_RELAY_NODE_ID);
			send(borewellOnMessage.set(RELAY_ON));
			Alarm.disable(waterDefaultLevelTimer);
			Alarm.disable(waterLevelFallingTimer);
			Alarm.enable(waterLevelRisingTimer);
		}
		else
		{
			request(BOREWELL_MOTOR_ID, V_STATUS, BOREWELL_RELAY_NODE_ID);
			borewellMotorStatusReceived = false;
		}
	}

	prevWaterLevelValue = currWaterLevelValue;
		
}

void sendWaterLevel(int waterLevel)
{
	if (waterLevel != prevWaterLevelValue)
	{
		send(waterLevelMessage.set(waterLevel));
		Alarm.delay(WAIT_5MS);
		send(lcdWaterLevelMessage.set(waterLevel));
		Alarm.delay(WAIT_5MS);
		send(thingspeakMessage.set(waterLevel));
	}
}
void checkWaterLowLevelRequestStatus()
{
	if (!waterLowLevelReceived)
		sendWaterLowLevelRequest = true;
}