#include <TimeAlarms.h>
#include <TimeLib.h>
#include <Time.h>
#include <SPI.h>

#define WATT_METER_NODE

#define MY_RADIO_NRF24
#define MY_REPEATER_FEATURE
//#define MY_NODE_ID PH3_NODE_ID
#define MY_NODE_ID 200
#define MY_DEBUG

#include <MyNodes.h>
#include <MySensors.h>
#include <MyConfig.h>

#define APPLICATION_NAME "3Phase Watt Meter"
#define APPLICATION_VERSION "06Sep2016"

AlarmId heartbeatTimer;
AlarmId pulseCountTimer;
AlarmId pulsesPerWattHourTimer;
AlarmId updateConsumptionTimer;

boolean sendPulseCountRequest;
boolean pulseCountReceived;
byte pulseCountRequestCount;

boolean sendPulsesPerWattHourRequest;
boolean pulsesPerWattHourReceived;
byte pulsesPerWattHourCount;


volatile unsigned long currPulseCount;
volatile unsigned long prevPulseCount;
volatile unsigned long currWatt;
volatile unsigned long prevWatt;
volatile unsigned long lastBlink;
double pulsesPerWattHour;
double pulseFactor;

void before()
{
	attachInterrupt(INTERRUPT_PULSE, onPulse, RISING);
}

void setup()
{
	heartbeatTimer = Alarm.timerRepeat(HEARTBEAT_INTERVAL, sendHeartbeat);
	sendPulseCountRequest = true;
	pulseCountReceived = false;
	pulseCountRequestCount = 0;
	sendPulsesPerWattHourRequest = true;
	pulsesPerWattHourReceived = false;
	pulsesPerWattHourCount = 0;
	lastBlink = 0;
	prevWatt = 0;
	currWatt = 0;
	pulseFactor = 0;
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
	present(DELTA_WATT_CONSUMPTION_ID, S_POWER, "Delta Consumption");
	Alarm.delay(WAIT_10MS);
	present(CURR_PULSE_COUNT_ID, S_CUSTOM, "Pulse Count");
	Alarm.delay(WAIT_10MS);
	present(PULSE_PER_KWH_ID, S_CUSTOM, "Pulses per KWH");
	Alarm.delay(WAIT_10MS);
	present(RESET_TYPE_ID, S_CUSTOM, "Reset Consumption");
	MyMessage resetTypeMessage(RESET_TYPE_ID, V_VAR3);
	send(resetTypeMessage.set(RESET_NONE));
}

void loop()
{
	if (sendPulseCountRequest)
	{
		sendPulseCountRequest = false;
		request(CURR_PULSE_COUNT_ID, V_VAR1);
		pulseCountTimer = Alarm.timerOnce(ONE_MINUTE, checkPulseCountRequestStatus);
		pulseCountRequestCount++;
		if (pulseCountRequestCount == 10)
		{
			MyMessage pulseCountMessage(CURR_PULSE_COUNT_ID, V_VAR1);
			send(pulseCountMessage.set(ZERO_PULSE));
		}
	}

	if (sendPulsesPerWattHourRequest)
	{
		sendPulsesPerWattHourRequest = false;
		request(PULSE_PER_KWH_ID, V_VAR2);
		pulsesPerWattHourTimer = Alarm.timerOnce(ONE_MINUTE, checkPulsesPerWattHourRequest);
		pulsesPerWattHourCount++;
		if (pulsesPerWattHourCount == 10)
		{
			MyMessage pulsesPerWattHourMessage(CURR_PULSE_COUNT_ID, V_VAR2);
			send(pulsesPerWattHourMessage.set(6400));
		}
	}
	Alarm.delay(1);
}
void receive(const MyMessage &message)
{
	if (message.sender == PH1_NODE_ID)
	{
		return;
	}
	switch (message.type)
	{
	case V_VAR1:
		currPulseCount = prevPulseCount = message.getLong();
		if (!pulseCountReceived)
		{
			pulseCountReceived = true;
			Alarm.free(pulseCountTimer);
			updateConsumptionTimer = Alarm.timerRepeat(ACCUMULATION_FREQUENCY_SECS, updateConsumptionData);
		}
		break;
	case V_VAR2:
		if (!pulsesPerWattHourReceived)
		{
			pulsesPerWattHourReceived = true;
			Alarm.free(pulsesPerWattHourTimer);
		}
		pulseFactor = message.getLong();
		pulsesPerWattHour = pulseFactor / 1000;
		break;
	case V_VAR3:
		switch (message.getInt())
		{
		case 0:
			break;
		case 1:
			resetHour();
			break;
		case 2:
			resetDay();
			break;
		case 3:
			resetMonth();
			break;
		case 4:
			resetAll();
			break;
		}
		break;
	}

}
void onPulse()
{
	unsigned long newBlink = micros();
	unsigned long interval = newBlink - lastBlink;
	if (interval < 10000L)
	{
		return;
	}
	currWatt = (3600000000.0 / interval) / pulsesPerWattHour;
	lastBlink = newBlink;
	currPulseCount++;
}

void updateConsumptionData()
{
	if (currWatt != prevWatt)
	{
		MyMessage currentConsumptionMessage(CURR_WATT_ID, V_WATT);
		if(currWatt < MAX_WATT)
			send(currentConsumptionMessage.set(currWatt));
		prevWatt = currWatt;
	}
	if (currPulseCount != prevPulseCount)
	{
		MyMessage currentPulseCountMessage(CURR_PULSE_COUNT_ID, V_VAR1);
		send(currentPulseCountMessage.set(currPulseCount));
		prevPulseCount = currPulseCount;
		double accumulatedKWH = ((double)currPulseCount / ((double)))
	}
}

void resetHour()
{

}

void resetDay()
{

}

void resetMonth()
{

}

void resetAll()
{

}

void checkPulseCountRequestStatus()
{
	if (!pulseCountReceived)
		sendPulseCountRequest = true;
}

void checkPulsesPerWattHourRequest()
{
	if (!pulsesPerWattHourReceived)
		sendPulsesPerWattHourRequest = true;
}