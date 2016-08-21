#include <TimeAlarms.h>
#include <TimeLib.h>
#include <Time.h>

#define MY_RADIO_NRF24
#define MY_REPEATER_FEATURE

#include <MyNodes.h>
#include <MySensors.h>
#include <MyConfig.h>
#include <SPI.h>

#define APPLICATION_NAME "Default MYS Firmware"
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
}
void loop()
{

}