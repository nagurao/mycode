#include <ESP8266WiFi.h>
#include <TimeAlarms.h>
#include <TimeLib.h>
#include <Time.h>
#include <ThingSpeak.h>
#include <SPI.h>

#define WIFI_NODE

#define MY_RADIO_NRF24
#define MY_REPEATER_FEATURE
#define MY_NODE_ID THINGSPEAK_NODE
#define MY_DEBUG

#include <MyNodes.h>
#include <MySensors.h>
#include <MyConfig.h>

#define APPLICATION_NAME "Thingspeak Node"
#define APPLICATION_VERSION "29Jul2016"

#define BALCONY_LIGHTS_FIELD 1
#define STAIRCASE_LIGHTS_FIELD 2
#define GATE_LIGHTS_FIELD 3
#define TANK01_LEVEL_FIELD 4
#define TANK02_LEVEL_FIELD 5
#define TANK03_LEVEL_FIELD 6
#define SUMP_MOTOR_FIELD 7
#define BOREWELL_FIELD 8
#define THINGSPEAK_INTERVAL 20000

int status = WL_IDLE_STATUS;
WiFiClient  client;

unsigned long myChannelNumber = 140352;
const char * myWriteAPIKey = "E1Y9BE8CO5E7J8WR";
const char * myReadAPIKey = "4LVCQYHL7A58MOTU";

byte channelData[8] = { 0,0,0,0,0,0,0,0 };
byte channelId;
AlarmId heartbeatTimer;
AlarmId thingspeakTimer;

void before()
{
	WiFi.begin(ssid, pass);
}

void setup()
{
	ThingSpeak.begin(client);
	heartbeatTimer = Alarm.timerRepeat(HEARTBEAT_INTERVAL, sendHeartbeat);
	thingspeakTimer = Alarm.timerRepeat(THINGSPEAK_INTERVAL, sendDataToThingspeak);
	channelId = 0;
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
	case V_STATUS: 
		switch (message.sender)
		{
		case BALCONYLIGHT_WITH_PIR_NODE:
			channelData[BALCONY_LIGHTS_FIELD - 1] = message.getInt();
			break;
		case GATELIGHT_WITH_PIR_NODE:
			channelData[GATE_LIGHTS_FIELD - 1] = message.getInt();
			break;
		case STAIRCASE_LIGHT_NODE:
			channelData[STAIRCASE_LIGHTS_FIELD - 1] = message.getInt();
			break;
		case OVERHEAD_TANK_02_NODE_ID:
			channelData[TANK02_LEVEL_FIELD - 1] = message.getInt();
			break;
		}
		break;
	case V_VAR1:
		break;
	case V_VAR2:
		break;
	}
}

void sendDataToThingspeak()
{
	boolean channelDataFound = false;
	byte startPos = channelId;
	while (channelDataFound)
	{
		if (channelData[channelId] != 0)
		{
			switch (channelData[channelId])
			{
				case BALCONY_LIGHTS_FIELD:
					ThingSpeak.writeField(myChannelNumber, BALCONY_LIGHTS_FIELD, channelData[channelId], myWriteAPIKey);
					channelData[channelId] = 0;
					channelDataFound = true;
					break;
				case STAIRCASE_LIGHTS_FIELD:
					ThingSpeak.writeField(myChannelNumber, STAIRCASE_LIGHTS_FIELD, channelData[channelId], myWriteAPIKey);
					channelData[channelId] = 0;
					channelDataFound = true;
					break;
				case GATE_LIGHTS_FIELD:
					ThingSpeak.writeField(myChannelNumber, GATE_LIGHTS_FIELD, channelData[channelId], myWriteAPIKey);
					channelData[channelId] = 0;
					channelDataFound = true;
					break;
				case TANK01_LEVEL_FIELD:
					ThingSpeak.writeField(myChannelNumber, TANK01_LEVEL_FIELD, channelData[channelId], myWriteAPIKey);
					channelData[channelId] = 0;
					channelDataFound = true;
					break;
				case TANK02_LEVEL_FIELD:
					ThingSpeak.writeField(myChannelNumber, TANK02_LEVEL_FIELD, channelData[channelId], myWriteAPIKey);
					channelData[channelId] = 0;
					channelDataFound = true;
					break;
				case TANK03_LEVEL_FIELD:
					ThingSpeak.writeField(myChannelNumber, TANK03_LEVEL_FIELD, channelData[channelId], myWriteAPIKey);
					channelData[channelId] = 0;
					channelDataFound = true;
					break;
				case SUMP_MOTOR_FIELD:
					ThingSpeak.writeField(myChannelNumber, SUMP_MOTOR_FIELD, channelData[channelId], myWriteAPIKey);
					channelData[channelId] = 0;
					channelDataFound = true;
					break;
				case BOREWELL_FIELD:
					ThingSpeak.writeField(myChannelNumber, BOREWELL_FIELD, channelData[channelId], myWriteAPIKey);
					channelData[channelId] = 0;
					channelDataFound = true;
					break;
			}
		}
		channelId = (channelId + 1) % 8;
		if (channelId == startPos)
			channelDataFound = true;

	}
}