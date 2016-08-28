#include <TimeAlarms.h>
#include <TimeLib.h>
#include <Time.h>
#include <SPI.h>

#define MY_RADIO_NRF24
#define MY_REPEATER_FEATURE
#define MY_NODE_ID 253
#define MY_DEBUG 

#include <MyNodes.h>
#include <MySensors.h>
#include <MyConfig.h>

#define APPLICATION_NAME "WattMeter Test"
#define APPLICATION_VERSION "28Aug2016"

#define ONE_HOUR 3600
AlarmId heartbeatTimer;
AlarmId hourlyTimer;

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
	requestTime();
}

void loop()
{
	Alarm.delay(1);
}

void receiveTime(unsigned long controllerTime)
{
	byte hr = hour(controllerTime);
	byte min = minute(controllerTime);
	byte sec = second(controllerTime);
	unsigned int secondsBeforeNextHour = (60 - sec) + ((60 - min) * 60);
	Alarm.timerOnce(secondsBeforeNextHour, createHourlyTimer);
	Serial.print("The time received is :");
	Serial.print(hr); Serial.print(":"); Serial.print(min); Serial.print(":"); Serial.println(sec);
}

void createHourlyTimer()
{
	hourlyTimer = Alarm.timerRepeat(ONE_HOUR, resetWattHour);
}

void resetWattHour()
{
	Serial.println("Watt Resetted");
}