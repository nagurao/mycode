#include <TimeAlarms.h>
#include <TimeLib.h>
#include <Time.h>
#include <SPI.h>

#define MOTION_SENSOR_WITH_LIGHT
#define NODE_HAS_RELAY
#define STAIRCASE_NODE

#define MY_RADIO_NRF24
#define MY_REPEATER_FEATURE
#define MY_NODE_ID BALCONYLIGHT_WITH_PIR_NODE_ID
#define MY_DEBUG 

#include <MyNodes.h>
#include <MySensors.h>
#include <MyConfig.h>

#define APPLICATION_NAME "PIR Balcony Light"
#define APPLICATION_VERSION "12Nov2016"
#define SENSOR_POLL_TIME 120
#define DEFAULT_CURR_MODE 0
#define DEFAULT_LIGHT_ON_DURATION 60

byte currMode;
byte currModeRequestCount;
byte lightOnDurationRequestCount;
boolean tripped;
boolean trippMessageToRelay;
boolean currModeReceived;
boolean lightOnDurationReceived;
int lightOnDuration;
boolean sendCurrModeRequest;
boolean sendlightOnDurationRequest;

AlarmId motionSensor;
AlarmId currModeTimer;
AlarmId lightOnDurationTimer;
AlarmId heartbeatTimer;

MyMessage sensorMessage(MOTION_SENSOR_ID, V_TRIPPED);
MyMessage lightRelayMessage(LIGHT_RELAY_ID, V_STATUS);
MyMessage staircaseLightRelayMessage(STAIRCASE_LIGHT_RELAY_ID, V_STATUS);
MyMessage thingspeakMessage(WIFI_NODEMCU_ID,V_CUSTOM);

void before()
{
	pinMode(LIGHT_RELAY_PIN, OUTPUT);
	pinMode(MOTION_SENSOR_PIN, INPUT);
}

void setup()
{
	digitalWrite(LIGHT_RELAY_PIN, LOW);
	currModeReceived = false;
	lightOnDurationReceived = false;
	tripped = false;
	trippMessageToRelay = false;
	sendCurrModeRequest = true;
	sendlightOnDurationRequest = true;
	staircaseLightRelayMessage.setDestination(STAIRCASE_LIGHT_NODE_ID);
	staircaseLightRelayMessage.setType(V_STATUS);
	staircaseLightRelayMessage.setSensor(STAIRCASE_LIGHT_RELAY_ID);
	thingspeakMessage.setDestination(THINGSPEAK_NODE_ID);
	thingspeakMessage.setType(V_CUSTOM);
	thingspeakMessage.setSensor(WIFI_NODEMCU_ID);
	currModeRequestCount = 0;
	lightOnDurationRequestCount = 0;
	heartbeatTimer = Alarm.timerRepeat(HEARTBEAT_INTERVAL, sendHeartbeat);
}

void presentation()
{
	sendSketchInfo(APPLICATION_NAME, APPLICATION_VERSION);
	present(MOTION_SENSOR_ID, S_MOTION, "Balcony Motion Sensor");
	wait(WAIT_50MS);
	present(LIGHT_RELAY_ID, S_BINARY, "Balcony Light Relay");
	wait(WAIT_50MS);
	present(CURR_MODE_ID, S_CUSTOM, "Operating Mode");
	wait(WAIT_50MS);
	present(LIGHT_DURATION_ID, S_CUSTOM, "Light On Duration");
	wait(WAIT_50MS);
	send(sensorMessage.set(NO_MOTION_DETECTED));
	wait(WAIT_50MS);
	send(lightRelayMessage.set(RELAY_OFF));
	wait(WAIT_50MS);
	send(staircaseLightRelayMessage.set(RELAY_OFF));
	wait(WAIT_50MS);
	send(thingspeakMessage.set(RELAY_OFF));
}

void loop()
{
	if (sendCurrModeRequest)
	{
		sendCurrModeRequest = false;
		request(CURR_MODE_ID, V_VAR1);
		currModeTimer = Alarm.timerOnce(REQUEST_INTERVAL, checkCurrModeRequestStatus);
		currModeRequestCount++;
		if (currModeRequestCount == 10)
		{
			MyMessage currModeMessage(CURR_MODE_ID, V_VAR1);
			send(currModeMessage.set(DEFAULT_CURR_MODE));
		}
	}

	if (sendlightOnDurationRequest)
	{
		sendlightOnDurationRequest = false;
		request(LIGHT_DURATION_ID, V_VAR2);
		lightOnDurationTimer = Alarm.timerOnce(REQUEST_INTERVAL, checkLightOnDurationRequest);
		lightOnDurationRequestCount++;
		if (lightOnDurationRequestCount == 10)
		{
			MyMessage lightOnDurationMessage(LIGHT_DURATION_ID, V_VAR2);
			send(lightOnDurationMessage.set(DEFAULT_LIGHT_ON_DURATION));
		}
	}

	if (currModeReceived && lightOnDurationReceived)
	{
		if (tripped && !trippMessageToRelay)
		{
			digitalWrite(LIGHT_RELAY_PIN, RELAY_ON);
			send(lightRelayMessage.set(RELAY_ON));
			trippMessageToRelay = true;
			send(staircaseLightRelayMessage.set(RELAY_ON));
			wait(WAIT_50MS);
			send(thingspeakMessage.set(RELAY_ON));
			Alarm.timerOnce(lightOnDuration, turnOffLightRelay);
			disableMotionSensor();
			tripped = false;
		}
	}

	Alarm.delay(1);
}

