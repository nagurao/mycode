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

AlarmId heartbeatTimer;
AlarmId hourlyTimer;
AlarmId refreshTimer;
AlarmId midnightTimer;
MyMessage hrMessage(1, V_CUSTOM);
MyMessage statMessage(2, V_CUSTOM);

byte hr;
byte min ;
byte sec ;

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
	present(1, S_CUSTOM, "Hour Data");
	present(2, S_CUSTOM, "Message");
	requestTime();
}

void loop()
{
	Alarm.delay(1);
}

void receiveTime(unsigned long controllerTime)
{
	send(statMessage.set("recTime"));
	hr = hour(controllerTime);
	min = minute(controllerTime);
	sec = second(controllerTime);
	send(hrMessage.set(hr));
	Alarm.free(refreshTimer);
	Alarm.free(hourlyTimer);
	Alarm.free(midnightTimer);
	unsigned int secondsBeforeNextHour = (60 - sec) + ((60 - min - 1) * 60);
	Alarm.timerOnce(secondsBeforeNextHour, createHourlyTimer);
	unsigned int secondsForRefreshTimer = secondsBeforeNextHour + (ONE_HOUR * ((hr + 1) % 3));
	Alarm.timerOnce(secondsForRefreshTimer, createRefreshTimer);
	unsigned int secondsForMidnight = ((((24 - hr - 1) * 60) + (60 - min - 1)) * 60) + (60 - sec);
	Alarm.timerOnce(secondsForMidnight, createMidnightTimer);
}

void createHourlyTimer()
{
	send(statMessage.set("CRTHRLYTMR"));
	hourlyTimer = Alarm.timerRepeat(ONE_HOUR, resetWattHour);
	hr = (hr + 1) % 24;
}

void createRefreshTimer()
{
	send(statMessage.set("CRTREFRTMR"));
	
	refreshTimer = Alarm.timerRepeat(ONE_HOUR * 3, requestTime);
}

void createMidnightTimer()
{
	send(statMessage.set("CRTMIDTMR"));
	
	midnightTimer = Alarm.timerRepeat(ONE_HOUR * 24, requestTime);
}

void resetWattHour()
{
	send(statMessage.set("RSTWTHR"));
	send(hrMessage.set(hr));
	hr = (hr + 1) % 24;
}

void resetWattDay()
{
	send(statMessage.set("RSTWTDAY"));
}
