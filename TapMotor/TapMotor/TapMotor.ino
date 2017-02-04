#include <Keypad.h>
#include <SPI.h>
#include <TimeAlarms.h>
#include <TimeLib.h>
#include <Time.h>

#define SUMP_RELATED_NODE
#define NODE_HAS_RELAY
#define KEYPAD_1R_2C
#define WATER_TANK_NODE_IDS

#define MY_RADIO_NRF24
//#define MY_REPEATER_FEATURE
#define MY_NODE_ID TAP_MOTOR_NODE_ID
//#define MY_DEBUG 

#include <MyNodes.h>
#include <MySensors.h>
#include <MyConfig.h>

#define APPLICATION_NAME "Tap Motor"

AlarmId heartbeatTimer;
boolean tank02AndTank03HighLevel;
boolean motorOn;
byte motorDelayCheckCount;

MyMessage tapMotorRelayMessage(RELAY_ID, V_STATUS);
MyMessage sumpMotorMessage(RELAY_ID, V_STATUS);
MyMessage pollTimerMessage;
Keypad keypad = Keypad(makeKeymap(keys), rowsPins, colsPins, ROWS, COLS);

void before()
{
	pinMode(RELAY_PIN, OUTPUT);
	pinMode(MOTOR_STATUS_PIN, OUTPUT);
}

void setup()
{
	motorDelayCheckCount = 0;
	keypad.addEventListener(keypadEvent);
	keypad.setDebounceTime(WAIT_50MS);
	digitalWrite(RELAY_PIN, LOW);
	digitalWrite(MOTOR_STATUS_PIN, LOW);
	tank02AndTank03HighLevel = false;
	pollTimerMessage.setType(V_VAR2);
	sumpMotorMessage.setDestination(SUMP_MOTOR_NODE_ID);
	heartbeatTimer = Alarm.timerRepeat(HEARTBEAT_INTERVAL, sendHeartbeat);
}

void presentation()
{
	sendSketchInfo(APPLICATION_NAME, __DATE__);
	present(RELAY_ID, S_BINARY, "Tap Motor Relay");
	Alarm.delay(WAIT_AFTER_SEND_MESSAGE);
	send(tapMotorRelayMessage.set(RELAY_OFF));
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
			if (motorOn)
				turnOffMotor();
			break;
		}
		break;
	case V_VAR3:
		tank02AndTank03HighLevel = message.getInt();

		if (tank02AndTank03HighLevel && motorOn)
			turnOffMotor();
		break;
	}
}

void turnOnMotor()
{
	digitalWrite(RELAY_PIN, RELAY_ON);
	send(tapMotorRelayMessage.set(RELAY_ON));
	Alarm.delay(WAIT_AFTER_SEND_MESSAGE);
	digitalWrite(MOTOR_STATUS_PIN, RELAY_ON);
	motorOn = true;
	pollTimerMessage.setDestination(TANK_03_NODE_ID);
	send(pollTimerMessage.set(RELAY_ON));
	Alarm.delay(WAIT_AFTER_SEND_MESSAGE);
	send(sumpMotorMessage.set(RELAY_ON));
	Alarm.delay(WAIT_AFTER_SEND_MESSAGE);
}

void turnOffMotor()
{
	digitalWrite(RELAY_PIN, RELAY_OFF);
	send(tapMotorRelayMessage.set(RELAY_OFF));
	Alarm.delay(WAIT_AFTER_SEND_MESSAGE);
	digitalWrite(MOTOR_STATUS_PIN, RELAY_OFF);
	motorOn = false;
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