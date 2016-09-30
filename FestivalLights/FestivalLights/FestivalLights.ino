#include <TimeAlarms.h>
#include <Time.h>
#include <SPI.h>

#define NODE_HAS_RELAY

#define MY_RADIO_NRF24
#define MY_REPEATER_FEATURE
#define MY_NODE_ID 200
#define MY_DEBUG 

#include <MyNodes.h>
#include <MySensors.h>
#include <MyConfig.h>

#define APPLICATION_NAME "Festival Lights"
#define APPLICATION_VERSION "01Oct2016"

#define RELAY_PIN 7
#define RELAY_ID 1

boolean relayStatusReceived;
boolean sendRelayStatusRequest;
byte relayStatusRequstCount;
AlarmId relayStatusTimer;
AlarmId heartbeatTimer;

MyMessage relayMessage(RELAY_ID, V_STATUS);


void before()
{
	pinMode(RELAY_PIN, OUTPUT);
}

void setup()
{
	digitalWrite(RELAY_PIN, LOW);
	relayStatusReceived = false;
	sendRelayStatusRequest = true;
	relayStatusRequstCount = 0;
	heartbeatTimer = Alarm.timerRepeat(HEARTBEAT_INTERVAL, sendHeartbeat);
}

void presentation()
{
	sendSketchInfo(APPLICATION_NAME, APPLICATION_VERSION);
	present(RELAY_ID, S_BINARY, "Festival Lights");
	wait(WAIT_50MS);
	send(relayMessage.set(RELAY_OFF));
}

void loop()
{
	if (sendRelayStatusRequest)
	{
		sendRelayStatusRequest = false;
		request(RELAY_ID, V_STATUS);
		relayStatusTimer = Alarm.timerOnce(REQUEST_INTERVAL, checkRelayStatusRequest);
		relayStatusRequstCount++;
		if (relayStatusRequstCount == 10)
		{
			relayStatusReceived = true;
			Alarm.free(relayStatusTimer);
			sendRelayStatusRequest = false;
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
			digitalWrite(RELAY_PIN, RELAY_ON);
			Alarm.delay(WAIT_50MS);
			send(relayMessage.set(RELAY_ON));
		}
		else
		{
			digitalWrite(RELAY_PIN, RELAY_OFF);
			Alarm.delay(WAIT_50MS);
			send(relayMessage.set(RELAY_OFF));
		}
		if (!relayStatusReceived)
		{
			relayStatusReceived = true;
			Alarm.free(relayStatusTimer);
			sendRelayStatusRequest = false;
		}
	}
}

void checkRelayStatusRequest()
{
	if (!relayStatusReceived)
		sendRelayStatusRequest = true;
}