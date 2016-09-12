#include <TimeAlarms.h>
#include <TimeLib.h>
#include <Time.h>
#include <SPI.h>

#define WATT_METER_NODE

#define MY_RADIO_NRF24
#define MY_REPEATER_FEATURE
//#define MY_NODE_ID PH1_NODE_ID
#define MY_NODE_ID 201
#define MY_DEBUG

#include <MyNodes.h>
#include <MySensors.h>
#include <MyConfig.h>

#define APPLICATION_NAME "1Phase Watt Meter"
#define APPLICATION_VERSION "13Sep2016"

#define DEFAULT_BLINKS_PER_KWH 6400 // value from energy meter
AlarmId heartbeatTimer;
AlarmId pulseCountTimer;
AlarmId pulsesPerWattHourTimer;
AlarmId updateConsumptionTimer;
AlarmId accumulationTimer;

boolean sendPulseCountRequest;
boolean pulseCountReceived;
byte pulseCountRequestCount;

boolean sendBlinksPerWattHourRequest;
boolean blinksPerWattHourReceived;
byte blinksPerWattHourCount;
double blinksPerWattHour;
double pulseFactor;

volatile unsigned long currPulseCount;
volatile unsigned long prevPulseCount;
volatile unsigned long currWatt;
volatile unsigned long prevWatt;
volatile unsigned long lastBlink;

double accumulatedKWH;
double hourlyConsumptionInitKWH;
double dailyConsumptionInitKWH;
double monthlyConsumptionInitKWH;

byte accumulationsStatus;
byte accumulationStatusCount;
boolean firstTime;

