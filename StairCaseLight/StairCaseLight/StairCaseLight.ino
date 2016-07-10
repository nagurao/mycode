#include <TimeAlarms.h>
#include <Time.h>
#include <SPI.h>

#define NODE_HAS_RELAY
#define STAIRCASE_NODE

#define MY_RADIO_NRF24
#define MY_REPEATER_FEATURE
#define MY_NODE_ID STAIRCASE_NODE_ID
#define MY_DEBUG 

#include <MyNodes.h>
#include <MySensors.h>
#include <MyConfig.h>

#define APPLICATION_NAME "Staircase Light"
#define APPLICATION_VERSION "10Jul2016"

#define LIGHT_RELAY_PIN 7
#define LIGHT_RELAY_ID 1
#define ONE_MINUTE 60

boolean lightStatusReceived;
boolean sendLightStatusRequest;
AlarmId lightStatusTimer;

MyMessage lightRelayMessage(LIGHT_RELAY_ID, V_STATUS);

void before()
{
	pinMode(LIGHT_RELAY_PIN, OUTPUT);
}

void setup()
{
	digitalWrite(LIGHT_RELAY_PIN, LOW);
	lightStatusReceived = false;
	sendLightStatusRequest = true;
}

void presentation()
{
	sendSketchInfo(APPLICATION_NAME, APPLICATION_VERSION);
	present(LIGHT_RELAY_ID, S_BINARY, "Staircase Light");
	wait(WAIT_50MS);
	send(lightRelayMessage.set(RELAY_OFF));
}

void loop()
{
	if (sendLightStatusRequest)
	{
		sendLightStatusRequest = false;
		lightStatusTimer = Alarm.timerOnce(ONE_MINUTE, checkLightStatusRequest);
		request(2, V_STATUS, 200);
	}

}

void receive(const MyMessage &message)
{
	if (message.type == V_STATUS)
	{
		if (message.getBool())
		{
			digitalWrite(LIGHT_RELAY_ID, RELAY_ON);
			send(lightRelayMessage.set(RELAY_ON));
		}
		else
		{
			digitalWrite(LIGHT_RELAY_ID, RELAY_OFF);
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

void checkLightStatusRequest()
{
	if (!lightStatusReceived)
		sendLightStatusRequest = true;
}