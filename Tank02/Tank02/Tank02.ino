#include <TimeAlarms.h>
#include <TimeLib.h>
#include <Time.h>
#include <SPI.h>
#define WATER_TANK_NODE
#define OVERHEAD_TANK_02_NODE
#define NODE_INTERACTS_WITH_RELAY
#define NODE_INTERACTS_WITH_LCD

#define MY_RADIO_NRF24
#define MY_REPEATER_FEATURE
#define MY_NODE_ID OVERHEAD_TANK_02_NODE_ID
#define MY_DEBUG

#include <MyNodes.h>
#include <MySensors.h>
#include <MyConfig.h>

#define APPLICATION_NAME "Tank 02"
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

boolean sumpMotorOn;
boolean sumpMotorStatusReceived;

MyMessage waterLevelMessage(CURR_WATER_LEVEL_ID, V_VOLUME);
MyMessage waterLowLevelMessage(WATER_LOW_LEVEL_IND_ID, V_VAR1);
MyMessage lcdWaterLevelMessage(CURR_WATER_LEVEL_ID, V_VOLUME);
MyMessage sumpMotorMessage(SUMP_RELAY_ID, V_STATUS);
MyMessage thingspeakMessage(WIFI_NODEMCU_ID, V_CUSTOM);

void before()
{
	for (byte sensorIndex = 0; sensorIndex < MAX_SENSORS; sensorIndex++)
		pinMode(sensorPinArray[sensorIndex], INPUT_PULLUP);
}

void setup()
{
	sendWaterLowLevelRequest = true;
	sumpMotorOn = false;
	sumpMotorStatusReceived = false;
	lcdWaterLevelMessage.setDestination(LCD_NODE_ID);
	lcdWaterLevelMessage.setSensor(CURR_WATER_LEVEL_ID);
	thingspeakMessage.setDestination(THINGSPEAK_NODE_ID);
	thingspeakMessage.setType(V_CUSTOM);
	thingspeakMessage.setSensor(WIFI_NODEMCU_ID);
	sumpMotorMessage.setDestination(SUMP_RELAY_NODE_ID);
	sumpMotorMessage.setSensor(SUMP_RELAY_ID);
	sumpMotorMessage.setType(V_STATUS);
	heartbeatTimer = Alarm.timerRepeat(HEARTBEAT_INTERVAL, sendHeartbeat);
	waterDefaultLevelTimer = Alarm.timerRepeat(DEFAULT_LEVEL_POLL_DURATION, getWaterLevel);
	waterLevelFallingTimer = Alarm.timerRepeat(FALLING_LEVEL_POLL_DURATION, getWaterLevel);
	waterLevelRisingTimer = Alarm.timerRepeat(RISING_LEVEL_POLL_DURATION, getWaterLevel);
}

void presentation()
{
	sendSketchInfo(APPLICATION_NAME, APPLICATION_VERSION);
	present(CURR_WATER_LEVEL_ID, S_WATER, "Tank 02 Water Level");
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
	case V_VAR4:
		sumpMotorOn = (message.getInt()) ? RELAY_ON : RELAY_OFF;
		sumpMotorStatusReceived = true;
		break;
	case V_VAR5:
		if (message.getInt())
		{
			Alarm.disable(waterLevelFallingTimer);
			Alarm.enable(waterLevelRisingTimer);
		}
		else
		{
			Alarm.disable(waterLevelRisingTimer);
			Alarm.enable(waterLevelFallingTimer);
		}
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

	if (sensorArray[waterOverFlowLevelIndex] == LOW)
	{
		if (sumpMotorOn)
		{
			if (sumpMotorOn && sumpMotorStatusReceived)
			{
				send(sumpMotorMessage.set(RELAY_OFF));
				Alarm.disable(waterDefaultLevelTimer);
				Alarm.disable(waterLevelRisingTimer);
				Alarm.enable(waterLevelFallingTimer);
				sumpMotorStatusReceived = false;
			}
			else
			{
				request(SUMP_RELAY_ID, V_VAR4, SUMP_RELAY_NODE_ID);
				sumpMotorStatusReceived = false;
			}
		}
	}

	if (sensorArray[waterLowLevelIndex] == HIGH)
	{
		if (!sumpMotorOn)
		{
			if (!sumpMotorOn && sumpMotorStatusReceived)
			{
				send(sumpMotorMessage.set(RELAY_ON));
				Alarm.disable(waterDefaultLevelTimer);
				Alarm.disable(waterLevelFallingTimer);
				Alarm.enable(waterLevelRisingTimer);
				sumpMotorStatusReceived = false;
			}
			else
			{
				request(SUMP_RELAY_ID, V_VAR4, SUMP_RELAY_NODE_ID);
				sumpMotorStatusReceived = false;
			}
		}
	}

	prevWaterLevelValue = currWaterLevelValue;

}

void sendWaterLevel(int waterLevel)
{
	MyMessage waterHighLevelMessage(WATER_FULL_LEVEL_ID, V_VAR2);
	if (waterLevel != prevWaterLevelValue)
	{
		send(waterLevelMessage.set(waterLevel));
		Alarm.delay(WAIT_5MS);
		send(lcdWaterLevelMessage.set(waterLevel));
		Alarm.delay(WAIT_5MS);
		send(thingspeakMessage.set(waterLevel));
		if (currWaterLevelValueDec == 0)
		{			
			waterHighLevelMessage.setDestination(SUMP_RELAY_NODE_ID);
			send(waterHighLevelMessage.set(HIGH_LEVEL)); 
		}
		else
		{
			waterHighLevelMessage.setDestination(SUMP_RELAY_NODE_ID);
			send(waterHighLevelMessage.set(NOT_HIGH_LEVEL)); 
		}
		Alarm.delay(WAIT_5MS);
	}
}

void checkWaterLowLevelRequestStatus()
{
	if (!waterLowLevelReceived)
		sendWaterLowLevelRequest = true;
}