#include <TimeAlarms.h>
#include <TimeLib.h>
#include <Time.h>
#include <SPI.h>

#define PHASE3_METER
#define WATT_METER_NODE

#define MY_RADIO_NRF24
#define MY_REPEATER_FEATURE
#define MY_NODE_ID PH3_NODE_ID
#define MY_DEBUG 

#include <MyNodes.h>
#include <MySensors.h>
#include <MyConfig.h>

#define APPLICATION_NAME "3Phase WattMeter"
#define APPLICATION_VERSION "30Aug2016"

volatile unsigned long pulseCount;
volatile unsigned long lastBlink;

boolean pulseCountRecived;
boolean sendPulseCountRequest;
byte sendPulseCountRequestCount;

AlarmId pulseCountTimer;
AlarmId heartbeatTimer;
AlarmId hourlyTimer;

MyMessage currWattMessage(CURR_WATT_ID, V_WATT);
MyMessage hourlyWattMessage(HOURLY_WATT_CONSUMPTION_ID, V_KWH);
MyMessage dailyWattMessage(DAILY_WATT_CONSUMPTION_ID, V_KWH);
MyMessage accumWattMessage(ACCUMULATED_WATT_CONSUMPTION_ID, V_KWH);
MyMessage pulseCountMessage(CURR_PULSE_COUNT_ID, V_VAR1);

void before()
{
	pulseCount = 0;
	lastBlink = 0;
	attachInterrupt(INTERRUPT_PULSE, onPulse, RISING);
}

void setup()
{
	heartbeatTimer = Alarm.timerRepeat(HEARTBEAT_INTERVAL, sendHeartbeat);
	sendPulseCountRequestCount = 0;
	sendPulseCountRequest = true;
	pulseCountRecived = false;
}

void presentation()
{
	sendSketchInfo(APPLICATION_NAME, APPLICATION_VERSION);
	present(CURR_WATT_ID, S_POWER, "Current Consumption");
	Alarm.delay(WAIT_10MS);
	present(HOURLY_WATT_CONSUMPTION_ID, S_POWER, "Hourly Consumption");
	Alarm.delay(WAIT_10MS);
	present(DAILY_WATT_CONSUMPTION_ID, S_POWER, "Daily Consumption");
	Alarm.delay(WAIT_10MS);
	present(ACCUMULATED_WATT_CONSUMPTION_ID, S_POWER, "Total Consumption");
	Alarm.delay(WAIT_10MS);
	present(CURR_PULSE_COUNT_ID, S_CUSTOM, "Pulse Count");
	requestTime();
}

void loop()
{
	if (sendPulseCountRequest)
	{
		sendPulseCountRequest = false;
		request(CURR_PULSE_COUNT_ID, V_VAR1);
		pulseCountTimer = Alarm.timerOnce(ONE_MINUTE, checkPulseCountRequest);
		sendPulseCountRequestCount++;
		if (sendPulseCountRequestCount == 10)
		{
			send(pulseCountMessage.set(pulseCount));
			pulseCount = 0;
		}
	}
	Alarm.delay(1);
}

void receive(const MyMessage &message)
{
	if (message.type == V_VAR1)
	{
		if (pulseCountRecived)
		{
			pulseCount = pulseCount + message.getLong();
		}
		else
		{
			pulseCount = pulseCount + message.getLong();
			send(pulseCountMessage.set(pulseCount));
			pulseCount = 0;
			pulseCountRecived = true;
			Alarm.free(pulseCountTimer);
			sendPulseCountRequest = false;
			request(CURR_PULSE_COUNT_ID, V_VAR1);
		}
	}
}
void checkPulseCountRequest()
{
	if (!pulseCountRecived)
		sendPulseCountRequest = true;
}

void onPulse()
{

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