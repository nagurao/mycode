#include <Keypad.h>
#include <SPI.h>
#include <TimeAlarms.h>
#include <TimeLib.h>
#include <Time.h>

#define WATER_MOTOR_NODE
#define NODE_HAS_RELAY
#define KEYPAD_1R_2C
#define WATER_TANK_NODE_IDS

#define MY_RADIO_NRF24
#define MY_REPEATER_FEATURE
#define MY_NODE_ID WATER_RELAY_NODE_ID
#define MY_DEBUG 

#include <MyNodes.h>
#include <MySensors.h>
#include <MyConfig.h>

#define APPLICATION_NAME "Water Motor"
#define APPLICATION_VERSION "20Nov2016"

AlarmId heartbeatTimer;
boolean tank02AndTank03HighLevel;
boolean delayStartSet;
boolean motorOn;
byte motorDelayCheckCount;

MyMessage waterMotorRelayMessage(WATER_RELAY_ID, V_STATUS);
MyMessage sumpMotorMessage(SUMP_RELAY_ID, V_STATUS);
MyMessage pollTimerMessage;
Keypad keypad = Keypad(makeKeymap(keys), rowsPins, colsPins, ROWS, COLS);

void before()
{
	pinMode(WATER_RELAY_PIN, OUTPUT);
	pinMode(MOTOR_STATUS_PIN, OUTPUT);
}

void setup()
{
	motorDelayCheckCount = 0;
	keypad.addEventListener(keypadEvent);
	keypad.setDebounceTime(WAIT_50MS);
	digitalWrite(WATER_RELAY_PIN, LOW);
	digitalWrite(MOTOR_STATUS_PIN, LOW);
	tank02AndTank03HighLevel = false;
	delayStartSet = false;
	sumpMotorMessage.setDestination(SUMP_RELAY_NODE_ID);
	heartbeatTimer = Alarm.timerRepeat(HEARTBEAT_INTERVAL, sendHeartbeat);
}

void presentation()
{
	sendSketchInfo(APPLICATION_NAME, APPLICATION_VERSION);
	present(WATER_RELAY_ID, S_BINARY, "Water Motor Relay");
	Alarm.delay(WAIT_AFTER_SEND_MESSAGE);
	send(waterMotorRelayMessage.set(RELAY_OFF));
	Alarm.delay(WAIT_AFTER_SEND_MESSAGE);
}

void loop()
{
	char key = keypad.getKey();
	Alarm.delay(1);
}

void receive(const MyMessage &message)
{
	switch (message.type)
	{
	case V_STATUS:
		switch (message.getInt())
		{
		case RELAY_ON:
			if (!tank02AndTank03HighLevel && !motorOn)
				turnOnMotor();
			break;
		case RELAY_OFF:
			if(motorOn)
				turnOffMotor();
			break;
		}
		break;
	case V_VAR2:
		tank02AndTank03HighLevel = message.getInt();
		break;
	}
}

void turnOnMotor()
{
	digitalWrite(WATER_RELAY_PIN, RELAY_ON);
	send(waterMotorRelayMessage.set(RELAY_ON));
	Alarm.delay(WAIT_AFTER_SEND_MESSAGE);
	digitalWrite(MOTOR_STATUS_PIN, RELAY_ON);
	motorOn = true;
	pollTimerMessage.setDestination(UNDERGROUND_NODE_ID);
	send(pollTimerMessage.set(RELAY_ON));
	Alarm.delay(WAIT_AFTER_SEND_MESSAGE);
	send(sumpMotorMessage.set(RELAY_ON));
	Alarm.delay(WAIT_AFTER_SEND_MESSAGE);
}

void turnOffMotor()
{
	digitalWrite(WATER_RELAY_PIN, RELAY_OFF);
	send(waterMotorRelayMessage.set(RELAY_OFF));
	Alarm.delay(WAIT_AFTER_SEND_MESSAGE);
	digitalWrite(MOTOR_STATUS_PIN, RELAY_OFF);
	motorOn = false;
	pollTimerMessage.setDestination(UNDERGROUND_NODE_ID);
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
			if (!tank02AndTank03HighLevel && !motorOn)
				turnOnMotor();
			break;
		case '2':
			if (motorOn)
				turnOffMotor();
			break;
		}
		break;
	case HOLD:
		switch (key)
		{
		case '1':
			if (!tank02AndTank03HighLevel && !motorOn)
				turnOnMotor();
			break;
		case '2':
			if (motorOn)
				turnOffMotor();
			break;
		}
		break;
	}
}