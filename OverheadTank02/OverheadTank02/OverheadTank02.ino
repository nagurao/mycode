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
#define APPLICATION_VERSION "16Jul2016"
#define NUMBER_OF_SENSORS 5
#define SENSOR_1_PIN 4
#define SENSOR_2_PIN 5
#define SENSOR_3_PIN 6
#define SENSOR_4_PIN 7
#define SENSOR_5_PIN 8

#define SENSOR_1 1
#define SENSOR_2 1
#define SENSOR_3 1
#define SENSOR_4 1
#define SENSOR_5 1

boolean waterLowLevelSensorReceived;
boolean sendWaterLowLevelSensorRequest;

AlarmId waterLowLevelSensorTimer;
AlarmId heartbeatTimer;

MyMessage lightRelayMessage(SUMP_RELAY_ID, V_STATUS);

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
	}

	Alarm.delay(1);
}

void receive(const MyMessage &message)
{
	if (message.type == V_VAR1)
	{
		byte receivedLevel = message.getByte();

		if (message.getInt())
		{
			digitalWrite(LIGHT_RELAY_PIN, RELAY_ON);
			Alarm.delay(WAIT_50MS);
			send(lightRelayMessage.set(RELAY_ON));
		}
		else
		{
			digitalWrite(LIGHT_RELAY_PIN, RELAY_OFF);
			Alarm.delay(WAIT_50MS);
			send(lightRelayMessage.set(RELAY_OFF));

		}
		if (!lightStatusReceived)
		{
			lightStatusReceived = true;
			Alarm.free(lightStatusTimer);
			sendLightStatusRequest = false;
		}
	}
}

void checkWaterLevelStatusRequest()
{
	if (!waterLowLevelSensorReceived)
		sendWaterLowLevelSensorRequest = true;
}