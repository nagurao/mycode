#include <TimeAlarms.h>
#include <TimeLib.h>
#include <Time.h>
#include <SPI.h>

#define SOLAR_BATT_VOLTAGE_NODE
#define NODE_INTERACTS_WITH_LCD

#define MY_RADIO_NRF24
#define MY_REPEATER_FEATURE
#define MY_NODE_ID BATT_VOLTAGE_NODE_ID
#define MY_DEBUG

#include <MyNodes.h>
#include <MySensors.h>
#include <MyConfig.h>

#define APPLICATION_NAME "Battery Voltage"
#define APPLICATION_VERSION "16Oct2016"

#define DEFAULT_R1_VALUE 47.40F
#define DEFAULT_R2_VALUE 3.23F
#define DEFAULT_VOLTS 0.00F

AlarmId heartbeatTimer;
AlarmId getSolarVoltageTimer;
AlarmId getBatteryVoltageTimer;
AlarmId requestSolarVoltageTimer;
AlarmId thingspeakMessageTimer;

byte solarNodeRequestCount;
byte solarVoltageRequestCount;
boolean solarNodeUp;
boolean sendSolarVoltageRequest;
boolean solarVoltageReceived;

float voltsPerBit;
float solarVoltage;
float batteryVoltage;

MyMessage solarVoltageMessage(SOLAR_VOLTAGE_ID, V_VOLTAGE);
MyMessage batteryVoltageMessage(BATTERY_VOLTAGE_ID, V_VOLTAGE);
MyMessage thingspeakMessage(WIFI_NODEMCU_ID, V_CUSTOM);
MyMessage lcdVoltageMessage;

void before()
{
	pinMode(VOLTAGE_SENSE_PIN, INPUT);
}

void setup()
{
	voltsPerBit = (((float)5.00 * (DEFAULT_R1_VALUE + DEFAULT_R2_VALUE)) / (DEFAULT_R2_VALUE * 1023));
	
	solarVoltage = 0;
	batteryVoltage = 0;
	solarNodeRequestCount = 0;
	solarVoltageRequestCount = 0;
	solarNodeUp = false;
	sendSolarVoltageRequest = false;
	solarVoltageReceived = false;

	thingspeakMessage.setDestination(THINGSPEAK_NODE_ID);
	lcdVoltageMessage.setDestination(LCD_NODE_ID);
	lcdVoltageMessage.setType(V_VOLTAGE);

	getBatteryVoltageTimer = Alarm.timerRepeat(ONE_MINUTE, getBatteryVoltage);
	thingspeakMessageTimer = Alarm.timerRepeat(FIVE_MINUTES, sendThingspeakMessage);
	heartbeatTimer = Alarm.timerRepeat(HEARTBEAT_INTERVAL, sendHeartbeat);

}

void presentation()
{
	sendSketchInfo(APPLICATION_NAME, APPLICATION_VERSION);
	present(SOLAR_VOLTAGE_ID, S_MULTIMETER, "Solar Voltage");
	wait(WAIT_50MS);
	present(BATTERY_VOLTAGE_ID, S_MULTIMETER, "Battery Voltage");
}

void loop()
{

	if (sendSolarVoltageRequest)
	{
		sendSolarVoltageRequest = false;
		request(SOLAR_VOLTAGE_ID, V_VOLTAGE, SOLAR_VOLTAGE_NODE_ID);
		requestSolarVoltageTimer = Alarm.timerOnce(REQUEST_INTERVAL, checkSolarVolategRequestStatus);
		solarVoltageRequestCount++;
		if (solarVoltageRequestCount == 3)
		{
			solarVoltage = DEFAULT_VOLTS;
			sendSolarVoltage();
			Alarm.free(requestSolarVoltageTimer);
			solarVoltageReceived = true;
			solarVoltageRequestCount = 0;
			solarNodeRequestCount++;
		}
		if (solarNodeRequestCount == 10)
		{
			solarNodeUp = false;
			Alarm.free(getSolarVoltageTimer);
		}
	}

	Alarm.delay(1);
}

void receive(const MyMessage &message)
{
	
	switch (message.type)
	{
	case V_VAR1:
		if (message.getInt() == UP)
		{
			if (!solarNodeUp)
			{
				solarNodeUp = true;
				getSolarVoltageTimer = Alarm.timerRepeat(ONE_MINUTE, getSolarVoltage);
			}
		}	
		break;
	case V_VOLTAGE:
		solarVoltage = message.getFloat();
		sendSolarVoltage();
		
		if (!solarVoltageReceived)
		{
			Alarm.free(requestSolarVoltageTimer);
			solarVoltageReceived = true;
			solarNodeRequestCount = 0;
			solarVoltageRequestCount = 0;
		}
		break;
	}
}

void getBatteryVoltage()
{
	int sensedValue = 0;
	int inputValue = 0;
	for (byte readCount = 1; readCount <= 10; readCount++)
	{
		inputValue = inputValue + analogRead(VOLTAGE_SENSE_PIN);
		Alarm.delay(WAIT_50MS);
	}
	sensedValue = inputValue / 10;

	batteryVoltage = sensedValue * voltsPerBit;

	send(batteryVoltageMessage.set(batteryVoltage, 5));
	Alarm.delay(WAIT_10MS);
	
	lcdVoltageMessage.setSensor(BATTERY_VOLTAGE_ID);
	send(lcdVoltageMessage.set(batteryVoltage, 5));
}

void getSolarVoltage()
{
	sendSolarVoltageRequest = true;
	solarVoltageReceived = false;
}

void checkSolarVolategRequestStatus()
{
	if (!solarVoltageReceived)
		sendSolarVoltageRequest = true;
}

void sendSolarVoltage ()
{
	send(solarVoltageMessage.set(solarVoltage, 5));
	Alarm.delay(WAIT_10MS);
	lcdVoltageMessage.setSensor(SOLAR_VOLTAGE_ID);
	send(lcdVoltageMessage.set(solarVoltage, 5));
}

void sendThingspeakMessage()
{
	thingspeakMessage.setSensor(SOLAR_VOLTAGE_ID);
	send(thingspeakMessage.set(solarVoltage, 5));
	Alarm.delay(WAIT_10MS);
	thingspeakMessage.setSensor(BATTERY_VOLTAGE_ID);
	send(thingspeakMessage.set(batteryVoltage, 5));
}