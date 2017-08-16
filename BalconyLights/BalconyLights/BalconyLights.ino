#include <TimeAlarms.h>
#include <TimeLib.h>
#include <Time.h>
#include <SPI.h>

#define LIGHT_NODE
#define NODE_HAS_RELAY

#define MY_RADIO_NRF24
//#define MY_REPEATER_FEATURE
#define MY_NODE_ID BALCONYLIGHT_NODE_ID
#define MY_PARENT_NODE_ID BALCONY_REPEATER_NODE_ID
#define MY_PARENT_NODE_IS_STATIC
//#define MY_DEBUG 

#include <MyNodes.h>
#include <MySensors.h>
#include <MyConfig.h>

#define APPLICATION_NAME "Balcony Light"

#define DEFAULT_CURR_MODE 0
#define DEFAULT_LIGHT_ON_DURATION 60

byte currMode;
byte currModeRequestCount;
byte lightOnDurationRequestCount;

boolean currModeReceived;
boolean lightOnDurationReceived;
boolean sendCurrModeRequest;
boolean sendlightOnDurationRequest;

int lightOnDuration;

AlarmId requestTimer;
AlarmId heartbeatTimer;
AlarmId sendLightStatusTimer;

MyMessage lightRelayMessage(LIGHT_RELAY_ID, V_STATUS);
MyMessage staircaseLightRelayMessage(LIGHT_RELAY_ID, V_STATUS);
MyMessage thingspeakMessage(WIFI_NODEMCU_ID,V_CUSTOM);

void before()
{
	pinMode(LIGHT_RELAY_PIN, OUTPUT);
}

void setup()
{
	digitalWrite(LIGHT_RELAY_PIN, LOW);
	currModeReceived = false;
	lightOnDurationReceived = false;
	sendCurrModeRequest = true;
	sendlightOnDurationRequest = false;
	staircaseLightRelayMessage.setDestination(STAIRCASE_LIGHT_NODE_ID);
	staircaseLightRelayMessage.setType(V_STATUS);
	staircaseLightRelayMessage.setSensor(LIGHT_RELAY_ID);
	thingspeakMessage.setDestination(THINGSPEAK_NODE_ID);
	thingspeakMessage.setType(V_CUSTOM);
	thingspeakMessage.setSensor(WIFI_NODEMCU_ID);
	currModeRequestCount = 0;
	lightOnDurationRequestCount = 0;
	sendLightStatusTimer = Alarm.timerRepeat(FIVE_MINUTES, sendLightStatus);
	heartbeatTimer = Alarm.timerRepeat(HEARTBEAT_INTERVAL, sendHeartbeat);
}

void presentation()
{
	sendSketchInfo(APPLICATION_NAME, __DATE__);
	present(LIGHT_RELAY_ID, S_BINARY, "Balcony Light Relay");
	Alarm.delay(WAIT_AFTER_SEND_MESSAGE);
	present(CURR_MODE_ID, S_CUSTOM, "Operating Mode");
	Alarm.delay(WAIT_AFTER_SEND_MESSAGE);
	present(LIGHT_DURATION_ID, S_CUSTOM, "Light On Duration");
	Alarm.delay(WAIT_AFTER_SEND_MESSAGE);
	send(lightRelayMessage.set(RELAY_OFF));
	Alarm.delay(WAIT_AFTER_SEND_MESSAGE);
	send(staircaseLightRelayMessage.set(RELAY_OFF));
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
		requestTimer = Alarm.timerOnce(REQUEST_INTERVAL, checkCurrModeRequestStatus);
		currModeRequestCount++;
		if (currModeRequestCount == 10)
		{
			MyMessage currModeMessage(CURR_MODE_ID, V_VAR1);
			send(currModeMessage.set(DEFAULT_CURR_MODE));
			wait(WAIT_AFTER_SEND_MESSAGE);
		}
	}
	
	if (sendlightOnDurationRequest)
	{
		sendlightOnDurationRequest = false;
		request(LIGHT_DURATION_ID, V_VAR2);
		requestTimer = Alarm.timerOnce(REQUEST_INTERVAL, checkLightOnDurationRequest);
		lightOnDurationRequestCount++;
		if (lightOnDurationRequestCount == 10)
		{
			MyMessage lightOnDurationMessage(LIGHT_DURATION_ID, V_VAR2);
			send(lightOnDurationMessage.set(DEFAULT_LIGHT_ON_DURATION));
			wait(WAIT_AFTER_SEND_MESSAGE);
		}
	}

	Alarm.delay(1);
}

