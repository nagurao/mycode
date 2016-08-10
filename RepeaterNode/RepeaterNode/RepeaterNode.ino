
#include <TimeAlarms.h>
#include <TimeLib.h>
#include <Time.h>
#define MY_RADIO_NRF24
#define MY_REPEATER_FEATURE
#define MY_NODE_ID 10
#include <SPI.h>
#include <MySensors.h>
#include <MyNodes.h>
#include <MyConfig.h>

#define APPLICATION_NAME "Repeater Node 10"
#define APPLICATION_VERSION "10Aug2016"

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

}