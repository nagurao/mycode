#include <Keypad.h>
#include <TimeAlarms.h>
#include <TimeLib.h>
#include <Time.h>
#include <SPI.h>

#define BOREWELL_NODE
#define NODE_WITH_ON_OFF_FEATURE
#define NODE_HAS_RELAY
#define WATER_TANK_NODE_IDS
#define KEYPAD_1R_2C
#define OVERHEAD_TANK_01_NODE

#define MY_RADIO_NRF24
#define MY_REPEATER_FEATURE
#define MY_NODE_ID BOREWELL_RELAY_NODE_ID
#define MY_DEBUG 

#include <MyNodes.h>
#include <MySensors.h>
#include <MyConfig.h>

#define APPLICATION_NAME "Borewell Motor"
#define APPLICATION_VERSION "20Nov2016"

AlarmId heartbeatTimer;
AlarmId dryRunTimer;

boolean borewellOn;
boolean tank01LowLevel;
boolean tank01HighLevel;

int currentWaterLevel;
int dryRunInitWaterLevel;

MyMessage thingspeakMessage(WIFI_NODEMCU_ID, V_CUSTOM);
MyMessage borewellMotorMessage(BOREWELL_MOTOR_ID, V_STATUS);
MyMessage borewellMotorOnRelayMessage(BORE_ON_RELAY_ID, V_STATUS);
MyMessage borewellMotorOffRelayMessage(BORE_OFF_RELAY_ID, V_STATUS);
MyMessage pollTimerMessage(BOREWELL_MOTOR_ID, V_VAR2);

Keypad keypad = Keypad(makeKeymap(keys), rowsPins, colsPins, ROWS, COLS);

void before()
{
	pinMode(BORE_ON_RELAY_PIN, OUTPUT);
	pinMode(BORE_OFF_RELAY_PIN, OUTPUT);
	pinMode(MOTOR_STATUS_PIN, OUTPUT);
}

void setup()
{
	keypad.addEventListener(keypadEvent);
	keypad.setDebounceTime(WAIT_50MS);
	borewellOn = false;
	tank01LowLevel = false;
	tank01HighLevel = false;
	currentWaterLevel = 0;
	dryRunInitWaterLevel = 0;

	digitalWrite(BORE_ON_RELAY_PIN, LOW);
	digitalWrite(BORE_OFF_RELAY_PIN, LOW);
	digitalWrite(MOTOR_STATUS_PIN, LOW);

	thingspeakMessage.setDestination(THINGSPEAK_NODE_ID);
	thingspeakMessage.setType(V_CUSTOM);
	thingspeakMessage.setSensor(WIFI_NODEMCU_ID);

	pollTimerMessage.setDestination(OVERHEAD_TANK_01_NODE_ID);
	pollTimerMessage.setType(V_VAR2);

	heartbeatTimer = Alarm.timerRepeat(HEARTBEAT_INTERVAL, sendHeartbeat);
	dryRunTimer = Alarm.timerRepeat(DRY_RUN_POLL_DURATION, checkCurrWaterLevel);
	Alarm.disable(dryRunTimer);
}

void presentation()
{
	sendSketchInfo(APPLICATION_NAME, APPLICATION_VERSION);
	Alarm.delay(WAIT_AFTER_SEND_MESSAGE);
	present(BOREWELL_MOTOR_ID, S_BINARY, "Borewell Motor");
	Alarm.delay(WAIT_AFTER_SEND_MESSAGE);
	present(BORE_ON_RELAY_ID, S_BINARY, "Bore On Relay");
	Alarm.delay(WAIT_AFTER_SEND_MESSAGE);
	present(BORE_OFF_RELAY_ID, S_BINARY, "Bore Off Relay");
	Alarm.delay(WAIT_AFTER_SEND_MESSAGE);

	send(borewellMotorMessage.set(RELAY_OFF));
	Alarm.delay(WAIT_AFTER_SEND_MESSAGE);
	send(borewellMotorOnRelayMessage.set(RELAY_OFF));
	Alarm.delay(WAIT_AFTER_SEND_MESSAGE);
	send(borewellMotorOffRelayMessage.set(RELAY_OFF));
	Alarm.delay(WAIT_AFTER_SEND_MESSAGE);
	send(thingspeakMessage.set(RELAY_OFF));
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
		switch (message.sensor)
		{
		case BORE_ON_RELAY_ID:
			if (!tank01HighLevel && !borewellOn)
				turnOnBorewell();
			break;
		case BORE_OFF_RELAY_ID:
			if(borewellOn)
				turnOffBorewell();
			break;
		}
		break;
	case V_VAR2:
		if (message.getInt())
			tank01LowLevel = ON;
		else
			tank01LowLevel = OFF;

		if (tank01LowLevel && !borewellOn)
			turnOnBorewell();

		break;
	case V_VAR3:
		if (message.getInt())
			tank01HighLevel = ON;
		else
			tank01HighLevel = OFF;

		if (tank01HighLevel && borewellOn)
			turnOffBorewell();
	case V_VOLUME:
		currentWaterLevel = message.getInt();
	}
}

