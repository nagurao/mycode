#include <TimeAlarms.h>
#include <TimeLib.h>
#include <Time.h>
#include <SPI.h>

#define TANK_01_NODE
#define WATER_TANK_NODE
#define NODE_INTERACTS_WITH_LCD

#define MY_RADIO_NRF24
//#define MY_REPEATER_FEATURE
#define MY_NODE_ID TANK_01_NODE_ID
//#define MY_DEBUG

#include <MyNodes.h>
#include <MySensors.h>
#include <MyConfig.h>

#define APPLICATION_NAME "Tank 01"

#define DEFAULT_ANALOG_TANK_EMPTY_VALUE 34
#define DEFAULT_ANALOG_TANK_FULL_VALUE 80

AlarmId heartbeatTimer;
AlarmId requestTimer;
AlarmId pollingTimer;

byte waterLowLevelInPercent;
byte currWaterLevelPercent;
byte prevWaterLevelPercent;

int tankEmptyAnalogValue;
int tankFullAnalogValue;

byte requestCount;
boolean waterLowLevelReceived;
boolean sendWaterLowLevelRequest;
boolean tankEmptyAnalogValueReceived;
boolean sendTankEmptyAnalogValueRequest;
boolean tankFullAnalogValueReceived;
boolean sendTankFullAnalogValueRequest;

MyMessage waterLevelMessage(CURR_WATER_LEVEL_ID,V_VOLUME);
MyMessage lcdWaterLevelMessage(CURR_WATER_LEVEL_ID, V_VOLUME);

MyMessage waterLevelMessage(CURR_WATER_LEVEL_ID, V_VOLUME);
MyMessage waterLevelToBorewellMessage(CURR_WATER_LEVEL_ID, V_VOLUME);
MyMessage waterLowLevelMessage(WATER_LOW_LEVEL_IND_ID, V_VAR1);
MyMessage analogValueMessage;
MyMessage thingspeakMessage(WIFI_NODEMCU_ID, V_CUSTOM);



void before()
{
	pinMode(PRESSURE_SENSOR_PIN, INPUT);
	pinMode(REFERENCE_VOLTAGE_PIN, INPUT);
}

void setup()
{
	waterLowLevelReceived = false;
	tankEmptyAnalogValueReceived = false;
	tankFullAnalogValueReceived = false;
	sendWaterLowLevelRequest = true;
	sendTankEmptyAnalogValueRequest = false;
	sendTankFullAnalogValueRequest = false;
	requestCount = 0;
	currWaterLevelPercent = 0;
	prevWaterLevelPercent = 0;
	lcdWaterLevelMessage.setDestination(LCD_NODE_ID);
	waterLevelToBorewellMessage.setDestination(BOREWELL_NODE_ID);

	thingspeakMessage.setDestination(THINGSPEAK_NODE_ID);
	thingspeakMessage.setType(V_CUSTOM);
	thingspeakMessage.setSensor(WIFI_NODEMCU_ID);

	pollingTimer = Alarm.timerRepeat(DEFAULT_LEVEL_POLL_DURATION, getWaterLevel);
	heartbeatTimer = Alarm.timerRepeat(HEARTBEAT_INTERVAL, sendHeartbeat);
}

void presentation()
{
	sendSketchInfo(APPLICATION_NAME, __DATE__);
	present(CURR_WATER_LEVEL_ID, S_WATER, "Tank 01 Water Level");
	Alarm.delay(WAIT_AFTER_SEND_MESSAGE);
	present(WATER_LOW_LEVEL_IND_ID, S_CUSTOM, "T1 Low Level %");
	Alarm.delay(WAIT_AFTER_SEND_MESSAGE);
	present(ANALOG_TANK_EMPTY_ID, S_CUSTOM, "T1 Analog Empty");
	Alarm.delay(WAIT_AFTER_SEND_MESSAGE);
	present(ANALOG_TANK_FULL_ID, S_CUSTOM, "T1 Analog Full");
	Alarm.delay(WAIT_AFTER_SEND_MESSAGE);
}