void receive(const MyMessage &message)
{
	if (message.type == V_VAR1)
	{
		if (currModeReceived)
		{
			switch (message.getInt())
			{
			case STANDBY_MODE:
				digitalWrite(LIGHT_RELAY_PIN, RELAY_OFF);
				send(lightRelayMessage.set(RELAY_OFF));
				send(thingspeakMessage.set(RELAY_OFF));
				send(staircaseLightRelayMessage.set(RELAY_OFF));
				disableMotionSensor();
				currMode = message.getInt();
				break;
			case DUSKLIGHT_MODE:
				digitalWrite(LIGHT_RELAY_PIN, RELAY_ON);
				send(lightRelayMessage.set(RELAY_ON));
				send(thingspeakMessage.set(RELAY_ON));
				send(staircaseLightRelayMessage.set(RELAY_ON));
				disableMotionSensor();
				currMode = message.getInt();
				break;
			case SENSOR_MODE:
				digitalWrite(LIGHT_RELAY_PIN, RELAY_OFF);
				send(lightRelayMessage.set(RELAY_OFF));
				send(thingspeakMessage.set(RELAY_OFF));
				trippMessageToRelay = false;
				send(staircaseLightRelayMessage.set(RELAY_OFF));
				enableMotionSensor();
				currMode = message.getInt();
				break;
			case ADHOC_MODE:
				disableMotionSensor();
				if (digitalRead(LIGHT_RELAY_PIN))
				{
					digitalWrite(LIGHT_RELAY_PIN, RELAY_OFF);
					send(lightRelayMessage.set(RELAY_OFF));
					send(thingspeakMessage.set(RELAY_OFF));
					send(staircaseLightRelayMessage.set(RELAY_OFF));
				}
				else
				{
					digitalWrite(LIGHT_RELAY_PIN, RELAY_ON);
					send(lightRelayMessage.set(RELAY_ON));
					send(thingspeakMessage.set(RELAY_ON));
					send(staircaseLightRelayMessage.set(RELAY_ON));
				}
				currMode = message.getInt();
				break;
			}
			MyMessage currModeMessage(CURR_MODE_ID, V_VAR1);
			send(currModeMessage.set(message.getInt()));
		}
		else
		{
			currMode = message.getInt();
			currModeReceived = true;
			Alarm.free(currModeTimer);
			sendCurrModeRequest = false;
			request(CURR_MODE_ID, V_VAR1);
		}
	}
	if (message.type == V_VAR2)
	{
		int newLightOnDuration = message.getInt();

		if (lightOnDurationReceived && newLightOnDuration > 0 && newLightOnDuration <= 600)
		{
			lightOnDuration = newLightOnDuration;
			MyMessage lightOnDurationMessage(LIGHT_DURATION_ID, V_VAR2);
			send(lightOnDurationMessage.set(lightOnDuration));
		}

		if (!lightOnDurationReceived)
		{
			if (newLightOnDuration > 0 && newLightOnDuration <= 600)
				lightOnDuration = newLightOnDuration;
			else
			{
				lightOnDuration = DEFAULT_LIGHT_ON_DURATION;
				MyMessage lightOnDurationMessage(LIGHT_DURATION_ID, V_VAR2);
				send(lightOnDurationMessage.set(lightOnDuration));
			}
			lightOnDurationReceived = true;
			Alarm.free(lightOnDurationTimer);
			sendlightOnDurationRequest = false;
			request(LIGHT_DURATION_ID, V_VAR2);
		}
	}
	if (message.type == V_STATUS)
	{
		if (currModeReceived && lightOnDurationReceived)
		{
			if (digitalRead(LIGHT_RELAY_PIN))
				send(staircaseLightRelayMessage.set(RELAY_ON));
			else
				send(staircaseLightRelayMessage.set(RELAY_OFF));
		}
	}
}

void checkCurrModeRequestStatus()
{
	if (!currModeReceived)
		sendCurrModeRequest = true;
}

void checkLightOnDurationRequest()
{
	if (!lightOnDurationReceived)
		sendlightOnDurationRequest = true;
}

void sendMotionSensorData()
{
	tripped = digitalRead(MOTION_SENSOR_PIN);
	send(sensorMessage.set(tripped ? MOTION_DETECTED : NO_MOTION_DETECTED));
	trippMessageToRelay = false;
}

void turnOffLightRelay()
{
	digitalWrite(LIGHT_RELAY_PIN, RELAY_OFF);
	send(lightRelayMessage.set(RELAY_OFF));
	send(thingspeakMessage.set(RELAY_OFF));
	send(staircaseLightRelayMessage.set(RELAY_OFF));
	enableMotionSensor();
}

void enableMotionSensor()
{
	attachInterrupt(INTERRUPT_MOTION, sendMotionSensorData, CHANGE);
	motionSensor = Alarm.timerRepeat(SENSOR_POLL_TIME, sendMotionSensorData);
	trippMessageToRelay = false;
}

void disableMotionSensor()
{
	detachInterrupt(INTERRUPT_MOTION);
	Alarm.free(motionSensor);
	detachInterrupt(INTERRUPT_MOTION);
}