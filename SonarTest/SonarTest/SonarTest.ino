#include <NewPing.h>
#include <SPI.h>
#include <TimeAlarms.h>
#include <TimeLib.h>
#include <Time.h>

#define NODE_HAS_ULTRASONIC_SENSOR

#define MY_RADIO_NRF24
#define MY_REPEATER_FEATURE
#define MY_NODE_ID OVERHEAD_TANK_02_NODE_ID
#define MY_DEBUG

#include <MyNodes.h>
#include <MySensors.h>
#include <MyConfig.h>

#define APPLICATION_NAME "Overhead Tank 02"
#define APPLICATION_VERSION "06Aug2016"

#define DEFAULT_POLL_INTERVAL 120
AlarmId heartbeatTimer;
AlarmId defaultPollTimer;
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

MyMessage waterLevelMessage(WATER_LEVEL_SENSOR_ID, V_DISTANCE);
MyMessage thingspeakMessage(WIFI_NODEMCU_ID, V_CUSTOM);

void before()
{

}

void setup()
{
	thingspeakMessage.setDestination(THINGSPEAK_NODE_ID);
	thingspeakMessage.setType(V_CUSTOM);
	thingspeakMessage.setSensor(WIFI_NODEMCU_ID);
	defaultPollTimer = Alarm.timerRepeat(DEFAULT_POLL_INTERVAL, sendWaterLevel);
	heartbeatTimer = Alarm.timerRepeat(HEARTBEAT_INTERVAL, sendHeartbeat);
}

void presentation()
{
	sendSketchInfo(APPLICATION_NAME, APPLICATION_VERSION);
	present(WATER_LEVEL_SENSOR_ID, S_DISTANCE, "Water Level");
	wait(WAIT_50MS);
}

void loop()
{
	Alarm.delay(1);
}
void receive(const MyMessage &message)
{

}

void sendWaterLevel()
{
	int sonarDistance = sonar.ping_cm();
	send(waterLevelMessage.set(sonarDistance));
	send(thingspeakMessage.set(sonarDistance));
}