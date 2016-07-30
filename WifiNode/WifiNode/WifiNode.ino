#define PRINT_DEBUG_MESSAGES
#define PRINT_HTTP
#include <ESP8266WiFi.h>
#include <TimeAlarms.h>
#include <TimeLib.h>
#include <Time.h>
#include <ThingSpeak.h>
#include <SPI.h>

#define WIFI_NODE
#define MY_RADIO_NRF24
#define MY_REPEATER_FEATURE
#define MY_NODE_ID THINGSPEAK_NODE_ID
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
#define THINGSPEAK_INTERVAL 20

int status = WL_IDLE_STATUS;
WiFiClient  client;

unsigned long myChannelNumber = 140352;
const char * myWriteAPIKey = "E1Y9BE8CO5E7J8WR";
const char * myReadAPIKey = "4LVCQYHL7A58MOTU";

int channelData[8] = { -1,-1,-1,-1,-1,-1,-1,-1 };
byte channelId;
AlarmId heartbeatTimer;
AlarmId thingspeakTimer;

void before()
{
	WiFi.begin(ssid, pass);
	ThingSpeak.begin(client);
}

void setup()
{
	heartbeatTimer = Alarm.timerRepeat(HEARTBEAT_INTERVAL, sendHeartbeat);
	thingspeakTimer = Alarm.timerRepeat(THINGSPEAK_INTERVAL, sendDataToThingspeak);
	channelId = 0;
}

void presentation()
{
	sendSketchInfo(APPLICATION_NAME, APPLICATION_VERSION);
	present(WIFI_NODEMCU_ID, S_CUSTOM, "Channel Data");
}

void loop()
{
	Alarm.delay(1);
}

void receive(const MyMessage &message)
{
	switch (message.type)
	{
	case V_CUSTOM: 
		switch (message.sender)
		{
		case BALCONYLIGHT_WITH_PIR_NODE:
			channelData[BALCONY_LIGHTS_FIELD - 1] = message.getInt();
			break;
		case STAIRCASE_LIGHT_NODE:
			channelData[STAIRCASE_LIGHTS_FIELD - 1] = message.getInt();
			break;
		case GATELIGHT_WITH_PIR_NODE:
			channelData[GATE_LIGHTS_FIELD - 1] = message.getInt();
			break;
		case OVERHEAD_TANK_02_NODE_ID:
			channelData[TANK02_LEVEL_FIELD - 1] = message.getInt();
			break;
		}
		break;
	}
}

void sendDataToThingspeak()
{
	boolean channelDataNotFound = true;
	byte startPos = channelId;
	while (channelDataNotFound)
	{
		if (channelData[channelId] != -1)
		{
			switch (channelId + 1)
			{
				case BALCONY_LIGHTS_FIELD:
					if (ThingSpeak.writeField(myChannelNumber, BALCONY_LIGHTS_FIELD, channelData[channelId], myWriteAPIKey) == OK_SUCCESS)
					{
						channelData[channelId] = -1;
						channelDataNotFound = false;
					}
					break;
				case STAIRCASE_LIGHTS_FIELD:
					if (ThingSpeak.writeField(myChannelNumber, STAIRCASE_LIGHTS_FIELD, channelData[channelId], myWriteAPIKey) == OK_SUCCESS)
					{
						channelData[channelId] = -1;
						channelDataNotFound = false;
					}
					break;
				case GATE_LIGHTS_FIELD:
					if (ThingSpeak.writeField(myChannelNumber, GATE_LIGHTS_FIELD, channelData[channelId], myWriteAPIKey) == OK_SUCCESS)
					{
						channelData[channelId] = -1;
						channelDataNotFound = false;
					}
					break;
				case TANK01_LEVEL_FIELD:
					if (ThingSpeak.writeField(myChannelNumber, TANK01_LEVEL_FIELD, channelData[channelId], myWriteAPIKey) == OK_SUCCESS)
					{
						channelData[channelId] = -1;
						channelDataNotFound = false;
					}
					break;
				case TANK02_LEVEL_FIELD:
					if (ThingSpeak.writeField(myChannelNumber, TANK02_LEVEL_FIELD, channelData[channelId], myWriteAPIKey) == OK_SUCCESS)
					{
						channelData[channelId] = -1;
						channelDataNotFound = false;
					}
					break;
				case TANK03_LEVEL_FIELD:
					if (ThingSpeak.writeField(myChannelNumber, TANK03_LEVEL_FIELD, channelData[channelId], myWriteAPIKey) == OK_SUCCESS)
					{
						channelData[channelId] = -1;
						channelDataNotFound = false;
					}
					break;
				case SUMP_MOTOR_FIELD:
					if (ThingSpeak.writeField(myChannelNumber, SUMP_MOTOR_FIELD, channelData[channelId], myWriteAPIKey) == OK_SUCCESS)
					{
						channelData[channelId] = -1;
						channelDataNotFound = false;
					}
					break;
				case BOREWELL_FIELD:
					if (ThingSpeak.writeField(myChannelNumber, BOREWELL_FIELD, channelData[channelId], myWriteAPIKey) == OK_SUCCESS)
					{
						channelData[channelId] = -1;
						channelDataNotFound = false;
					}
					break;
			}
		}
		channelId = (channelId + 1) % 8;
		if (channelId == startPos)
			channelDataNotFound = false;

	}
}