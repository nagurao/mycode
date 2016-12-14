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
#define APPLICATION_VERSION "13Dec2016"

#define DEFAULT_R1_VALUE 47.20F
#define DEFAULT_R2_VALUE 3.24F
#define DEFAULT_VOLTS 0.00F

AlarmId heartbeatTimer;
AlarmId nodeUpTimer;
AlarmId requestTimer;

float voltsPerBit;

MyMessage solarVoltageMessage(SOLAR_VOLTAGE_ID, V_VOLTAGE);

void before()
{
	pinMode(VOLTAGE_SENSE_PIN, INPUT);
}

void setup()
{
	heartbeatTimer = Alarm.timerRepeat(HEARTBEAT_INTERVAL, sendHeartbeat);
	solarVoltageMessage.setDestination(BATT_VOLTAGE_NODE_ID);
	nodeUpTimer = Alarm.timerRepeat(FIVE_MINUTES, sendNodeUpMessage);
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
		break;
	}
}

void readSolarVoltage()
{
	int sensedValue = 0;
	int inputValue = 0;
	int inputVolts = 0;
	float sensedVolts = 0;
	for (byte readCount = 1; readCount <= 10; readCount++)
	{
		inputValue = inputValue + analogRead(VOLTAGE_SENSE_PIN);
		Alarm.delay(WAIT_50MS);
		inputVolts = inputVolts + analogRead(INPUT_VOLTAGE_PIN);
		Alarm.delay(WAIT_50MS);
	}
	sensedValue = inputValue / 10;
	sensedVolts = inputVolts / 10 ;

	sensedVolts = sensedVolts * 5.0 / 1024;
	voltsPerBit = ((sensedVolts * (DEFAULT_R1_VALUE + DEFAULT_R2_VALUE)) / (DEFAULT_R2_VALUE * 1024));

	float solarVoltage = sensedValue * voltsPerBit;
	send(solarVoltageMessage.set(solarVoltage, 5));
	Alarm.delay(WAIT_AFTER_SEND_MESSAGE);
}

void sendNodeUpMessage()
{
	MyMessage nodeUpMessage(SOLAR_VOLTAGE_ID,V_VAR1);
	nodeUpMessage.setDestination(BATT_VOLTAGE_NODE_ID);
	send(nodeUpMessage.set(UP));
	Alarm.delay(WAIT_AFTER_SEND_MESSAGE);
}