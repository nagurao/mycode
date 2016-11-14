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
#define APPLICATION_VERSION "12Nov2016"

AlarmId heartbeatTimer;
boolean tank02AndTank03HighLevel;
boolean delayStartSet;
boolean motorOn;
byte motorDelayCheckCount;

MyMessage waterMotorRelayMessage(WATER_RELAY_ID, V_STATUS);
MyMessage waterMotorDelayMessage(WATER_RELAY_DELAY_ID, V_VAR1);
MyMessage sumpMotorMessage(SUMP_RELAY_ID, V_STATUS);

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
	tank02HighLevel = false;
	tank03HighLevel = false;
	delayStartSet = false;
	sumpMotorMessage.setDestination(SUMP_RELAY_NODE_ID);
	heartbeatTimer = Alarm.timerRepeat(HEARTBEAT_INTERVAL, sendHeartbeat);
}

void presentation()
{
	sendSketchInfo(APPLICATION_NAME, APPLICATION_VERSION);
	present(WATER_RELAY_ID, S_BINARY, "Water Motor Relay");
	Alarm.delay(WAIT_10MS);
	send(waterMotorRelayMessage.set(RELAY_OFF));
	present(WATER_RELAY_DELAY_ID, S_CUSTOM, "Delay Start");
	Alarm.delay(WAIT_10MS);
	send(waterMotorDelayMessage.set(RELAY_OFF));
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
		if (message.getInt())
		{
			if (!tank02HighLevel && !tank03HighLevel)
				sendUpdate(message.getInt());
			else
			{
				send(waterMotorDelayMessage.set(RELAY_ON));
				Alarm.timerOnce(FIVE_MINUTES, checkTankWaterLevel);
				delayStartSet = true;
			}
			send(sumpMotorMessage.set(RELAY_ON));
		}
		else
		{
			sendUpdate(message.getInt());
			delayStartSet = false;
		}
		break;
	case V_VAR2:
		tank02HighLevel = message.getInt();
		break;
	case V_VAR5:
		tank03HighLevel = message.getInt();
		break;
	}
}

void sendUpdate(int currentState)
{
	digitalWrite(WATER_RELAY_PIN, currentState ? RELAY_ON : RELAY_OFF);
	digitalWrite(MOTOR_STATUS_PIN, currentState ? RELAY_ON : RELAY_OFF);
	send(waterMotorRelayMessage.set(currentState));
	Alarm.delay(WAIT_10MS);
}

void checkTankWaterLevel()
{
	if (motorDelayCheckCount == 5)
		delayStartSet = false;

	if (delayStartSet)
	{
		motorDelayCheckCount++;
		if (!tank02HighLevel && !tank03HighLevel)
		{
			sendUpdate(RELAY_ON);
			delayStartSet = false;
		}
		else
		{
			Alarm.timerOnce(FIVE_MINUTES, checkTankWaterLevel);
		}
	}
	else
	{
		send(waterMotorDelayMessage.set(RELAY_OFF));
	}
}

void keypadEvent(KeypadEvent key)
{
	MyMessage sumpMotorMessage(SUMP_RELAY_ID, V_STATUS);
	MyMessage tank03TimerMessage(SUMP_RELAY_ID, V_VAR5);
	tank03TimerMessage.setDestination(UNDERGROUND_NODE_ID);
	tank03TimerMessage.setType(V_VAR5); 
	switch (keypad.getState())
	{
	case PRESSED:
		switch (key)
		{
		case '1':
			if (!tank02HighLevel && !tank03HighLevel)
			{
				sendUpdate(RELAY_ON);
				Alarm.delay(WAIT_10MS);
				send(tank03TimerMessage.set(RELAY_ON));
			}
			else
			{
				send(waterMotorDelayMessage.set(RELAY_ON));
				Alarm.timerOnce(FIVE_MINUTES, checkTankWaterLevel);
			}
			sumpMotorMessage.setDestination(SUMP_RELAY_NODE_ID);
			send(sumpMotorMessage.set(RELAY_ON));
			break;
		case '2':
			sendUpdate(RELAY_OFF);
			Alarm.delay(WAIT_10MS);
			send(tank03TimerMessage.set(RELAY_OFF));
			delayStartSet = false;
			break;
		}
		break;
	case HOLD:
		switch (key)
		{
		case '1':
			if (!tank02HighLevel && !tank03HighLevel)
			{
				sendUpdate(RELAY_ON);
				Alarm.delay(WAIT_10MS);
				send(tank03TimerMessage.set(RELAY_ON));
			}
			else
			{
				send(waterMotorDelayMessage.set(RELAY_ON));
				Alarm.timerOnce(FIVE_MINUTES, checkTankWaterLevel);
			}
			sumpMotorMessage.setDestination(SUMP_RELAY_NODE_ID);
			send(sumpMotorMessage.set(RELAY_ON));
			break;
		case '2':
			sendUpdate(RELAY_OFF);
			Alarm.delay(WAIT_10MS);
			send(tank03TimerMessage.set(RELAY_OFF));
			delayStartSet = false;
			break;
		}
		break;
	}
}