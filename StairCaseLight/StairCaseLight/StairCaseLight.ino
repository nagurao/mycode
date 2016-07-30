#include <TimeAlarms.h>
#include <Time.h>
#include <SPI.h>

#define NODE_HAS_RELAY
#define STAIRCASE_NODE

#define MY_RADIO_NRF24
#define MY_REPEATER_FEATURE
#define MY_NODE_ID STAIRCASE_LIGHT_NODE
#define MY_DEBUG 

#include <MyNodes.h>
#include <MySensors.h>
#include <MyConfig.h>

#define APPLICATION_NAME "Staircase Light"
#define APPLICATION_VERSION "30Jul2016"

#define LIGHT_RELAY_PIN 7
#define LIGHT_RELAY_ID 1
#define BALCONY_LIGHT_RELAY_ID 2

boolean lightStatusReceived;
boolean sendLightStatusRequest;
byte lightStatusRequstCount;
AlarmId lightStatusTimer;
AlarmId heartbeatTimer;

MyMessage lightRelayMessage(LIGHT_RELAY_ID, V_STATUS);
MyMessage thingspeakMessage(WIFI_NODEMCU_ID, V_CUSTOM);

void before()
{
	pinMode(LIGHT_RELAY_PIN, OUTPUT);
}

void setup()
{
	digitalWrite(LIGHT_RELAY_PIN, LOW);
	lightStatusReceived = false;
	sendLightStatusRequest = true;
	lightStatusRequstCount = 0;
	heartbeatTimer = Alarm.timerRepeat(HEARTBEAT_INTERVAL, sendHeartbeat);
	thingspeakMessage.setDestination(THINGSPEAK_NODE_ID);
	thingspeakMessage.setType(V_CUSTOM);
	thingspeakMessage.setSensor(WIFI_NODEMCU_ID);
}

void presentation()
{
	sendSketchInfo(APPLICATION_NAME, APPLICATION_VERSION);
	present(LIGHT_RELAY_ID, S_BINARY, "Staircase Light");
	wait(WAIT_50MS);
	send(lightRelayMessage.set(RELAY_OFF));
	send(thingspeakMessage.set(RELAY_OFF));
}

void loop()
{
	if (sendLightStatusRequest)
	{
		sendLightStatusRequest = false;
		request(BALCONY_LIGHT_RELAY_ID, V_STATUS, BALCONYLIGHT_WITH_PIR_NODE);
		lightStatusTimer = Alarm.timerOnce(ONE_MINUTE, checkLightStatusRequest);
		lightStatusRequstCount++;
		if (lightStatusRequstCount == 10)
		{
			lightStatusReceived = true;
			Alarm.free(lightStatusTimer);
			sendLightStatusRequest = false;
		}
	}
	Alarm.delay(1);
}

void receive(const MyMessage &message)
{
	if (message.type == V_STATUS)
	{
		if (message.getInt())
		{
			digitalWrite(LIGHT_RELAY_PIN, RELAY_ON);
			Alarm.delay(WAIT_50MS);
			send(lightRelayMessage.set(RELAY_ON));
			send(thingspeakMessage.set(RELAY_ON));
		}
		else
		{
			digitalWrite(LIGHT_RELAY_PIN, RELAY_OFF);
			Alarm.delay(WAIT_50MS);
			send(lightRelayMessage.set(RELAY_OFF));
			send(thingspeakMessage.set(RELAY_OFF));

		}
		if (!lightStatusReceived)
		{
			lightStatusReceived = true;
			Alarm.free(lightStatusTimer);
			sendLightStatusRequest = false;
		}
	}
}

void checkLightStatusRequest()
{
	if (!lightStatusReceived)
		sendLightStatusRequest = true;
}