MyMessage currentConsumptionMessage(CURR_WATT_ID, V_WATT);
MyMessage hourlyConsumptionMessage(HOURLY_WATT_CONSUMPTION_ID, V_KWH);
MyMessage dailyConsumptionMessage(DAILY_WATT_CONSUMPTION_ID, V_KWH);
MyMessage monthlyConsumptionMessage(MONTHLY_WATT_CONSUMPTION_ID, V_KWH);
MyMessage accumulatedKWMessage(ACCUMULATED_WATT_CONSUMPTION_ID, V_KWH);
MyMessage deltaConsumptionMessage(DELTA_WATT_CONSUMPTION_ID, V_KWH);
MyMessage pulseCountMessage(CURR_PULSE_COUNT_ID, V_VAR1);
MyMessage thingspeakMessage(WIFI_NODEMCU_ID, V_CUSTOM);

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
	sendBlinksPerWattHourRequest = true;
	blinksPerWattHourReceived = false;
	blinksPerWattHourCount = 0;
	lastBlink = 0;
	prevWatt = 0;
	currWatt = 0;
	pulseFactor = 0;
	accumulatedKWH = 0;
	hourlyConsumptionInitKWH = 0;
	dailyConsumptionInitKWH = 0;
	monthlyConsumptionInitKWH = 0;
	accumulationsStatus = GET_HOURLY_KWH;
	accumulationStatusCount = 0;
	firstTime = true;
	thingspeakMessage.setDestination(THINGSPEAK_NODE_ID);
	thingspeakMessage.setType(V_CUSTOM);
	thingspeakMessage.setSensor(WIFI_NODEMCU_ID);
	deltaConsumptionMessage.setDestination(PH3_NODE_ID);
	deltaConsumptionMessage.setType(V_KWH);
	deltaConsumptionMessage.setSensor(DELTA_WATT_CONSUMPTION_ID);
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
	present(MONTHLY_WATT_CONSUMPTION_ID, S_POWER, "Monthly Consumption");
	Alarm.delay(WAIT_10MS);
	present(ACCUMULATED_WATT_CONSUMPTION_ID, S_POWER, "Total Consumption");
	Alarm.delay(WAIT_10MS);
	present(CURR_PULSE_COUNT_ID, S_CUSTOM, "Pulse Count");
	Alarm.delay(WAIT_10MS);
	present(BLINKS_PER_KWH_ID, S_CUSTOM, "Pulses per KWH");
	Alarm.delay(WAIT_10MS);
	present(RESET_TYPE_ID, S_CUSTOM, "Reset Consumption");
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
			send(pulseCountMessage.set(ZERO_PULSE));
		}
	}

	if (sendBlinksPerWattHourRequest)
	{
		sendBlinksPerWattHourRequest = false;
		request(BLINKS_PER_KWH_ID, V_VAR2);
		pulsesPerWattHourTimer = Alarm.timerOnce(ONE_MINUTE, checkBlinksPerWattHourRequest);
		blinksPerWattHourCount++;
		if (blinksPerWattHourCount == 10)
		{
			MyMessage blinksPerWattHourMessage(BLINKS_PER_KWH_ID, V_VAR2);
			send(blinksPerWattHourMessage.set(DEFAULT_BLINKS_PER_KWH));
		}
	}
	Alarm.delay(1);
}
void receive(const MyMessage &message)
{
	switch (message.type)
	{
	case V_VAR1:
		currPulseCount = currPulseCount + message.getLong();
		if (!pulseCountReceived)
		{
			pulseCountReceived = true;
			Alarm.free(pulseCountTimer);
			updateConsumptionTimer = Alarm.timerRepeat(ACCUMULATION_FREQUENCY_SECS, updateConsumptionData);
		}
		break;
	case V_VAR2:
		if (!blinksPerWattHourReceived)
		{
			blinksPerWattHourReceived = true;
			Alarm.free(pulsesPerWattHourTimer);
		}
		blinksPerWattHour = message.getLong();
		pulseFactor = blinksPerWattHour / 1000;
		break;
	case V_VAR3:
		switch (message.getInt())
		{
		case 0:
			switch (accumulationsStatus)
			{
			case GET_HOURLY_KWH:
				accumulationStatusCount++;
				if (accumulationStatusCount == 3)
					send(hourlyConsumptionMessage.set((double)ZERO, 4));
				request(HOURLY_WATT_CONSUMPTION_ID, V_KWH);
				break;
			case GET_DAILY_KWH:
				accumulationStatusCount++;
				if (accumulationStatusCount == 3)
					send(dailyConsumptionMessage.set((double)ZERO, 4));
				request(DAILY_WATT_CONSUMPTION_ID, V_KWH);
				break;
			case GET_MONTHLY_KWH:
				accumulationStatusCount++;
				if (accumulationStatusCount == 3)
					send(monthlyConsumptionMessage.set((double)ZERO, 4));
				request(MONTHLY_WATT_CONSUMPTION_ID, V_KWH);
				break;
			}
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
	case V_KWH:
		switch (message.sensor)
		{
		case HOURLY_WATT_CONSUMPTION_ID:
			hourlyConsumptionInitKWH = accumulatedKWH - message.getLong();
			accumulationsStatus = GET_DAILY_KWH;
			accumulationStatusCount = 0;
			break;
		case DAILY_WATT_CONSUMPTION_ID:
			dailyConsumptionInitKWH = accumulatedKWH - message.getLong();
			accumulationsStatus = GET_MONTHLY_KWH;
			accumulationStatusCount = 0;
			break;
		case MONTHLY_WATT_CONSUMPTION_ID:
			monthlyConsumptionInitKWH = accumulatedKWH - message.getLong();
			accumulationStatusCount = 0;
			accumulationsStatus = ALL_DONE;
			Alarm.free(accumulationTimer);
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
	currWatt = (3600000000.0 / interval) / pulseFactor;
	lastBlink = newBlink;
	currPulseCount++;
}

void updateConsumptionData()
{
	if (currWatt != prevWatt)
	{
		if (currWatt < MAX_WATT)
			send(currentConsumptionMessage.set(currWatt));
		prevWatt = currWatt;
	}
	if (currPulseCount != prevPulseCount)
	{
		send(pulseCountMessage.set(currPulseCount));
		prevPulseCount = currPulseCount;
		double currAccumulatedKWH = ((double)currPulseCount / ((double)blinksPerWattHour));
		if (currAccumulatedKWH != accumulatedKWH)
		{
			send(accumulatedKWMessage.set(currAccumulatedKWH, 4));
			accumulatedKWH = currAccumulatedKWH;
			if (firstTime)
			{
				MyMessage resetTypeMessage(RESET_TYPE_ID, V_VAR3);
				send(resetTypeMessage.set(RESET_NONE));
				firstTime = false;
				request(RESET_TYPE_ID, V_VAR3);
				accumulationTimer = Alarm.timerRepeat(ONE_MINUTE, getAccumulation);
			}
		}
		if (accumulationsStatus == ALL_DONE)
		{
			send(hourlyConsumptionMessage.set((accumulatedKWH - hourlyConsumptionInitKWH), 4));
			send(dailyConsumptionMessage.set((accumulatedKWH - dailyConsumptionInitKWH), 4));
			send(monthlyConsumptionMessage.set((accumulatedKWH - monthlyConsumptionInitKWH), 4));
		}
	}
}

void resetHour()
{
	double sendKHWValue = accumulatedKWH - hourlyConsumptionInitKWH;
	send(hourlyConsumptionMessage.set(sendKHWValue, 4));
	send(thingspeakMessage.set(sendKHWValue, 4));
	hourlyConsumptionInitKWH = accumulatedKWH;
	MyMessage resetTypeMessage(RESET_TYPE_ID, V_VAR3);
	send(resetTypeMessage.set(RESET_NONE));
}

void resetDay()
{
	double sendKHWValue = accumulatedKWH - dailyConsumptionInitKWH;
	send(dailyConsumptionMessage.set(sendKHWValue, 4));
	send(thingspeakMessage.set(sendKHWValue, 4));
	dailyConsumptionInitKWH = accumulatedKWH;

	double deltaKWH = accumulatedKWH - monthlyConsumptionInitKWH;
	send(deltaConsumptionMessage.set(deltaKWH, 4));
	MyMessage resetTypeMessage(RESET_TYPE_ID, V_VAR3);
	send(resetTypeMessage.set(RESET_NONE));
}

void resetMonth()
{
	double sendKHWValue = accumulatedKWH - monthlyConsumptionInitKWH;
	send(monthlyConsumptionMessage.set(sendKHWValue, 4));
	send(thingspeakMessage.set(sendKHWValue, 4));
	monthlyConsumptionInitKWH = accumulatedKWH;
	MyMessage resetTypeMessage(RESET_TYPE_ID, V_VAR3);
	send(resetTypeMessage.set(RESET_NONE));
}

void resetAll()
{
	send(accumulatedKWMessage.set((double)ZERO, 4));
	send(pulseCountMessage.set(ZERO_PULSE));
	request(CURR_PULSE_COUNT_ID, V_VAR1);

	send(hourlyConsumptionMessage.set((double)ZERO, 4));
	send(dailyConsumptionMessage.set((double)ZERO, 4));
	send(monthlyConsumptionMessage.set((double)ZERO, 4));
	send(thingspeakMessage.set((double)ZERO, 4));
}

void checkPulseCountRequestStatus()
{
	if (!pulseCountReceived)
		sendPulseCountRequest = true;
}

void checkBlinksPerWattHourRequest()
{
	if (!blinksPerWattHourReceived)
		sendBlinksPerWattHourRequest = true;
}

void getAccumulation()
{
	request(RESET_TYPE_ID, V_VAR3);
}