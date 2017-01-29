#include <TimeAlarms.h>
#include <TimeLib.h>
#include <Time.h>
#include <SPI.h>

#define WATT_METER_NODE
#define NODE_INTERACTS_WITH_LCD

#define MY_RADIO_NRF24
#define MY_REPEATER_FEATURE
#define MY_NODE_ID PH3_NODE_ID
#define MY_DEBUG

#include <MyNodes.h>
#include <MySensors.h>
#include <MyConfig.h>

#define APPLICATION_NAME "3Phase Watt Meter"
#define APPLICATION_VERSION "13Dec2016"

#define DEFAULT_BLINKS_PER_KWH 1600 // value from energy meter
AlarmId heartbeatTimer;
AlarmId pulseCountTimer;
AlarmId pulsesPerWattHourTimer;
AlarmId updateConsumptionTimer;
AlarmId accumulationTimer;
AlarmId lcdUpdateTimer;

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
MyMessage lcdCurrWattMessage(CURR_WATT_ID, V_WATT);

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
	lcdCurrWattMessage.setDestination(LCD_NODE_ID);
}

void presentation()
{
	sendSketchInfo(APPLICATION_NAME, APPLICATION_VERSION);
	present(CURR_WATT_ID, S_POWER, "Current Consumption");
	wait(WAIT_AFTER_SEND_MESSAGE);
	present(HOURLY_WATT_CONSUMPTION_ID, S_POWER, "Hourly Consumption");
	wait(WAIT_AFTER_SEND_MESSAGE);
	present(DAILY_WATT_CONSUMPTION_ID, S_POWER, "Daily Consumption");
	wait(WAIT_AFTER_SEND_MESSAGE);
	present(MONTHLY_WATT_CONSUMPTION_ID, S_POWER, "Monthly Consumption");
	wait(WAIT_AFTER_SEND_MESSAGE);
	present(ACCUMULATED_WATT_CONSUMPTION_ID, S_POWER, "Total Consumption");
	wait(WAIT_AFTER_SEND_MESSAGE);
	present(CURR_PULSE_COUNT_ID, S_CUSTOM, "Pulse Count");
	wait(WAIT_AFTER_SEND_MESSAGE);
	present(BLINKS_PER_KWH_ID, S_CUSTOM, "Pulses per KWH");
	wait(WAIT_AFTER_SEND_MESSAGE);
	present(RESET_TYPE_ID, S_CUSTOM, "Reset Consumption");
	wait(WAIT_AFTER_SEND_MESSAGE);
	present(DELTA_WATT_CONSUMPTION_ID, S_POWER, "Delta Consumption");
}