void turnOnBorewell()
{
	digitalWrite(BORE_ON_RELAY_PIN, RELAY_ON);
	send(borewellMotorOnRelayMessage.set(RELAY_ON));
	Alarm.timerOnce(RELAY_TRIGGER_INTERVAL, toggleOnRelay);
	Alarm.delay(WAIT_AFTER_SEND_MESSAGE);
}

void toggleOnRelay()
{
	digitalWrite(BORE_ON_RELAY_PIN, RELAY_OFF);
	send(borewellMotorOnRelayMessage.set(RELAY_OFF));
	Alarm.delay(WAIT_AFTER_SEND_MESSAGE);
	send(borewellMotorMessage.set(RELAY_ON));
	Alarm.delay(WAIT_AFTER_SEND_MESSAGE);
	send(thingspeakMessage.set(RELAY_ON));
	Alarm.delay(WAIT_AFTER_SEND_MESSAGE);
	digitalWrite(MOTOR_STATUS_PIN, RELAY_ON);
	borewellOn = true;
	send(pollTimerMessage.set(ON));
	Alarm.delay(WAIT_AFTER_SEND_MESSAGE);
	dryRunInitWaterLevel = currentWaterLevel;
	Alarm.enable(dryRunTimer);
	
}

void turnOffBorewell()
{
	digitalWrite(BORE_OFF_RELAY_PIN, RELAY_ON);
	send(borewellMotorOffRelayMessage.set(RELAY_ON));
	Alarm.timerOnce(RELAY_TRIGGER_INTERVAL, toggleOffRelay);
	Alarm.delay(WAIT_AFTER_SEND_MESSAGE);
}

void toggleOffRelay()
{
	digitalWrite(BORE_OFF_RELAY_PIN, RELAY_OFF);
	send(borewellMotorOffRelayMessage.set(RELAY_OFF));
	Alarm.delay(WAIT_AFTER_SEND_MESSAGE);
	send(borewellMotorMessage.set(RELAY_OFF));
	Alarm.delay(WAIT_AFTER_SEND_MESSAGE);
	send(thingspeakMessage.set(RELAY_OFF));
	Alarm.delay(WAIT_AFTER_SEND_MESSAGE);
	digitalWrite(MOTOR_STATUS_PIN, RELAY_OFF);
	borewellOn = false;
	send(pollTimerMessage.set(OFF));
	Alarm.delay(WAIT_AFTER_SEND_MESSAGE);
	Alarm.disable(dryRunTimer);
}

void checkCurrWaterLevel()
{
	if (currentWaterLevel <= dryRunInitWaterLevel)
		turnOffBorewell();
	else
		dryRunInitWaterLevel = currentWaterLevel;
}

void keypadEvent(KeypadEvent key)
{
	switch (keypad.getState())
	{
	case PRESSED:
		switch (key)
		{
		case '1':
			if (!tank01HighLevel && !borewellOn)
			{
				turnOnBorewell();
			}
			break;
		case '2':
			if (borewellOn)
			{
				turnOffBorewell();
			}
			break;
		}
		break;
	case HOLD:
		switch (key)
		{
		case '1':
			if (!tank01HighLevel && !borewellOn)
			{
				turnOnBorewell();
			}
			break;
		case '2':
			if (borewellOn)
			{
				turnOffBorewell();
			}
			break;
		}
		break;
	}
}