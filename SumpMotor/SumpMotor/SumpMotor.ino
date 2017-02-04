#include <Keypad.h>
#include <SPI.h>
#include <TimeAlarms.h>
#include <TimeLib.h>
#include <Time.h>

#define SUMP_RELATED_NODE
#define LOW_TRIGGER_RELAY
#define NODE_HAS_RELAY
#define KEYPAD_1R_2C
#define WATER_TANK_NODE_IDS
#define NODE_WITH_HIGH_LOW_FEATURE

#define MY_RADIO_NRF24
//#define MY_REPEATER_FEATURE
#define MY_NODE_ID SUMP_MOTOR_NODE_ID
//#define MY_DEBUG 

#include <MyNodes.h>
#include <MySensors.h>
#include <MyConfig.h>

#define APPLICATION_NAME "Sump Motor"

AlarmId heartbeatTimer;
boolean tank02LowLevel;
boolean tank02HighLevel;
boolean tank03LowLevel;
boolean tank03HighLevel;
boolean sumpMotorOn;

MyMessage thingspeakMessage(WIFI_NODEMCU_ID, V_CUSTOM);
MyMessage sumpMotorRelayMessage(RELAY_ID, V_STATUS);
MyMessage pollTimerMessage;
MyMessage tank02And03WaterHighLevelMessage(CURR_WATER_LEVEL_ID, V_VAR3);

Keypad keypad = Keypad(makeKeymap(keys), rowsPins, colsPins, ROWS, COLS);


void before()
{
	pinMode(RELAY_PIN, OUTPUT);
	pinMode(MOTOR_STATUS_PIN, OUTPUT);
}

void setup()
{
	keypad.addEventListener(keypadEvent);
	keypad.setDebounceTime(WAIT_50MS);
	sumpMotorOn = false;
	tank02LowLevel = false;
	tank02HighLevel = false;
	tank03LowLevel = false;
	tank03HighLevel = false;

	digitalWrite(RELAY_PIN, LOW);
	digitalWrite(MOTOR_STATUS_PIN, LOW);

	thingspeakMessage.setDestination(THINGSPEAK_NODE_ID);
	thingspeakMessage.setType(V_CUSTOM);
	thingspeakMessage.setSensor(WIFI_NODEMCU_ID);

	tank02And03WaterHighLevelMessage.setDestination(TAP_MOTOR_NODE_ID);
	tank02And03WaterHighLevelMessage.setType(V_VAR3);
	tank02And03WaterHighLevelMessage.setSensor(CURR_WATER_LEVEL_ID);
	pollTimerMessage.setType(V_VAR2);

	heartbeatTimer = Alarm.timerRepeat(HEARTBEAT_INTERVAL, sendHeartbeat);
}

void presentation()
{
	sendSketchInfo(APPLICATION_NAME, __DATE__);
	Alarm.delay(WAIT_AFTER_SEND_MESSAGE);
	present(RELAY_ID, S_BINARY, "Sump Motor Relay");
	Alarm.delay(WAIT_AFTER_SEND_MESSAGE);
	send(sumpMotorRelayMessage.set(RELAY_OFF));
	Alarm.delay(WAIT_AFTER_SEND_MESSAGE);
	send(thingspeakMessage.set(RELAY_OFF));
}

void loop()
{
	char key = keypad.getKey();
	Alarm.delay(1);
}

