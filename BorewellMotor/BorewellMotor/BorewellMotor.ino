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
#define APPLICATION_VERSION "21Aug2016"

AlarmId heartbeatTimer;

MyMessage thingspeakMessage(WIFI_NODEMCU_ID, V_CUSTOM);
MyMessage borewellMotorOnRelayMessage(BORE_ON_RELAY_SENSOR_ID, V_STATUS);
MyMessage borewellMotorOffRelayMessage(BORE_OFF_RELAY_SENSOR_ID, V_STATUS);

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
}

void presentation()
{
	sendSketchInfo(APPLICATION_NAME, APPLICATION_VERSION);
	present(BORE_ON_RELAY_SENSOR_ID, S_BINARY, "Bore On Relay");
	present(BORE_OFF_RELAY_SENSOR_ID, S_BINARY, "Bore Off Relay");
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

}
