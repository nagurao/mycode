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

MyMessage rstMsg(1, V_CUSTOM);
MyMessage msg(2, V_CUSTOM);

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
	present(1, S_CUSTOM, "Reset");
	present(2, S_CUSTOM, "Message");
	
}

void loop()
{
	Alarm.delay(1);
}

void receive(const MyMessage &message)
{
	if (message.type == V_VAR1)
	{
		switch (message.getInt())
		{
		case 1: resetWattHour(); break;
		case 2: resetWattDay(); break;
		case 3: resetWattMonth(); break;
		}
		send(rstMsg.set(message.getInt()));
	}
}

/*
void receiveTime(unsigned long controllerTime)
{

	/*send(statMessage.set("recTime"));
	hr = hour(controllerTime);
	min = minute(controllerTime);
	sec = second(controllerTime);
	send(hrMessage.set(hr));
	Alarm.free(hourlyTimer);
	Alarm.free(midnightTimer);
	Alarm.free(refreshTimeTimer);
	unsigned int secondsForNextHour = (60 - sec) + ((59 - min) * 60);
	Alarm.timerOnce(secondsForNextHour, createHourlyTimer);
	send(statMessage.set(secondsForNextHour));
	unsigned int secondsForMidnight = (60 - sec) + ((23 - hr) * 60 + (59 - min)) * 60;
	midnightTimer = Alarm.timerOnce(secondsForMidnight, resetWattDay);
	refreshTimeTimer = Alarm.timerOnce(secondsForMidnight + ONE_HOUR, refreshNodeTime);
	send(statMessage.set(secondsForMidnight));
	
	setTime(controllerTime);
}
*/

/*void createHourlyTimer()
{
	Serial.println("CRTHRLYTMR");
	send(statMessage.set("CRTHRLYTMR"));
	hourlyTimer = Alarm.timerRepeat(ONE_HOUR, resetWattHour);
	hr = (hr + 1) % 24;
} */

/*void refreshNodeTime()
{
	send(statMessage.set("REFRTIME"));
	Serial.println("REFRTIME");
	requestTime();
}*/

void resetWattHour()
{
	send(msg.set("RSTWTHR"));
	Serial.println("RSTWTHR");
}

void resetWattDay()
{
	send(msg.set("RSTWTDAY"));
	Serial.println("RSTWTDAY");
}

void resetWattMonth()
{
	send(msg.set("RSTWTMON"));
	Serial.println("RSTWTMON");
}
