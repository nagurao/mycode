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
#define APPLICATION_VERSION "08Oct2016"

#define DEFAULT_R1_VALUE 47.00F
#define DEFAULT_R2_VALUE 3.55F
#define DEFAULT_VOLTS 0.00F

AlarmId heartbeatTimer;
AlarmId nodeUpTimer;

boolean firstTime;
float voltsPerBit;

MyMessage solarVoltageMessage(SOLAR_VOLTAGE_ID, V_VOLTAGE);

void before()
{
	pinMode(VOLTAGE_SENSE_PIN, INPUT);
}

void setup()
{
	voltsPerBit = (((float)5.00 * (DEFAULT_R1_VALUE + DEFAULT_R2_VALUE)) / (DEFAULT_R2_VALUE * 1023));
	heartbeatTimer = Alarm.timerRepeat(HEARTBEAT_INTERVAL, sendHeartbeat);
	solarVoltageMessage.setDestination(BATT_VOLTAGE_NODE_ID);
	nodeUpTimer = Alarm.timerRepeat(ONE_MINUTE, sendNodeUpMessage);
	firstTime = true;
}

void presentation()
{
	sendSketchInfo(APPLICATION_NAME, APPLICATION_VERSION);
}

void loop()
{
	Alarm.delay(1);
}

void receive(const MyMessage &message)
{
	switch (message.type)
	{
	case V_VOLTAGE:
		readSolarVoltage();
		if (firstTime)
		{
			Alarm.free(nodeUpTimer);
			firstTime = false;
		}
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

	float solarVoltage = sensedValue * voltsPerBit;
	send(solarVoltageMessage.set(solarVoltage, 5));
}

void sendNodeUpMessage()
{
	MyMessage nodeUpMessage(SOLAR_VOLTAGE_ID,V_VAR1);
	nodeUpMessage.setDestination(BATT_VOLTAGE_NODE_ID);
	send(nodeUpMessage.set(UP));
}