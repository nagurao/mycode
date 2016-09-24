#include <SPI.h>
#include <TimeAlarms.h>
#include <TimeLib.h>
#include <Time.h>

#define SUMP_MOTOR_NODE
#define NODE_HAS_RELAY

#define MY_RADIO_NRF24
#define MY_REPEATER_FEATURE
#define MY_NODE_ID SUMP_RELAY_NODE_ID
#define MY_DEBUG 

#include <MyNodes.h>
#include <MySensors.h>
#include <MyConfig.h>

#define APPLICATION_NAME "Sump Motor"
#define APPLICATION_VERSION "25Sep2016"

AlarmId heartbeatTimer;

MyMessage thingspeakMessage(WIFI_NODEMCU_ID, V_CUSTOM);
MyMessage sumpMotorRelayMessage(SUMP_RELAY_ID, V_STATUS);
void before()
{
	pinMode(SUMP_RELAY_PIN, OUTPUT);
}

void setup()
{
	digitalWrite(SUMP_RELAY_PIN, LOW);
	thingspeakMessage.setDestination(THINGSPEAK_NODE_ID);
	thingspeakMessage.setType(V_CUSTOM);
	thingspeakMessage.setSensor(WIFI_NODEMCU_ID);
	heartbeatTimer = Alarm.timerRepeat(HEARTBEAT_INTERVAL, sendHeartbeat);
}

void presentation()
{
	sendSketchInfo(APPLICATION_NAME, APPLICATION_VERSION);
	present(SUMP_RELAY_ID, S_BINARY, "Sump Motor Relay");
	send(sumpMotorRelayMessage.set(RELAY_OFF));
	send(thingspeakMessage.set(RELAY_OFF));
}

void loop()
{
	Alarm.delay(1);
}

void receive(const MyMessage &message)
{
	if (message.type == V_STATUS)
	{
		digitalWrite(SUMP_RELAY_PIN, message.getInt() ? RELAY_ON : RELAY_OFF);
		send(sumpMotorRelayMessage.set(message.getInt()));
		Alarm.delay(WAIT_10MS);
		send(thingspeakMessage.set(message.getInt()));
	}
}