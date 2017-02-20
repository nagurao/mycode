#include <TimeAlarms.h>
#include <TimeLib.h>
#include <Time.h>
#include <SPI.h>

#define WATT_METER_NODE

#define MY_RADIO_NRF24
//#define MY_REPEATER_FEATURE
#define MY_NODE_ID INV_IN_NODE_ID
//#define MY_PARENT_NODE_ID BATT_VOLTAGE_NODE_ID
//#define MY_DEBUG

#include <MyNodes.h>
#include <MySensors.h>
#include <MyConfig.h>

#define APPLICATION_NAME "Inverter In"

#define DEFAULT_BLINKS_PER_KWH 6400 // value from energy meter
AlarmId heartbeatTimer;
AlarmId requestTimer;
AlarmId updateConsumptionTimer;
AlarmId accumulationTimer;

boolean sendPulseCountRequest;
boolean pulseCountReceived;
byte pulseCountRequestCount;

boolean sendBlinksPerWattHourRequest;
boolean blinksPerWattHourReceived;
byte blinksPerWattHourCount;
long blinksPerWattHour;
float pulseFactor;

volatile unsigned long currPulseCount;
volatile unsigned long prevPulseCount;
volatile float currWatt;
volatile float prevWatt;
volatile unsigned long lastBlink;

float accumulatedKWH;
float hourlyConsumptionInitKWH;
float dailyConsumptionInitKWH;
float monthlyConsumptionInitKWH;
float unitsPerHour;
float unitsPerDay;
float unitsPerMonth;
float deltaUnitsTillDay;

byte accumulationsStatus;
byte accumulationStatusCount;
boolean firstTime;
boolean sendAccumulationData;

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
	sendBlinksPerWattHourRequest = false;
	blinksPerWattHourReceived = false;
	blinksPerWattHourCount = 0;
	lastBlink = 0;
	prevWatt = 0.00;
	currWatt = 0.00;
	pulseFactor = 0;
	accumulatedKWH = 0.00;
	hourlyConsumptionInitKWH = 0.00;
	dailyConsumptionInitKWH = 0.00;
	monthlyConsumptionInitKWH = 0.00;
	unitsPerHour = 0.00;
	unitsPerDay = 0.00;
	unitsPerMonth = 0.00;
	deltaUnitsTillDay = 0.00;
	accumulationsStatus = GET_HOURLY_KWH;
	accumulationStatusCount = 0;
	firstTime = true;
	sendAccumulationData = true;
	thingspeakMessage.setDestination(THINGSPEAK_NODE_ID);
}

void presentation()
{
	sendSketchInfo(APPLICATION_NAME, __DATE__);
	Alarm.delay(WAIT_AFTER_SEND_MESSAGE);
	present(CURR_WATT_ID, S_POWER, "Current Consumption");
	Alarm.delay(WAIT_AFTER_SEND_MESSAGE);
	present(HOURLY_WATT_CONSUMPTION_ID, S_POWER, "Hourly Consumption");
	Alarm.delay(WAIT_AFTER_SEND_MESSAGE);
	present(DAILY_WATT_CONSUMPTION_ID, S_POWER, "Daily Consumption");
	Alarm.delay(WAIT_AFTER_SEND_MESSAGE);
	present(MONTHLY_WATT_CONSUMPTION_ID, S_POWER, "Monthly Consumption");
	Alarm.delay(WAIT_AFTER_SEND_MESSAGE);
	present(ACCUMULATED_WATT_CONSUMPTION_ID, S_POWER, "Total Consumption");
	Alarm.delay(WAIT_AFTER_SEND_MESSAGE);
	present(DELTA_WATT_CONSUMPTION_ID, S_POWER, "Delta Consumption");
	Alarm.delay(WAIT_AFTER_SEND_MESSAGE);
	present(CURR_PULSE_COUNT_ID, S_CUSTOM, "Pulse Count");
	Alarm.delay(WAIT_AFTER_SEND_MESSAGE);
	present(BLINKS_PER_KWH_ID, S_CUSTOM, "Pulses per KWH");
	Alarm.delay(WAIT_AFTER_SEND_MESSAGE);
	present(RESET_TYPE_ID, S_CUSTOM, "Reset Consumption");
	Alarm.delay(WAIT_AFTER_SEND_MESSAGE);
	present(INCOMING_REQUEST_ID, S_CUSTOM, "Request Data");
	Alarm.delay(WAIT_AFTER_SEND_MESSAGE);
}

