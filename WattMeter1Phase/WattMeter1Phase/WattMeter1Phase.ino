#include <TimeAlarms.h>
#include <TimeLib.h>
#include <Time.h>
#include <SPI.h>

#define WATT_METER_NODE

#define MY_RADIO_NRF24
#define MY_REPEATER_FEATURE
#define MY_NODE_ID PH1_NODE_ID
#define MY_DEBUG

#include <MyNodes.h>
#include <MySensors.h>
#include <MyConfig.h>

#define APPLICATION_NAME "1Phase Watt Meter"
#define APPLICATION_VERSION "06Sep2016"

AlarmId heartbeatTimer;

void before()
{
	attachInterrupt(INTERRUPT_PULSE, onPulse, RISING);
}

void setup()
{
	heartbeatTimer = Alarm.timerRepeat(HEARTBEAT_INTERVAL, sendHeartbeat);
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
	MyMessage resetTypeMessage()
}

void loop()
{
	Alarm.delay(1);
}
void receive(const MyMessage &message)
{

}

void onPulse()
{

}