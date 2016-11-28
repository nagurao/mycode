#include <TimeAlarms.h>
#include <TimeLib.h>
#include <Time.h>
#include <SPI.h>

#define MOTION_SENSOR_WITH_LIGHT
#define NODE_HAS_RELAY
#define STAIRCASE_NODE

#define MY_RADIO_NRF24
#define MY_REPEATER_FEATURE
#define MY_NODE_ID GATELIGHT_WITH_PIR_NODE_ID
#define MY_DEBUG 

#include <MyNodes.h>
#include <MySensors.h>
#include <MyConfig.h>

#define APPLICATION_NAME "PIR Gate Light"
#define APPLICATION_VERSION "28Nov2016"
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
boolean sendCurrModeRequest;
boolean sendlightOnDurationRequest;
int lightOnDuration;

AlarmId motionSensor;
AlarmId currModeTimer;
AlarmId lightOnDurationTimer;
AlarmId heartbeatTimer;

MyMessage sensorMessage(MOTION_SENSOR_ID, V_TRIPPED);
MyMessage lightRelayMessage(LIGHT_RELAY_ID, V_STATUS);
MyMessage staircaseLightRelayMessage(STAIRCASE_LIGHT_RELAY_ID, V_STATUS);
MyMessage thingspeakMessage(WIFI_NODEMCU_ID, V_CUSTOM);

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
	present(MOTION_SENSOR_ID, S_MOTION, "Gate Motion Sensor");
	Alarm.delay(WAIT_AFTER_SEND_MESSAGE);
	present(LIGHT_RELAY_ID, S_BINARY, "Gate Light Relay");
	Alarm.delay(WAIT_AFTER_SEND_MESSAGE);
	present(CURR_MODE_ID, S_CUSTOM, "Operating Mode");
	Alarm.delay(WAIT_AFTER_SEND_MESSAGE);
	present(LIGHT_DURATION_ID, S_CUSTOM, "Light On Duration");
	Alarm.delay(WAIT_AFTER_SEND_MESSAGE);
	send(sensorMessage.set(NO_MOTION_DETECTED));
	Alarm.delay(WAIT_AFTER_SEND_MESSAGE);
	send(lightRelayMessage.set(RELAY_OFF));
	Alarm.delay(WAIT_AFTER_SEND_MESSAGE);
	send(thingspeakMessage.set(RELAY_OFF));
	Alarm.delay(WAIT_AFTER_SEND_MESSAGE);
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
			Alarm.delay(WAIT_AFTER_SEND_MESSAGE);
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
			Alarm.delay(WAIT_AFTER_SEND_MESSAGE);
		}
	}

	if (currModeReceived && lightOnDurationReceived && currMode == SENSOR_MODE)
	{
		if (tripped && !trippMessageToRelay)
		{
			digitalWrite(LIGHT_RELAY_PIN, RELAY_ON);
			send(lightRelayMessage.set(RELAY_ON));
			Alarm.delay(WAIT_AFTER_SEND_MESSAGE);
			trippMessageToRelay = true;
			send(staircaseLightRelayMessage.set(RELAY_ON));
			Alarm.delay(WAIT_AFTER_SEND_MESSAGE);
			send(thingspeakMessage.set(RELAY_ON));
			Alarm.delay(WAIT_AFTER_SEND_MESSAGE);
			Alarm.timerOnce(lightOnDuration, turnOffLightRelay);
			tripped = false;
			disableMotionSensor();
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
				Alarm.delay(WAIT_AFTER_SEND_MESSAGE);
				send(thingspeakMessage.set(RELAY_OFF));
				Alarm.delay(WAIT_AFTER_SEND_MESSAGE);
				disableMotionSensor();
				currMode = message.getInt();
				break;
			case DUSKLIGHT_MODE:
				digitalWrite(LIGHT_RELAY_PIN, RELAY_ON);
				send(lightRelayMessage.set(RELAY_ON));
				Alarm.delay(WAIT_AFTER_SEND_MESSAGE);
				send(thingspeakMessage.set(RELAY_ON));
				Alarm.delay(WAIT_AFTER_SEND_MESSAGE);
				disableMotionSensor();
				currMode = message.getInt();
				break;
			case SENSOR_MODE:
				digitalWrite(LIGHT_RELAY_PIN, RELAY_OFF);
				send(lightRelayMessage.set(RELAY_OFF));
				Alarm.delay(WAIT_AFTER_SEND_MESSAGE);
				send(thingspeakMessage.set(RELAY_OFF));
				Alarm.delay(WAIT_AFTER_SEND_MESSAGE);
				trippMessageToRelay = false;
				send(staircaseLightRelayMessage.set(RELAY_OFF));
				Alarm.delay(WAIT_AFTER_SEND_MESSAGE);
				enableMotionSensor();
				currMode = message.getInt();
				break;
			case ADHOC_MODE:
				disableMotionSensor();
				if (digitalRead(LIGHT_RELAY_PIN))
				{
					digitalWrite(LIGHT_RELAY_PIN, RELAY_OFF);
					send(lightRelayMessage.set(RELAY_OFF));
					Alarm.delay(WAIT_AFTER_SEND_MESSAGE);
					send(thingspeakMessage.set(RELAY_OFF));
					Alarm.delay(WAIT_AFTER_SEND_MESSAGE);
				}
				else
				{
					digitalWrite(LIGHT_RELAY_PIN, RELAY_ON);
					send(lightRelayMessage.set(RELAY_ON));
					Alarm.delay(WAIT_AFTER_SEND_MESSAGE);
					send(staircaseLightRelayMessage.set(RELAY_ON));
					Alarm.delay(WAIT_AFTER_SEND_MESSAGE);
				}
				currMode = message.getInt();
				break;
			}
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
			Alarm.delay(WAIT_AFTER_SEND_MESSAGE);
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
				Alarm.delay(WAIT_AFTER_SEND_MESSAGE);
			}
			lightOnDurationReceived = true;
			Alarm.free(lightOnDurationTimer);
			sendlightOnDurationRequest = false;
			request(LIGHT_DURATION_ID, V_VAR2);
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
	Alarm.delay(WAIT_AFTER_SEND_MESSAGE);
	send(thingspeakMessage.set(RELAY_OFF));
	Alarm.delay(WAIT_AFTER_SEND_MESSAGE);
	send(staircaseLightRelayMessage.set(RELAY_OFF));
	Alarm.delay(WAIT_AFTER_SEND_MESSAGE);
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