void loop()
{
	if (sendPulseCountRequest)
	{
		sendPulseCountRequest = false;
		request(CURR_PULSE_COUNT_ID, V_VAR1);
		requestTimer = Alarm.timerOnce(REQUEST_INTERVAL, checkPulseCountRequestStatus);
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
		requestTimer = Alarm.timerOnce(REQUEST_INTERVAL, checkBlinksPerWattHourRequest);
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
			Alarm.free(requestTimer);
			sendBlinksPerWattHourRequest = true;
			requestTimer = Alarm.timerRepeat(ACCUMULATION_FREQUENCY_SECS, updateConsumptionData);
		}
		break;
	case V_VAR2:
		if (!blinksPerWattHourReceived)
		{
			blinksPerWattHourReceived = true;
			Alarm.free(requestTimer);
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
				if (accumulationStatusCount == 20)
					send(hourlyConsumptionMessage.set((float)ZERO, 5));
				request(HOURLY_WATT_CONSUMPTION_ID, V_KWH);
				break;
			case GET_DAILY_KWH:
				accumulationStatusCount++;
				if (accumulationStatusCount == 20)
					send(dailyConsumptionMessage.set((float)ZERO, 5));
				request(DAILY_WATT_CONSUMPTION_ID, V_KWH);
				break;
			case GET_MONTHLY_KWH:
				accumulationStatusCount++;
				if (accumulationStatusCount == 20)
					send(monthlyConsumptionMessage.set((float)ZERO, 5));
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
		break;/*
	case V_VAR5:
		switch (message.getInt())
		{
		case REQ_CURR_WATT:
			thingspeakMessage.setSensor(CURR_WATT_ID);
			send(thingspeakMessage.set(currWatt, 5));
			wait(WAIT_AFTER_SEND_MESSAGE);
			break;
		case REQ_HOURLY_WATT:
			thingspeakMessage.setSensor(HOURLY_WATT_CONSUMPTION_ID);
			send(thingspeakMessage.set(unitsPerHour, 5));
			wait(WAIT_AFTER_SEND_MESSAGE);
			break;
		case REQ_DAILY_WATT:
			thingspeakMessage.setSensor(DAILY_WATT_CONSUMPTION_ID);
			send(thingspeakMessage.set(unitsPerDay, 5));
			wait(WAIT_AFTER_SEND_MESSAGE);
			break;
		case REQ_MONTHLY_WATT:
			thingspeakMessage.setSensor(MONTHLY_WATT_CONSUMPTION_ID);
			send(thingspeakMessage.set(unitsPerMonth, 5));
			wait(WAIT_AFTER_SEND_MESSAGE);
			break;
		case REQ_DAILY_DELTA_WATT:
			thingspeakMessage.setSensor(DELTA_WATT_CONSUMPTION_ID);
			send(thingspeakMessage.set(deltaUnitsPerDay, 5));
			wait(WAIT_AFTER_SEND_MESSAGE);
			break;
		case REQ_REAL_DELTA_WATT:
			float deltaKWH = accumulatedKWH - monthlyConsumptionInitKWH;
			MyMessage realtimeDeltaConsumptionMessage(DELTA_WATT_CONSUMPTION_ID, V_KWH);
			realtimeDeltaConsumptionMessage.setDestination(INV_OUT_NODE_ID);
			realtimeDeltaConsumptionMessage.setSensor(DELTA_WATT_CONSUMPTION_ID);
			send(realtimeDeltaConsumptionMessage.set(deltaKWH, 2));
			wait(WAIT_AFTER_SEND_MESSAGE);
			break;
		}
		break;*/
	case V_KWH:
		switch (message.sensor)
		{
		case HOURLY_WATT_CONSUMPTION_ID:
			hourlyConsumptionInitKWH = accumulatedKWH - message.getFloat();
			unitsPerHour = hourlyConsumptionInitKWH;
			accumulationsStatus = GET_DAILY_KWH;
			accumulationStatusCount = 0;
			break;
		case DAILY_WATT_CONSUMPTION_ID:
			dailyConsumptionInitKWH = accumulatedKWH - message.getFloat();
			unitsPerDay = dailyConsumptionInitKWH;
			accumulationsStatus = GET_MONTHLY_KWH;
			accumulationStatusCount = 0;
			break;
		case MONTHLY_WATT_CONSUMPTION_ID:
			monthlyConsumptionInitKWH = accumulatedKWH - message.getFloat();
			unitsPerMonth = monthlyConsumptionInitKWH;
			accumulationStatusCount = 0;
			accumulationsStatus = ALL_DONE;
			Alarm.free(accumulationTimer);
			accumulationTimer = Alarm.timerRepeat(FIVE_MINUTES, setAccumulationDataFlag);
			break;
		case DELTA_WATT_CONSUMPTION_ID:
			deltaUnitsTillDay = (accumulatedKWH - monthlyConsumptionInitKWH) - message.getFloat();
			send(deltaConsumptionMessage.set(deltaUnitsTillDay, 5));
			wait(WAIT_AFTER_SEND_MESSAGE);
			thingspeakMessage.setSensor(DELTA_WATT_CONSUMPTION_ID);
			send(thingspeakMessage.set(deltaUnitsTillDay, 5));
			wait(WAIT_AFTER_SEND_MESSAGE);
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
		if (currWatt < MAX_WATT_INVERTER)
			send(currentConsumptionMessage.set(currWatt, 2));
		prevWatt = currWatt;
	}
	if (currPulseCount != prevPulseCount)
	{
		send(pulseCountMessage.set(currPulseCount));
		prevPulseCount = currPulseCount;
		float currAccumulatedKWH = ((float)currPulseCount / ((float)blinksPerWattHour));
		if (currAccumulatedKWH != accumulatedKWH)
		{
			send(accumulatedKWMessage.set(currAccumulatedKWH, 5));
			accumulatedKWH = currAccumulatedKWH;
			if (firstTime)
			{
				MyMessage resetTypeMessage(RESET_TYPE_ID, V_VAR3);
				send(resetTypeMessage.set(RESET_NONE));
				firstTime = false;
				request(RESET_TYPE_ID, V_VAR3);
				accumulationTimer = Alarm.timerRepeat(REQUEST_INTERVAL, getAccumulation);
			}
		}
		if (accumulationsStatus == ALL_DONE && sendAccumulationData)
		{
			thingspeakMessage.setSensor(CURR_WATT_ID);
			send(thingspeakMessage.set(currWatt, 5));
			wait(WAIT_AFTER_SEND_MESSAGE);
			send(hourlyConsumptionMessage.set((accumulatedKWH - hourlyConsumptionInitKWH), 5));
			wait(WAIT_AFTER_SEND_MESSAGE);
			send(dailyConsumptionMessage.set((accumulatedKWH - dailyConsumptionInitKWH), 5));
			wait(WAIT_AFTER_SEND_MESSAGE);
			send(monthlyConsumptionMessage.set((accumulatedKWH - monthlyConsumptionInitKWH), 5));
			wait(WAIT_AFTER_SEND_MESSAGE);
			sendAccumulationData = false;
			float deltaKWH = accumulatedKWH - monthlyConsumptionInitKWH;
			MyMessage realtimeDeltaConsumptionMessage(DELTA_WATT_CONSUMPTION_ID, V_KWH);
			realtimeDeltaConsumptionMessage.setDestination(INV_OUT_NODE_ID);
			realtimeDeltaConsumptionMessage.setSensor(DELTA_WATT_CONSUMPTION_ID);
			send(realtimeDeltaConsumptionMessage.set(deltaKWH, 2));
			wait(WAIT_AFTER_SEND_MESSAGE);
		}
	}
}

void resetHour()
{
	unitsPerHour = accumulatedKWH - hourlyConsumptionInitKWH;
	hourlyConsumptionInitKWH = accumulatedKWH;
	send(hourlyConsumptionMessage.set(unitsPerHour, 5));
	wait(WAIT_AFTER_SEND_MESSAGE);
	thingspeakMessage.setSensor(HOURLY_WATT_CONSUMPTION_ID);
	send(thingspeakMessage.set(unitsPerHour, 5));
	wait(WAIT_AFTER_SEND_MESSAGE);
	MyMessage resetTypeMessage(RESET_TYPE_ID, V_VAR3);
	send(resetTypeMessage.set(RESET_NONE));
	wait(WAIT_AFTER_SEND_MESSAGE);
}

void resetDay()
{
	unitsPerDay = accumulatedKWH - dailyConsumptionInitKWH;
	dailyConsumptionInitKWH = accumulatedKWH;
	send(dailyConsumptionMessage.set(unitsPerDay, 5));
	wait(WAIT_AFTER_SEND_MESSAGE);
	thingspeakMessage.setSensor(DAILY_WATT_CONSUMPTION_ID);
	send(thingspeakMessage.set(unitsPerDay, 5));
	wait(WAIT_AFTER_SEND_MESSAGE);
	MyMessage resetTypeMessage(RESET_TYPE_ID, V_VAR3);
	send(resetTypeMessage.set(RESET_NONE));
	wait(WAIT_AFTER_SEND_MESSAGE);
}

void resetMonth()
{
	unitsPerMonth = accumulatedKWH - monthlyConsumptionInitKWH;
	monthlyConsumptionInitKWH = accumulatedKWH;
	send(monthlyConsumptionMessage.set(unitsPerMonth, 5));
	wait(WAIT_AFTER_SEND_MESSAGE);
	thingspeakMessage.setSensor(MONTHLY_WATT_CONSUMPTION_ID);
	send(thingspeakMessage.set(unitsPerMonth, 5));
	wait(WAIT_AFTER_SEND_MESSAGE);
	MyMessage resetTypeMessage(RESET_TYPE_ID, V_VAR3);
	send(resetTypeMessage.set(RESET_NONE));
	wait(WAIT_AFTER_SEND_MESSAGE);
}

void resetAll()
{
	currPulseCount = 0;
	accumulatedKWH = 0;
	hourlyConsumptionInitKWH = 0;
	dailyConsumptionInitKWH = 0;
	monthlyConsumptionInitKWH = 0;
	unitsPerHour = 0.00;
	unitsPerDay = 0.00;
	unitsPerMonth = 0.00;
	deltaUnitsTillDay = 0.00;
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

void setAccumulationDataFlag()
{
	sendAccumulationData = true;
}

