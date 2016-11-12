#include <Keypad.h>
#include <SPI.h>
#include <TimeAlarms.h>
#include <TimeLib.h>
#include <Time.h>

#define SUMP_MOTOR_NODE
#define NODE_HAS_RELAY
#define KEYPAD_1R_2C
#define WATER_TANK_NODE_IDS

#define MY_RADIO_NRF24
#define MY_REPEATER_FEATURE
#define MY_NODE_ID SUMP_RELAY_NODE_ID
#define MY_DEBUG 

#include <MyNodes.h>
#include <MySensors.h>
#include <MyConfig.h>

#define APPLICATION_NAME "Sump Motor"
#define APPLICATION_VERSION "12Nov2016"

AlarmId heartbeatTimer;
boolean tank02HighLevel;
boolean tank03LowLevel;
boolean tank03HighLevel;

MyMessage thingspeakMessage(WIFI_NODEMCU_ID, V_CUSTOM);
MyMessage sumpMotorRelayMessage(SUMP_RELAY_ID, V_STATUS);
MyMessage tank02SumpMotorMessage;

Keypad keypad = Keypad(makeKeymap(keys), rowsPins, colsPins, ROWS, COLS);


void before()
{
	pinMode(SUMP_RELAY_PIN, OUTPUT);
}

void setup()
{
	keypad.addEventListener(keypadEvent);
	digitalWrite(SUMP_RELAY_PIN, LOW);
	thingspeakMessage.setDestination(THINGSPEAK_NODE_ID);
	thingspeakMessage.setType(V_CUSTOM);
	thingspeakMessage.setSensor(WIFI_NODEMCU_ID);
	tank02SumpMotorMessage.setDestination(OVERHEAD_TANK_02_NODE_ID);
	tank02HighLevel = false;
	tank03LowLevel = false;
	tank03HighLevel = false;
	heartbeatTimer = Alarm.timerRepeat(HEARTBEAT_INTERVAL, sendHeartbeat);
}

void presentation()
{
	sendSketchInfo(APPLICATION_NAME, APPLICATION_VERSION);
	present(SUMP_RELAY_ID, S_BINARY, "Sump Motor Relay");
	send(sumpMotorRelayMessage.set(RELAY_OFF));
	send(thingspeakMessage.set(RELAY_OFF));
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
			if(!tank02HighLevel && !tank03LowLevel)
				sendUpdate(message.getInt());
		}
		else
			sendUpdate(message.getInt());
		break;
	case V_VAR2:
		tank02HighLevel = message.getInt();
		if(tank02HighLevel)
			sendUpdate(RELAY_OFF);
		break;
	case V_VAR3:
		tank03LowLevel = message.getInt();
		if (tank03LowLevel)
			sendUpdate(RELAY_OFF);
		break;
	case V_VAR4:
		tank02SumpMotorMessage.setSensor(SUMP_RELAY_ID);
		tank02SumpMotorMessage.setType(V_VAR4);
		send(tank02SumpMotorMessage.set(digitalRead(SUMP_RELAY_PIN)));
		break;
	case V_VAR5:
		tank03HighLevel = message.getInt();
		if(tank03HighLevel && !tank02HighLevel)
			sendUpdate(RELAY_ON);
	}
}

void sendUpdate(int currentState)
{
	digitalWrite(SUMP_RELAY_PIN, currentState ? RELAY_ON : RELAY_OFF);
	send(sumpMotorRelayMessage.set(currentState));
	Alarm.delay(WAIT_10MS);
	send(thingspeakMessage.set(currentState));
}

void keypadEvent(KeypadEvent key)
{
	MyMessage tank02TimerMessage(SUMP_RELAY_ID, V_VAR5);
	tank02TimerMessage.setDestination(OVERHEAD_TANK_02_NODE_ID);
	tank02TimerMessage.setType(V_VAR5);
	switch (keypad.getState())
	{
	case PRESSED:
		switch (key)
		{
		case '1':
			if (!tank02HighLevel && !tank03LowLevel)
			{
				sendUpdate(RELAY_ON);
				Alarm.delay(WAIT_10MS);
				send(tank02TimerMessage.set(RELAY_ON));
			}
			break;
		case '2':
			sendUpdate(RELAY_OFF);
			Alarm.delay(WAIT_10MS);
			send(tank02TimerMessage.set(RELAY_OFF));
			break;
		}
		break;
	case HOLD:
		switch (key)
		{
		case '1':
			if (!tank02HighLevel && !tank03LowLevel)
			{
				sendUpdate(RELAY_ON);
				Alarm.delay(WAIT_10MS);
				send(tank02TimerMessage.set(RELAY_ON));
			}
			break;
		case '2':
			sendUpdate(RELAY_OFF);
			Alarm.delay(WAIT_10MS);
			send(tank02TimerMessage.set(RELAY_OFF));
			break;
		}
		break;
	}
}