void loop()
{
	if (sendPulseCountRequest)
	{
		sendPulseCountRequest = false;
		request(CURR_PULSE_COUNT_ID, V_VAR1);
		pulseCountTimer = Alarm.timerOnce(REQUEST_INTERVAL, checkPulseCountRequestStatus);
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
		pulsesPerWattHourTimer = Alarm.timerOnce(REQUEST_INTERVAL, checkBlinksPerWattHourRequest);
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
				if(accumulationStatusCount == 3)
					send(hourlyConsumptionMessage.set((float)ZERO, 5));
				request(HOURLY_WATT_CONSUMPTION_ID, V_KWH);
				break;
			case GET_DAILY_KWH:
				accumulationStatusCount++;
				if (accumulationStatusCount == 3)
					send(dailyConsumptionMessage.set((float)ZERO, 5));
				request(DAILY_WATT_CONSUMPTION_ID, V_KWH);
				break;
			case GET_MONTHLY_KWH:
				accumulationStatusCount++;
				if (accumulationStatusCount == 3)
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
		break;
	case V_KWH:
		switch (message.sensor)
		{
		case HOURLY_WATT_CONSUMPTION_ID:
			hourlyConsumptionInitKWH = accumulatedKWH - message.getFloat();
			accumulationsStatus = GET_DAILY_KWH;
			accumulationStatusCount = 0;
			break;
		case DAILY_WATT_CONSUMPTION_ID:
			dailyConsumptionInitKWH = accumulatedKWH - message.getFloat();
			accumulationsStatus = GET_MONTHLY_KWH;
			accumulationStatusCount = 0;
			break;
		case MONTHLY_WATT_CONSUMPTION_ID:
			monthlyConsumptionInitKWH = accumulatedKWH - message.getFloat();
			accumulationStatusCount = 0;
			accumulationsStatus = ALL_DONE;
			Alarm.free(accumulationTimer);
			accumulationTimer = Alarm.timerRepeat(FIVE_MINUTES, sendAccumulation);
			lcdUpdateTimer = Alarm.timerRepeat(ONE_MINUTE * 3, sendLCDAccumulation);
			break;
		case DELTA_WATT_CONSUMPTION_ID:
			float monthlyConsumptionKWHPH1 = message.getFloat();
			float deltaKWH = (accumulatedKWH - monthlyConsumptionInitKWH) - monthlyConsumptionKWHPH1;
			send(deltaConsumptionMessage.set(deltaKWH, 5));
			thingspeakMessage.setSensor(DELTA_WATT_CONSUMPTION_ID);
			send(thingspeakMessage.set(deltaKWH, 5));
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
		if(currWatt < MAX_WATT)
			send(currentConsumptionMessage.set(currWatt,2));
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
		if (accumulationsStatus == ALL_DONE)
		{
			send(hourlyConsumptionMessage.set((accumulatedKWH-hourlyConsumptionInitKWH), 5));
			wait(WAIT_AFTER_SEND_MESSAGE);
			send(dailyConsumptionMessage.set((accumulatedKWH - dailyConsumptionInitKWH), 5));
			wait(WAIT_AFTER_SEND_MESSAGE);
			send(monthlyConsumptionMessage.set((accumulatedKWH - monthlyConsumptionInitKWH), 5));
		}
	}
}

void resetHour()
{
	float sendKHWValue = accumulatedKWH - hourlyConsumptionInitKWH;
	hourlyConsumptionInitKWH = accumulatedKWH;
	send(hourlyConsumptionMessage.set(sendKHWValue, 5));
	thingspeakMessage.setSensor(HOURLY_WATT_CONSUMPTION_ID);
	send(thingspeakMessage.set(sendKHWValue, 5));
	MyMessage resetTypeMessage(RESET_TYPE_ID, V_VAR3);
	send(resetTypeMessage.set(RESET_NONE));
}

void resetDay()
{
	float sendKHWValue = accumulatedKWH - dailyConsumptionInitKWH;
	dailyConsumptionInitKWH = accumulatedKWH;
	send(dailyConsumptionMessage.set(sendKHWValue, 5));
	thingspeakMessage.setSensor(DAILY_WATT_CONSUMPTION_ID);
	send(thingspeakMessage.set(sendKHWValue, 5));
	MyMessage resetTypeMessage(RESET_TYPE_ID, V_VAR3);
	send(resetTypeMessage.set(RESET_NONE));
}

void resetMonth()
{
	float sendKHWValue = accumulatedKWH - monthlyConsumptionInitKWH;
	monthlyConsumptionInitKWH = accumulatedKWH;
	send(monthlyConsumptionMessage.set(sendKHWValue, 5));
	thingspeakMessage.setSensor(MONTHLY_WATT_CONSUMPTION_ID);
	send(thingspeakMessage.set(sendKHWValue, 5));
	MyMessage resetTypeMessage(RESET_TYPE_ID, V_VAR3);
	send(resetTypeMessage.set(RESET_NONE));
}

void resetAll()
{
	currPulseCount = 0;
	accumulatedKWH = 0;
	hourlyConsumptionInitKWH = 0;
	dailyConsumptionInitKWH = 0;
	monthlyConsumptionInitKWH = 0;
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

void sendAccumulation()
{
	if (currWatt < MAX_WATT)
	{
		thingspeakMessage.setSensor(CURR_WATT_ID);
		send(thingspeakMessage.set(currWatt, 2));
		wait(WAIT_AFTER_SEND_MESSAGE);
		lcdCurrWattMessage.setSensor(CURR_WATT_ID);
		send(lcdCurrWattMessage.set(currWatt, 2));
		wait(WAIT_AFTER_SEND_MESSAGE);
		sendDelta();
	}
	
}

void sendLCDAccumulation()
{
	if (currWatt < MAX_WATT)
	{
		lcdCurrWattMessage.setSensor(CURR_WATT_ID);
		send(lcdCurrWattMessage.set(currWatt, 2));
		wait(WAIT_AFTER_SEND_MESSAGE);
	}
}

void sendDelta()
{
	float deltaKWH = accumulatedKWH - monthlyConsumptionInitKWH;
	MyMessage realtimeDeltaConsumptionMessage(DELTA_WATT_CONSUMPTION_ID, V_KWH);
	realtimeDeltaConsumptionMessage.setDestination(PH1_NODE_ID);
	realtimeDeltaConsumptionMessage.setSensor(DELTA_WATT_CONSUMPTION_ID);
	send(realtimeDeltaConsumptionMessage.set(deltaKWH, 2));
	wait(WAIT_AFTER_SEND_MESSAGE);
}