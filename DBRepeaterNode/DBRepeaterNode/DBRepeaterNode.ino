#include <TimeAlarms.h>
#include <TimeLib.h>
#include <Time.h>
#include <SPI.h>

#define REPEATER_NODE

#define MY_RADIO_NRF24
#define MY_REPEATER_FEATURE
#define MY_NODE_ID DB_REPEATER_NODE_ID
#define MY_DEBUG

#include <MyNodes.h>
#include <MySensors.h>
#include <MyConfig.h>

#define APPLICATION_NAME "Repeater Node"
#define APPLICATION_VERSION "13Dec2016"

AlarmId heartbeatTimer;

void before()
{

}

void setup()
{
	heartbeatTimer = Alarm.timerRepeat(HEARTBEAT_INTERVAL, sendHeartbeat);
}

void presentation()
{
	sendSketchInfo(APPLICATION_NAME, APPLICATION_VERSION);
}

void loop()
{
	Alarm.delay(1);
}
void receive(const MyMessage &message)
{

}