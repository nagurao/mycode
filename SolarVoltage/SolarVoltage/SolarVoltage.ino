#include <TimeAlarms.h>
#include <TimeLib.h>
#include <Time.h>
#include <SPI.h>

#define SOLAR_BATT_VOLTAGE_NODE

#define MY_RADIO_NRF24
#define MY_REPEATER_FEATURE
#define MY_NODE_ID SOLAR_VOLTAGE_NODE_ID
#define MY_DEBUG

#include <MyNodes.h>
#include <MySensors.h>
#include <MyConfig.h>

#define APPLICATION_NAME "Solar Voltage"
#define APPLICATION_VERSION "06Oct2016"

#define DEFAULT_R1_VALUE 47000
#define DEFAULT_R2_VALUE 3300
#define DEFAULT_MAX_VOLTS 30

AlarmId heartbeatTimer;
AlarmId resistorR1Timer;
AlarmId resistorR2Timer;
AlarmId maxVoltsTimer;

int R1Value;
byte resistorR1RequestCount;
boolean sendResistorR1Request;
boolean resistorR1Received;

int R2Value;
byte resistorR2RequestCount;
boolean sendResistorR2Request;
boolean resistorR2Received;

int maxVolts;
byte maxVoltsRequestCount;
boolean sendMaxVoltsRequest;
boolean maxVoltsReceived;

boolean calcVoltsPerBit;
float scaleFactor;

MyMessage solarVoltageMessage(SOLAR_VOLTAGE_ID, V_VOLTAGE);

void before()
{
	pinMode(VOLTAGE_SENSE_PIN, INPUT);
}

void setup()
{
	R1Value = 0;
	R2Value = 0;
	maxVolts = 0;
	resistorR1RequestCount = 0;
	resistorR2RequestCount = 0;
	maxVoltsRequestCount = 0;
	sendResistorR1Request = true;
	sendResistorR2Request = true;
	sendMaxVoltsRequest = true;
	resistorR1Received = false;
	resistorR2Received = false;
	maxVoltsReceived = false;
	calcVoltsPerBit = true;
	voltsPerBit = 0.00;
	heartbeatTimer = Alarm.timerRepeat(HEARTBEAT_INTERVAL, sendHeartbeat);
	solarVoltageMessage.setDestination(BATT_VOLTAGE_NODE_ID);
}

void presentation()
{
	sendSketchInfo(APPLICATION_NAME, APPLICATION_VERSION);
	present(R1_VALUE_ID, S_CUSTOM, "R1 Resistor Value");
	wait(WAIT_50MS);
	present(R2_VALUE_ID, S_CUSTOM, "R2 Resistor Value");
	wait(WAIT_50MS);
	present(MAX_VOLTS_ID, S_CUSTOM, "Max Volts Value");
}

void loop()
{
	if (sendResistorR1Request)
	{
		sendResistorR1Request = false;
		request(R1_VALUE_ID, V_VAR1);
		resistorR1Timer = Alarm.timerOnce(REQUEST_INTERVAL, checkResistorR1RequestStatus);
		resistorR1RequestCount++;
		if (resistorR1RequestCount == 10)
		{
			MyMessage currModeMessage(R1_VALUE_ID, V_VAR1);
			send(currModeMessage.set(DEFAULT_R1_VALUE));
		}
	}
	if (sendResistorR2Request)
	{
		sendResistorR2Request = false;
		request(R2_VALUE_ID, V_VAR2);
		resistorR2Timer = Alarm.timerOnce(REQUEST_INTERVAL, checkResistorR2RequestStatus);
		resistorR2RequestCount++;
		if (resistorR2RequestCount == 10)
		{
			MyMessage currModeMessage(R2_VALUE_ID, V_VAR2);
			send(currModeMessage.set(DEFAULT_R2_VALUE));
		}
	}
	if (sendMaxVoltsRequest)
	{
		sendMaxVoltsRequest = false;
		request(MAX_VOLTS_ID, V_VAR3);
		maxVoltsTimer = Alarm.timerOnce(REQUEST_INTERVAL, checkMaxVoltsRequestStatus);
		maxVoltsRequestCount++;
		if (maxVoltsRequestCount == 10)
		{
			MyMessage currModeMessage(MAX_VOLTS_ID, V_VAR3);
			send(currModeMessage.set(DEFAULT_MAX_VOLTS));
		}
	}

	if (calcVoltsPerBit)
	{
		if (resistorR1Received && resistorR2Received && maxVoltsReceived)
		{
			scaleFactor = ((R1Value + R2Value) / R2Value)  * (5.00 / 1024);
			calcVoltsPerBit = false;
			
		}
	}
	Alarm.delay(1);
}

void receive(const MyMessage &message)
{
	switch (message.type)
	{
	case V_VAR1:
		R1Value = message.getInt();
		if (resistorR1Received)
		{
			calcVoltsPerBit = true;
		}
		else
		{
			resistorR1Received = true;
			Alarm.free(resistorR1Timer);
			sendResistorR1Request = false;
			request(R1_VALUE_ID, V_VAR1);
		}
		break;
	case V_VAR2:
		R2Value = message.getInt();
		if (resistorR2Received)
		{
			calcVoltsPerBit = true;
		}
		else
		{
			resistorR2Received = true;
			Alarm.free(resistorR2Timer);
			sendResistorR2Request = false;
			request(R2_VALUE_ID, V_VAR2);
		}
		break;
	case V_VAR3:
		maxVolts = message.getInt();
		if (maxVoltsReceived)
		{
			calcVoltsPerBit = true;
		}
		else
		{
			maxVoltsReceived = true;
			Alarm.free(maxVoltsTimer);
			sendMaxVoltsRequest = false;
			request(MAX_VOLTS_ID, V_VAR3);
		}
		break;
	case V_VOLTAGE:
		readSolarVoltage();
		break;
	}
}

void readSolarVoltage()
{
	int sensedValue = 0;
	int inputValue = 0;
	for (byte readCount = 1; readCount <= 10; readCount++)
	{
		inputValue = inputValue + analogRead(VOLTAGE_SENSE_PIN);
		Alarm.delay(WAIT_50MS);
	}
	sensedValue = inputValue / 10;
	float sensedVoltage = sensedValue * scaleFactor;
	send(solarVoltageMessage.set(sensedVoltage, 5));
}

void checkResistorR1RequestStatus()
{
	if (!resistorR1Received)
		sendResistorR1Request = true;
}

void checkResistorR2RequestStatus()
{
	if (!resistorR2Received)
		sendResistorR2Request = true;
}
void checkMaxVoltsRequestStatus()
{
	if (!maxVoltsReceived)
		sendMaxVoltsRequest = true;
}