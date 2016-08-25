#include <TimeAlarms.h>
#include <TimeLib.h>
#include <Time.h>
#include <SPI.h>

#define BOREWELL_NODE
#define NODE_HAS_RELAY

#define MY_RADIO_NRF24
#define MY_REPEATER_FEATURE
#define MY_NODE_ID BOREWELL_RELAY_NODE_ID
#define MY_DEBUG 

#include <MyNodes.h>
#include <MySensors.h>
#include <MyConfig.h>

#define APPLICATION_NAME "Borewell Motor"
#define APPLICATION_VERSION "24Aug2016"

AlarmId heartbeatTimer;

MyMessage thingspeakMessage(WIFI_NODEMCU_ID, V_CUSTOM);
MyMessage borewellMotorMessage(BOREWELL_MOTOR_ID, V_STATUS);
MyMessage borewellMotorOnRelayMessage(BORE_ON_RELAY_ID, V_STATUS);
MyMessage borewellMotorOffRelayMessage(BORE_OFF_RELAY_ID, V_STATUS);

void before()
{
	pinMode(BORE_ON_RELAY_PIN, OUTPUT);
	pinMode(BORE_OFF_RELAY_PIN, OUTPUT);
}

void setup()
{
	digitalWrite(BORE_ON_RELAY_PIN, LOW);
	digitalWrite(BORE_OFF_RELAY_PIN, LOW);
	thingspeakMessage.setDestination(THINGSPEAK_NODE_ID);
	thingspeakMessage.setType(V_CUSTOM);
	thingspeakMessage.setSensor(WIFI_NODEMCU_ID);
	heartbeatTimer = Alarm.timerRepeat(HEARTBEAT_INTERVAL, sendHeartbeat);
}

void presentation()
{
	sendSketchInfo(APPLICATION_NAME, APPLICATION_VERSION);
	present(BOREWELL_MOTOR_ID, S_BINARY, "Borewell Motor");
	present(BORE_ON_RELAY_ID, S_BINARY, "Bore On Relay");
	present(BORE_OFF_RELAY_ID, S_BINARY, "Bore Off Relay");
	send(borewellMotorMessage.set(RELAY_OFF));
	send(borewellMotorOnRelayMessage.set(RELAY_OFF));
	send(borewellMotorOffRelayMessage.set(RELAY_OFF));
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
		switch (message.sensor)
		{
		case BORE_ON_RELAY_ID:
			digitalWrite(BORE_ON_RELAY_PIN, RELAY_ON);
			send(borewellMotorOnRelayMessage.set(RELAY_ON));
			Alarm.timerOnce(RELAY_TRIGGER_INTERVAL, toggleOnRelay);
			break;
		case BORE_OFF_RELAY_ID:
			digitalWrite(BORE_OFF_RELAY_PIN, RELAY_ON);
			send(borewellMotorOffRelayMessage.set(RELAY_ON));
			Alarm.timerOnce(RELAY_TRIGGER_INTERVAL, toggleOffRelay);
			break;
		}
	}
}

void toggleOnRelay()
{
	digitalWrite(BORE_ON_RELAY_PIN, RELAY_OFF);
	send(borewellMotorOnRelayMessage.set(RELAY_OFF));
	send(borewellMotorMessage.set(RELAY_ON));
	send(thingspeakMessage.set(RELAY_ON));
}

void toggleOffRelay()
{
	digitalWrite(BORE_OFF_RELAY_PIN, RELAY_OFF);
	send(borewellMotorOffRelayMessage.set(RELAY_OFF));
	send(borewellMotorMessage.set(RELAY_OFF));
	send(thingspeakMessage.set(RELAY_OFF));
}