void loop() 
{
	if (sendWaterLowLevelRequest)
	{
		sendWaterLowLevelRequest = false;
		request(WATER_LOW_LEVEL_IND_ID, V_VAR1);
		wait(WAIT_AFTER_SEND_MESSAGE);
		requestTimer = Alarm.timerOnce(REQUEST_INTERVAL, checkWaterLowLevelRequestStatus);
		requestCount++;
		if (requestCount == 10)
		{
			MyMessage waterLowLevelMessage(WATER_LOW_LEVEL_IND_ID, V_VAR1);
			send(waterLowLevelMessage.set(DEFAULT_LOW_LEVEL));
			wait(WAIT_AFTER_SEND_MESSAGE);
		}
	}
	if (sendTankEmptyAnalogValueRequest)
	{
		sendTankEmptyAnalogValueRequest = false;
		request(ANALOG_TANK_EMPTY_ID, V_VAR2);
		wait(WAIT_AFTER_SEND_MESSAGE);
		requestTimer = Alarm.timerOnce(REQUEST_INTERVAL, checkTankEmptyAnalogValueRequestStatus);
		requestCount++;
		if (requestCount == 10)
		{
			analogValueMessage.setSensor(ANALOG_TANK_EMPTY_ID);
			analogValueMessage.setType(V_VAR2);
			send(analogValueMessage.set(DEFAULT_ANALOG_TANK_EMPTY_VALUE));
			wait(WAIT_AFTER_SEND_MESSAGE);
		}
	}
	if (sendTankFullAnalogValueRequest)
	{
		sendTankFullAnalogValueRequest = false;
		request(ANALOG_TANK_FULL_ID, V_VAR3);
		wait(WAIT_AFTER_SEND_MESSAGE);
		requestTimer = Alarm.timerOnce(REQUEST_INTERVAL, checkTankFullAnalogValueRequestStatus);
		requestCount++;
		if (requestCount == 10)
		{
			analogValueMessage.setSensor(ANALOG_TANK_FULL_ID);
			analogValueMessage.setType(V_VAR3);
			send(analogValueMessage.set(DEFAULT_ANALOG_TANK_FULL_VALUE));
			wait(WAIT_AFTER_SEND_MESSAGE);
		}
	}
	Alarm.delay(1);
}
void receive(const MyMessage &message)
{
	switch (message.type)
	{
	case V_VAR1:
		if (message.getByte() > 0)
		{
			waterLowLevelInPercent = message.getByte();
			send(waterLowLevelMessage.set(message.getByte()));
			wait(WAIT_AFTER_SEND_MESSAGE);
			send(waterLevelToBorewellMessage.set(message.getByte()));
			wait(WAIT_AFTER_SEND_MESSAGE);
			if (!waterLowLevelReceived)
			{
				waterLowLevelReceived = true;
				Alarm.free(requestTimer);
				request(WATER_LOW_LEVEL_IND_ID, V_VAR1);
				requestCount = 0;
				sendTankEmptyAnalogValueRequest = true;
			}
		}
		break;
	case V_VAR2:
		if (message.getInt() > 0)
		{
			tankEmptyAnalogValue = message.getInt();
			analogValueMessage.setSensor(ANALOG_TANK_EMPTY_ID);
			analogValueMessage.setType(V_VAR2);
			send(analogValueMessage.set(tankEmptyAnalogValue));
			wait(WAIT_AFTER_SEND_MESSAGE);
			if (!tankEmptyAnalogValueReceived)
			{
				tankEmptyAnalogValueReceived = true;
				Alarm.free(requestTimer);
				request(ANALOG_TANK_EMPTY_ID, V_VAR2);
				requestCount = 0;
				sendTankFullAnalogValueRequest = true;
			}
		}
		break;
	case V_VAR3:
		if (message.getInt() > 0)
		{
			tankFullAnalogValue = message.getInt();
			analogValueMessage.setSensor(ANALOG_TANK_FULL_ID);
			analogValueMessage.setType(V_VAR3);
			send(analogValueMessage.set(tankFullAnalogValue));
			wait(WAIT_AFTER_SEND_MESSAGE);
			if (!tankFullAnalogValueReceived)
			{
				tankFullAnalogValueReceived = true;
				Alarm.free(requestTimer);
				request(ANALOG_TANK_FULL_ID, V_VAR3);
				requestCount = 0;
			}
		}
		break;
	case V_STATUS:
		if (message.getByte())
		{
			Alarm.free(pollingTimer);
			pollingTimer = Alarm.timerRepeat(RISING_LEVEL_POLL_DURATION, getWaterLevel);
		}
		else
		{
			Alarm.free(pollingTimer);
			pollingTimer = Alarm.timerRepeat(DEFAULT_LEVEL_POLL_DURATION, getWaterLevel);
		}
	}
}

void getWaterLevel()
{
	float sensedInputVoltage = 0;
	float thresholdVoltage = 0;
	for (byte readCount = 1; readCount <= 10; readCount++)
	{
		thresholdVoltage = thresholdVoltage + analogRead(REFERENCE_VOLTAGE_PIN);
		wait(WAIT_50MS);
		sensedInputVoltage = sensedInputVoltage + analogRead(PRESSURE_SENSOR_PIN);
		wait(WAIT_50MS);
	}
	thresholdVoltage = thresholdVoltage / 10;
	sensedInputVoltage = sensedInputVoltage / 10;
}

void checkWaterLowLevelRequestStatus()
{
	if (!waterLowLevelReceived)
		sendWaterLowLevelRequest = true;
}

void checkTankEmptyAnalogValueRequestStatus()
{
	if (!tankEmptyAnalogValueReceived)
		sendTankEmptyAnalogValueRequest = true;
}

void checkTankFullAnalogValueRequestStatus()
{
	if(!tankFullAnalogValueReceived)
		sendTankFullAnalogValueRequest = true;
}