void receive(const MyMessage &message)
{
	byte incomingValue;
	switch (message.type) 
	{
	case V_STATUS:
		switch (message.sender)
		{
		case THINGSPEAK_NODE_ID:
			incomingValue = message.getBool();
			break;
		default:
			incomingValue = message.getInt();
			break;
		}
		switch (incomingValue)
		{
		case RELAY_ON:
			if (!tank02HighLevel && !tank03LowLevel && !sumpMotorOn)
				turnOnSumpMotor();
			break;
		case RELAY_OFF:
			if (sumpMotorOn)
				turnOffSumpMotor();
			break;
		}
		break;
	case V_VAR2:
		switch (message.sender)
		{
		case TANK_02_NODE_ID:
			if (message.getInt())
				tank02LowLevel = LOW_LEVEL;
			else
				tank02LowLevel = NOT_LOW_LEVEL;
			break;
		case TANK_03_NODE_ID:
			if (message.getInt())
				tank03LowLevel = LOW_LEVEL;
			else
				tank03LowLevel = NOT_LOW_LEVEL;
			break;
		}

		if(tank02LowLevel && !tank03LowLevel && !sumpMotorOn)
			turnOnSumpMotor();

		if(tank03LowLevel && sumpMotorOn)
			turnOffSumpMotor();
		break;
	case V_VAR3:
		switch (message.sender)
		{
		case TANK_02_NODE_ID:
			if (message.getInt())
				tank02HighLevel = HIGH_LEVEL;
			else
				tank02HighLevel = NOT_HIGH_LEVEL;
			break;
		case TANK_03_NODE_ID:
			if (message.getInt())
				tank03HighLevel = HIGH_LEVEL;
			else
				tank03HighLevel = NOT_HIGH_LEVEL;
			break;
		}

		Alarm.delay(WAIT_AFTER_SEND_MESSAGE);

		if(!tank02HighLevel && tank03HighLevel && !sumpMotorOn)
			turnOnSumpMotor();

		if(tank02HighLevel && sumpMotorOn)
			turnOffSumpMotor();

		if (tank02HighLevel && tank03HighLevel)
			send(tank02And03WaterHighLevelMessage.set(HIGH_LEVEL));
		else
			send(tank02And03WaterHighLevelMessage.set(NOT_HIGH_LEVEL));
		
		Alarm.delay(WAIT_AFTER_SEND_MESSAGE);

		break;
	}
}

void turnOnSumpMotor()
{
	digitalWrite(RELAY_PIN, RELAY_ON);
	send(sumpMotorRelayMessage.set(RELAY_ON));
	Alarm.delay(WAIT_AFTER_SEND_MESSAGE);
	send(thingspeakMessage.set(RELAY_ON));
	Alarm.delay(WAIT_AFTER_SEND_MESSAGE);
	digitalWrite(MOTOR_STATUS_PIN, RELAY_ON);
	sumpMotorOn = true;
	pollTimerMessage.setDestination(TANK_02_NODE_ID);
	send(pollTimerMessage.set(RELAY_ON));
	Alarm.delay(WAIT_AFTER_SEND_MESSAGE);
	pollTimerMessage.setDestination(TANK_03_NODE_ID);
	send(pollTimerMessage.set(RELAY_ON));
}

void turnOffSumpMotor()
{
	digitalWrite(RELAY_PIN, RELAY_OFF);
	send(sumpMotorRelayMessage.set(RELAY_OFF));
	Alarm.delay(WAIT_AFTER_SEND_MESSAGE);
	send(thingspeakMessage.set(RELAY_OFF));
	Alarm.delay(WAIT_AFTER_SEND_MESSAGE);
	digitalWrite(MOTOR_STATUS_PIN, RELAY_OFF);
	sumpMotorOn = false;
	pollTimerMessage.setDestination(TANK_02_NODE_ID);
	send(pollTimerMessage.set(RELAY_OFF));
	Alarm.delay(WAIT_AFTER_SEND_MESSAGE);
	pollTimerMessage.setDestination(TANK_03_NODE_ID);
	send(pollTimerMessage.set(RELAY_OFF));
	Alarm.delay(WAIT_AFTER_SEND_MESSAGE);
}

void keypadEvent(KeypadEvent key)
{
	switch (keypad.getState())
	{
	case PRESSED:
		switch (key)
		{
		case '1':
			if (!tank02HighLevel && !tank03LowLevel && !sumpMotorOn)
				turnOnSumpMotor();
			break;
		case '2':
			if (sumpMotorOn)
				turnOffSumpMotor();
			break;
		}
		break;
	case HOLD:
		switch (key)
		{
		case '1':
			if (!tank02HighLevel && !tank03LowLevel && !sumpMotorOn)
				turnOnSumpMotor();
			break;
		case '2':
			if (sumpMotorOn)
				turnOffSumpMotor();
			break;
		}
		break;
	}
}