#include <TimeAlarms.h>
#include <TimeLib.h>
#include <Time.h>
#include <SPI.h>

#define NODE_TYPE

#define MY_RADIO_NRF24
#define MY_REPEATER_FEATURE
#define MY_NODE_ID NODE_ID_FROM_MYNODES_HEADERFILE
#define MY_DEBUG

#include <MySensors.h>
#include <MyNodes.h>
#include <MyConfig.h>

#define APPLICATION_NAME "Appl_Name"
#define APPLICATION_VERSION "Appl_Date"

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