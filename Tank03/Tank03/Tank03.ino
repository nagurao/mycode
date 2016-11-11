#include <TimeAlarms.h>
#include <TimeLib.h>
#include <Time.h>
#include <SPI.h>

#define WATER_TANK_NODE
#define UNDERGROUND_TANK_NODE
#define NODE_INTERACTS_WITH_RELAY
#define NODE_INTERACTS_WITH_LCD

#define MY_RADIO_NRF24
#define MY_REPEATER_FEATURE
#define MY_NODE_ID UNDERGROUND_NODE_ID
#define MY_DEBUG

#include <MyNodes.h>
#include <MySensors.h>
#include <MyConfig.h>

#define APPLICATION_NAME "Tank 03"
#define APPLICATION_VERSION "12Nov2016"

AlarmId heartbeatTimer;
AlarmId waterLowLevelRequestTimer;
AlarmId waterLevelRisingTimer;
AlarmId waterLevelFallingTimer;
AlarmId waterDefaultLevelTimer;

int prevWaterLevelValue;
int currWaterLevelValue;
int currWaterLevelValueDec;

byte waterOverFlowLevelIndex;
byte waterLowLevelIndex;
byte waterLowLevelInPercent;
byte waterLowLevelRequestCount;
boolean waterLowLevelReceived;
boolean sendWaterLowLevelRequest;

MyMessage waterLevelMessage(CURR_WATER_LEVEL_ID, V_VOLUME);
MyMessage waterLowLevelMessage(WATER_LOW_LEVEL_IND_ID, V_VAR1);
MyMessage lcdWaterLevelMessage(CURR_WATER_LEVEL_ID, V_VOLUME);

MyMessage thingspeakMessage(WIFI_NODEMCU_ID, V_CUSTOM);

void before()
{
	for (byte sensorIndex = 0; sensorIndex < MAX_SENSORS; sensorIndex++)
		pinMode(sensorPinArray[sensorIndex], INPUT_PULLUP);
}

void setup()
{
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
	present(CURR_WATER_LEVEL_ID, S_WATER, "Tank 03 Water Level");
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
	}
}

void getWaterLevel()
{
	currWaterLevelValueDec = 0;
	for (byte sensorIndex = 0; sensorIndex < MAX_SENSORS; sensorIndex++)
	{
		sensorArray[sensorIndex] = 0;
		sensorArray[sensorIndex] = digitalRead(sensorPinArray[sensorIndex]);
		byte power = binToDecArray[sensorIndex] * sensorArray[sensorIndex];
		currWaterLevelValueDec = currWaterLevelValueDec + power;
		Alarm.delay(WAIT_5MS);
	}

	switch (currWaterLevelValueDec)
	{
	case 0:
		sendWaterLevel(LEVEL_110);
		currWaterLevelValue = LEVEL_110;
		break;
	case 1:
		sendWaterLevel(LEVEL_100);
		currWaterLevelValue = LEVEL_100;
		break;
	case 3:
		sendWaterLevel(LEVEL_80);
		currWaterLevelValue = LEVEL_80;
		break;
	case 7:
		sendWaterLevel(LEVEL_60);
		currWaterLevelValue = LEVEL_60;
		break;
	case 15:
		sendWaterLevel(LEVEL_40);
		currWaterLevelValue = LEVEL_40;
		break;
	case 31:
		sendWaterLevel(LEVEL_20);
		currWaterLevelValue = LEVEL_20;
		break;
	case 63:
		sendWaterLevel(LEVEL_0);
		currWaterLevelValue = LEVEL_0;
		break;
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