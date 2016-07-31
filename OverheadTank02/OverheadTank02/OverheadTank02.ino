#include <TimeAlarms.h>
#include <Time.h>
#include <SPI.h>

#define OVERHEAD_TANK_02_NODE
#define NODE_INTERACTS_WITH_RELAY

#define MY_RADIO_NRF24
#define MY_REPEATER_FEATURE
#define MY_NODE_ID OVERHEAD_TANK_02_NODE_ID
#define MY_DEBUG 

#include <MyNodes.h>
#include <MySensors.h>
#include <MyConfig.h>

#define APPLICATION_NAME "Overhead Tank 02"
#define APPLICATION_VERSION "22Jul2016"
#define NUMBER_OF_SENSORS 5
#define DEFAULT_LOW_LEVEL NUMBER_OF_SENSORS/2
#define DEFAULT_POLL_INTERVAL_SECS 900
#define RISING_LEVEL_POLL_INTERVAL_SECS 60

#define SENSOR_1_PIN 4
#define SENSOR_2_PIN 5
#define SENSOR_3_PIN 6
#define SENSOR_4_PIN 7
#define SENSOR_5_PIN 8

boolean waterLowLevelSensorReceived;
boolean sendWaterLowLevelSensorRequest;
boolean isWaterLevelRising;
byte sendWaterLowLevelSensorRequesCount;
byte lowLevel;
byte prevLevel;
AlarmId waterLowLevelSensorTimer;
AlarmId heartbeatTimer;
AlarmId pollingTimer;
MyMessage lightRelayMessage(SUMP_RELAY_ID, V_STATUS);
MyMessage lcdDisplayMessage(LCD_DISPLAY_ID, V_TEXT);
MyMessage sumpRelayMessage(SUMP_RELAY_ID, V_STATUS);
MyMessage waterLevelMessage()

void before()
{
	pinMode(SENSOR_1_PIN, INPUT_PULLUP);
	pinMode(SENSOR_2_PIN, INPUT_PULLUP);
	pinMode(SENSOR_3_PIN, INPUT_PULLUP);
	pinMode(SENSOR_4_PIN, INPUT_PULLUP);
	pinMode(SENSOR_5_PIN, INPUT_PULLUP);
}

void setup()
{
	waterLowLevelSensorReceived = false;
	sendWaterLowLevelSensorRequest = true;
	heartbeatTimer = Alarm.timerRepeat(HEARTBEAT_INTERVAL, sendHeartbeat);
	sendWaterLowLevelSensorRequesCount = 0;
	isWaterLevelRising = false;
	lcdDisplayMessage.setDestination(LCD_DISPLAY_NODE_ID);
	lcdDisplayMessage.setType(V_TEXT);
	lcdDisplayMessage.setSensor(LCD_DISPLAY_ID);
	sumpRelayMessage.setDestination(SUMP_RELAY_NODE_ID);
	sumpRelayMessage.setType(V_STATUS);
	sumpRelayMessage.setSensor(SUMP_RELAY_ID);
	prevLevel = 0;
	getWaterLevel();
}

void presentation()
{
	sendSketchInfo(APPLICATION_NAME, APPLICATION_VERSION);
	present(LOW_LEVEL_SENSOR_ID, S_CUSTOM, "Tank Low Level");
	wait(WAIT_50MS);
}

void loop()
{
	if (sendWaterLowLevelSensorRequest)
	{
		waterLowLevelSensorReceived = false;
		waterLowLevelSensorTimer = Alarm.timerOnce(ONE_MINUTE, checkWaterLevelStatusRequest);
		request(LOW_LEVEL_SENSOR_ID, V_VAR1);
		sendWaterLowLevelSensorRequesCount++;
		if (sendWaterLowLevelSensorRequesCount == 10)
		{
			MyMessage lowLevelMessage(LOW_LEVEL_SENSOR_ID, V_VAR1);
			send(lowLevelMessage.set(DEFAULT_LOW_LEVEL));
		}
	}

	Alarm.delay(1);
}

void receive(const MyMessage &message)
{
	if (message.type == V_VAR1)
	{
		byte receivedLevel = message.getByte();
		if (receivedLevel >= NUMBER_OF_SENSORS)
			lowLevel = DEFAULT_LOW_LEVEL;
		else
			lowLevel = receivedLevel;

		if (!waterLowLevelSensorReceived)
		{
			waterLowLevelSensorReceived = true;
			Alarm.free(waterLowLevelSensorTimer);
			sendWaterLowLevelSensorRequest = false;
		}
	}
}

void checkWaterLevelStatusRequest()
{
	if (!waterLowLevelSensorReceived)
		sendWaterLowLevelSensorRequest = true;
}

void getWaterLevel()
{
	byte sensorArray[NUMBER_OF_SENSORS] = { SENSOR_1_PIN,SENSOR_2_PIN,SENSOR_3_PIN,SENSOR_4_PIN,SENSOR_5_PIN };
	byte sensorData[NUMBER_OF_SENSORS] = { 0,0,0,0,0 };
	byte dec = pow(2, NUMBER_OF_SENSORS);
	byte currLevel = 0;
	byte sensor;
	byte pin;
	for (sensor = 0, pin = sensorArray[sensor]; sensor < NUMBER_OF_SENSORS; sensor++)
	{
		sensorData[sensor] = digitalRead(sensorArray[sensor]);
		if (sensorData[sensor] = HIGH)
		{
			sensor = NUMBER_OF_SENSORS;
			currLevel = currLevel + dec;
		}
		dec = dec / 2;
	}
	
	if(sensor == lowLevel)
		send(sumpRelayMessage.set(RELAY_ON));

	if (currLevel <= prevLevel)
	{
		Alarm.free(pollingTimer);
		isWaterLevelRising = false;
		pollingTimer = Alarm.timerRepeat(DEFAULT_POLL_INTERVAL_SECS, getWaterLevel);
	}
	else
	{
		Alarm.free(pollingTimer);
		isWaterLevelRising = true;
		pollingTimer = Alarm.timerRepeat(RISING_LEVEL_POLL_INTERVAL_SECS, getWaterLevel);
	}

	switch (currLevel)
	{
	case 0: send(sumpRelayMessage.set(RELAY_OFF));
			Alarm.delay(WAIT_50MS);
			send(lcdDisplayMessage.set("100"));
			break;
	case 1: send(lcdDisplayMessage.set("100"));
			break;
	case 2: send(lcdDisplayMessage.set("80"));
			break;
	case 4: send(lcdDisplayMessage.set("60"));
			break;
	case 8: send(lcdDisplayMessage.set("40"));
			break;
	case 16: send(lcdDisplayMessage.set("20"));
			break;
	}
}