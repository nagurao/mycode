#include <TimeAlarms.h>
#include <TimeLib.h>
#include <Time.h>
#include <SPI.h>

#define PHASE3_METER

#define MY_RADIO_NRF24
#define MY_REPEATER_FEATURE
#define MY_NODE_ID PH3_NODE_ID
#define MY_DEBUG 

#include <MyNodes.h>
#include <MySensors.h>
#include <MyConfig.h>

#define APPLICATION_NAME "3Phase WattMeter"
#define APPLICATION_VERSION "30Aug2016"

AlarmId heartbeatTimer;
AlarmId hourlyTimer;

MyMessage currWattMessage(CURR_WATT_ID, V_WATT);
MyMessage hourlyWattMessage(HOURLY_WATT_CONSUMPTION_ID, V_KWH);
MyMessage dailyWattMessage(DAILY_WATT_CONSUMPTION_ID, V_KWH);
MyMessage accumWattMessage(ACCUMULATED_WATT_CONSUMPTION_ID, V_KWH);
MyMessage pulseCountMessage(CURR_PULSE_COUNT_ID, V_VAR1);

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
	byte date = day(controllerTime);
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