void receive(const MyMessage &message)
{
	int newLightOnDuration;
	switch (message.type)
	{
	case V_VAR1:
		if (currModeReceived)
		{
			switch (message.sender)
			{
			case THINGSPEAK_NODE_ID:
				currMode = message.getLong();
				break;
			default:
				currMode = message.getInt();
				break;
			}
			switch (currMode)
			{
			case STANDBY_MODE:
				digitalWrite(LIGHT_RELAY_PIN, RELAY_OFF);
				send(lightRelayMessage.set(RELAY_OFF));
				wait(WAIT_AFTER_SEND_MESSAGE);
				send(thingspeakMessage.set(RELAY_OFF));
				wait(WAIT_AFTER_SEND_MESSAGE);
				send(staircaseLightRelayMessage.set(RELAY_OFF));
				wait(WAIT_AFTER_SEND_MESSAGE);
				currMode = STANDBY_MODE;
				Alarm.free(sendLightStatusTimer);
				break;
			case DUSKLIGHT_MODE:
				digitalWrite(LIGHT_RELAY_PIN, RELAY_ON);
				send(lightRelayMessage.set(RELAY_ON));
				wait(WAIT_AFTER_SEND_MESSAGE);
				send(thingspeakMessage.set(RELAY_ON));
				wait(WAIT_AFTER_SEND_MESSAGE);
				send(staircaseLightRelayMessage.set(RELAY_ON));
				wait(WAIT_AFTER_SEND_MESSAGE);
				currMode = DUSKLIGHT_MODE;
				sendLightStatusTimer = Alarm.timerRepeat(FIVE_MINUTES, sendLightStatus);
				break;
			}
			MyMessage currModeMessage(CURR_MODE_ID, V_VAR1);
			send(currModeMessage.set(currMode));
			wait(WAIT_AFTER_SEND_MESSAGE);
		}
		else
		{
			currMode = message.getInt();
			currModeReceived = true;
			Alarm.free(requestTimer);
			request(CURR_MODE_ID, V_VAR1);
			wait(WAIT_AFTER_SEND_MESSAGE);
			sendlightOnDurationRequest = true;
		}
		break;
	case V_VAR2:
		newLightOnDuration = message.getInt();

		if (lightOnDurationReceived && newLightOnDuration > 0 && newLightOnDuration <= 600)
		{
			lightOnDuration = newLightOnDuration;
			MyMessage lightOnDurationMessage(LIGHT_DURATION_ID, V_VAR2);
			send(lightOnDurationMessage.set(lightOnDuration));
			wait(WAIT_AFTER_SEND_MESSAGE);
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
				wait(WAIT_AFTER_SEND_MESSAGE);
			}
			lightOnDurationReceived = true;
			Alarm.free(requestTimer);
			request(LIGHT_DURATION_ID, V_VAR2);
			wait(WAIT_AFTER_SEND_MESSAGE);
		}
		break;
	case V_VAR3:
		if (currMode == STANDBY_MODE)
		{
			if (message.getInt())
			{
				digitalWrite(LIGHT_RELAY_PIN, RELAY_ON);
				send(lightRelayMessage.set(RELAY_ON));
				wait(WAIT_AFTER_SEND_MESSAGE);
				send(thingspeakMessage.set(RELAY_ON));
				wait(WAIT_AFTER_SEND_MESSAGE);
				send(staircaseLightRelayMessage.set(RELAY_ON));
				wait(WAIT_AFTER_SEND_MESSAGE);
			}
			else
			{
				digitalWrite(LIGHT_RELAY_PIN, RELAY_OFF);
				send(lightRelayMessage.set(RELAY_OFF));
				wait(WAIT_AFTER_SEND_MESSAGE);
				send(thingspeakMessage.set(RELAY_OFF));
				wait(WAIT_AFTER_SEND_MESSAGE);
				send(staircaseLightRelayMessage.set(RELAY_OFF));
				wait(WAIT_AFTER_SEND_MESSAGE);
			}
		}
		break;
	case V_STATUS:
		if (currModeReceived && lightOnDurationReceived)
		{
			if (digitalRead(LIGHT_RELAY_PIN))
				send(staircaseLightRelayMessage.set(RELAY_ON));
			else
				send(staircaseLightRelayMessage.set(RELAY_OFF));
			wait(WAIT_AFTER_SEND_MESSAGE);
		}
		break;
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

void sendLightStatus()
{
	if (digitalRead(LIGHT_RELAY_PIN))
		send(staircaseLightRelayMessage.set(RELAY_ON));
	else
		send(staircaseLightRelayMessage.set(RELAY_OFF));
	wait(WAIT_AFTER_SEND_MESSAGE);
}