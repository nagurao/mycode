#include <TimeAlarms.h>
#include <TimeLib.h>
#include <Time.h>
#include <SPI.h>

#define REMOTE_CONTROLLER_NODE
#define NODE_INTERACTS_WITH_RELAY

#define MY_RADIO_NRF24
#define MY_REPEATER_FEATURE
#define MY_NODE_ID REMOTE_CONTROLLER_NODE_ID
#define MY_DEBUG

#include <MyNodes.h>
#include <MySensors.h>
#include <MyConfig.h>

#define APPLICATION_NAME "Remote Controller"
#define APPLICATION_VERSION "24Dec2016"

byte keyPressed;
byte prevKeyPressed;

AlarmId resetPrevKeyTimer;
AlarmId heartbeatTimer;

MyMessage borewellNodeMessage;
MyMessage sumpMotorMessage(RELAY_ID, V_STATUS);
MyMessage tapWaterMotorMessage(RELAY_ID, V_STATUS);

void before()
{

}

void setup()
{
	pinMode(SDA_PIN, INPUT);
	pinMode(SCL_PIN, OUTPUT);
	keyPressed = 0;
	prevKeyPressed = 0;
	resetPrevKeyTimer = Alarm.timerRepeat(FIVE_MINUTES, resetPrevKeyPressed);
	heartbeatTimer = Alarm.timerRepeat(HEARTBEAT_INTERVAL, sendHeartbeat);
}

void presentation()
{
	sendSketchInfo(APPLICATION_NAME, APPLICATION_VERSION);
}

void loop()
{
	keyPressed = readKeypad();
	if (keyPressed && prevKeyPressed != keyPressed)
	{
		switch (keyPressed)
		{
		case 1:
			break;
		case 2:
			break;
		case 3:
			break;
		case 4:
			break;
		case 5:
			sumpMotorMessage.setDestination(SUMP_MOTOR_NODE_ID);
			sumpMotorMessage.setSensor(RELAY_ID);
			sumpMotorMessage.set(RELAY_ON);
			send(sumpMotorMessage);
			break;
		case 6:
			sumpMotorMessage.setDestination(SUMP_MOTOR_NODE_ID);
			sumpMotorMessage.setSensor(RELAY_ID);
			sumpMotorMessage.set(RELAY_OFF);
			send(sumpMotorMessage);
			break;
		case 7:
			borewellNodeMessage.setDestination(BOREWELL_NODE_ID);
			borewellNodeMessage.setSensor(BORE_ON_RELAY_ID);
			borewellNodeMessage.setType(V_STATUS);
			borewellNodeMessage.set(RELAY_ON);
			send(borewellNodeMessage);
			break;
		case 8:
			borewellNodeMessage.setDestination(BOREWELL_NODE_ID);
			borewellNodeMessage.setSensor(BORE_OFF_RELAY_ID);
			borewellNodeMessage.setType(V_STATUS);
			borewellNodeMessage.set(RELAY_ON);
			send(borewellNodeMessage);
			break;
		case 9:
			tapWaterMotorMessage.setDestination(TAP_MOTOR_NODE_ID);
			tapWaterMotorMessage.setSensor(RELAY_ID);
			tapWaterMotorMessage.set(RELAY_ON);
			send(tapWaterMotorMessage);
			break;
		case 10:
			tapWaterMotorMessage.setDestination(TAP_MOTOR_NODE_ID);
			tapWaterMotorMessage.setSensor(RELAY_ID);
			tapWaterMotorMessage.set(RELAY_OFF);
			send(tapWaterMotorMessage);
			break;
		case 13:
			break;
		case 14:
			break;
		}
		prevKeyPressed = keyPressed;
	}

	
	Alarm.delay(1);
}
void receive(const MyMessage &message)
{

}

byte readKeypad()
{
	byte currentPressedKey = 0;
	for (byte keyIndex = 1; keyIndex <= KEYPAD_SIZE; keyIndex++)
	{
		digitalWrite(SCL_PIN, LOW);
		if (!digitalRead(SDA_PIN))
			currentPressedKey = keyIndex;
		digitalWrite(SCL_PIN, HIGH);
	}
	Alarm.delay(KEYPAD_READ_INTERVAL);
	return currentPressedKey;
}

void resetPrevKeyPressed()
{
	prevKeyPressed = 0;
}