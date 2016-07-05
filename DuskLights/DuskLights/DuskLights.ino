#include <TimeAlarms.h>
#include <Time.h>
#include <SPI.h>

#define MOTION_SENSOR_WITH_LIGHT
#define NODE_HAS_RELAY

#define MY_RADIO_NRF24
#define MY_REPEATER_FEATURE
#define MY_NODE_ID 200
#define MY_DEBUG 

#include <MyNodes.h>
#include <MySensors.h>
#include <MyConfig.h>

#define APPLICATION_NAME "Dusk Light with PIR"
#define APPLICATION_VERSION "29Jun2016"
#define SENSOR_POLL_TIME 120
#define DEFAULT_LIGHT_ON_DURATION 300

byte currMode;
boolean firstRun;
boolean tripped;
boolean trippMessageToRelay;
boolean currModeReceived;
boolean lightIntervalReceived;
int lightOnDuration;
AlarmId motionSensor;

MyMessage sensorMessage(MOTION_SENSOR_ID, V_TRIPPED);
MyMessage lightRelayMessage(LIGHT_RELAY_ID, V_STATUS);

#define LOG(X) Serial.println(X)
void before()
{
	pinMode(LIGHT_RELAY_PIN, OUTPUT);
	pinMode(MOTION_SENSOR_PIN, INPUT);
}

void setup()
{
	digitalWrite(LIGHT_RELAY_PIN, LOW);
	currModeReceived = false;
	lightIntervalReceived = false;
	tripped = false;
	trippMessageToRelay = false;
	firstRun = true;
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
}

void loop()
{
	if (firstRun)
	{
		send(sensorMessage.set(NO_MOTION_DETECTED));
		wait(WAIT_50MS);
		send(lightRelayMessage.set(RELAY_OFF));
		firstRun = false;
	}

	if (!currModeReceived)
	{
		request(CURR_MODE_ID, V_VAR1);
		Alarm.delay(1000);
	}
	else
	{
		if (!lightIntervalReceived)
		{
			request(LIGHT_DURATION_ID, V_VAR2);
			Alarm.delay(1000);
		}
	}

	if (currModeReceived && lightIntervalReceived)
	{
		if (tripped && !trippMessageToRelay)
		{
			LOG("if (tripped && !trippMessageToRelay)");
			digitalWrite(LIGHT_RELAY_PIN, RELAY_ON);
			send(lightRelayMessage.set(RELAY_ON));
			trippMessageToRelay = true;
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
		disableMotionSensor();
		if (currModeReceived)
		{
			LOG("if (currModeReceived)");
			switch (message.getInt())
			{
			case STANDBY_MODE:
				digitalWrite(LIGHT_RELAY_PIN, RELAY_OFF);
				send(lightRelayMessage.set(RELAY_OFF));
				disableMotionSensor();
				currMode = message.getInt();
				break;
			case DUSKLIGHT_MODE:
				digitalWrite(LIGHT_RELAY_PIN, RELAY_ON);
				send(lightRelayMessage.set(RELAY_ON));
				disableMotionSensor();
				currMode = message.getInt();
				break;
			case SENSOR_MODE:
				digitalWrite(LIGHT_RELAY_PIN, RELAY_OFF);
				send(lightRelayMessage.set(RELAY_OFF));
				trippMessageToRelay = false;
				enableMotionSensor();
				currMode = message.getInt();
				break;
			case ADHOC_MODE:
				disableMotionSensor();
				if (digitalRead(LIGHT_RELAY_PIN))
				{
					digitalWrite(LIGHT_RELAY_PIN, RELAY_OFF);
					send(lightRelayMessage.set(RELAY_OFF));
				}
				else
				{
					digitalWrite(LIGHT_RELAY_PIN, RELAY_ON);
					send(lightRelayMessage.set(RELAY_ON));
				}
				currMode = message.getInt();
				break;
			}
		}
		else
		{
			LOG("else(currModeReceived)");
			currMode = message.getInt();
			currModeReceived = true;
			if (message.getInt() == SENSOR_MODE)
			{
				trippMessageToRelay = false;
				enableMotionSensor();
			}
		}
	}
	if (message.type == V_VAR2)
	{
		if (lightIntervalReceived)
		{
			int newLightOnDuration = message.getInt();
			if (newLightOnDuration > 0 && newLightOnDuration <= 600)
				lightOnDuration = newLightOnDuration;
		}
		else
		{
			int newLightOnDuration = message.getInt();
			if (newLightOnDuration > 0 && newLightOnDuration <= 600)
				lightOnDuration = newLightOnDuration;
			else
			{
				lightOnDuration = DEFAULT_LIGHT_ON_DURATION;
				MyMessage lightOnDurationMessage(LIGHT_DURATION_ID, V_VAR2);
				send(lightOnDurationMessage.set(lightOnDuration));
			}
			lightIntervalReceived = true;
		}
	}
}

void sendMotionSensorData()
{
	LOG("sendMotionSensorData");
	tripped = digitalRead(MOTION_SENSOR_PIN);
	send(sensorMessage.set(tripped ? MOTION_DETECTED : NO_MOTION_DETECTED));
	trippMessageToRelay = false;
}
void turnOffLightRelay()
{
	LOG("turnOffLightRelay");
	digitalWrite(LIGHT_RELAY_PIN, RELAY_OFF);
	send(lightRelayMessage.set(RELAY_OFF));
	enableMotionSensor();
}

void enableMotionSensor()
{
	LOG("enableMotionSensor");
	attachInterrupt(INTERRUPT_MOTION, sendMotionSensorData, CHANGE);
	motionSensor = Alarm.timerRepeat(SENSOR_POLL_TIME, sendMotionSensorData);
	trippMessageToRelay = false;
}

void disableMotionSensor()
{
	LOG("disableMotionSensor");
	detachInterrupt(INTERRUPT_MOTION);
	Alarm.free(motionSensor);
	detachInterrupt(INTERRUPT_MOTION);
}