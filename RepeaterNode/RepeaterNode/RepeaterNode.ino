#include <TimeAlarms.h>
#include <TimeLib.h>
#include <Time.h>
#define MY_RADIO_NRF24
#define MY_REPEATER_FEATURE
#define MY_NODE_ID 6
#include <SPI.h>
#include <MySensors.h>
#include <MyNodes.h>
#include <MyConfig.h>

#define APPLICATION_NAME "Testing MYS 6"
#define APPLICATION_VERSION "20Aug2016"

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
	present(1, S_BINARY, "Light Relay");
	present(2, S_BINARY, "Light Relay");
	present(3, S_BINARY, "Light Relay");
	present(4, S_BINARY, "Light Relay");
}

void loop()
{

}