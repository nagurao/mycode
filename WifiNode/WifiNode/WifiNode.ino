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

//static data logging channel
#define BALCONY_LIGHTS_FIELD 1
#define STAIRCASE_LIGHTS_FIELD 2
#define GATE_LIGHTS_FIELD 3
#define SUMP_MOTOR_FIELD 4
#define BOREWELL_FIELD 5

//real-time logging channel
#define OVERHEAD_TANK01_FIELD 1
#define OVERHEAD_TANK02_FIELD 2
#define SUMP_TANK_FIELD 3
#define PHASE3_METER_FIELD 4
#define PHASE1_METER_FIELD 5
#define SOLAR_VOLT_FIELD 6
#define BATTERY_VOLT_FIELD 7

#define BALCONY_LIGHTS_IDX 0
#define STAIRCASE_LIGHTS_IDX 1
#define GATE_LIGHTS_IDX 2
#define SUMP_MOTOR_IDX 3
#define BOREWELL_MOTOR_IDX 4
#define OVERHEAD_TANK01_IDX 5
#define OVERHEAD_TANK02_IDX 6
#define SUMP_TANK_IDX 7
#define PHASE3_METER_IDX 8
#define PHASE1_METER_IDX 9
#define SOLAR_VOLTAGE_IDX 10
#define BATTERY_VOLTAGE_IDX 11

#define THINGSPEAK_INTERVAL 20


int status = WL_IDLE_STATUS;
WiFiClient  client;

unsigned long myChannelNumber = 140352;
const char * myWriteAPIKey = "E1Y9BE8CO5E7J8WR";
const char * myReadAPIKey = "4LVCQYHL7A58MOTU";

unsigned long realtimeChannelNumber = 141630;
const char * myrealtimeWriteAPIKey = "QC5K9DN9COI9P4KU";
const char * myrealtimeReadAPIKey = "X3JM31N6KQRNUG2P";

long channelData[12] = { -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1 };
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
			channelData[BALCONY_LIGHTS_IDX] = message.getInt();
			break;
		case STAIRCASE_LIGHT_NODE:
			channelData[STAIRCASE_LIGHTS_IDX] = message.getInt();
			break;
		case GATELIGHT_WITH_PIR_NODE:
			channelData[GATE_LIGHTS_IDX] = message.getInt();
			break;
		case SUMP_RELAY_NODE_ID:
			channelData[SUMP_MOTOR_IDX] = message.getInt();
			break;
		case BOREWELL_RELAY_NODE_ID:
			channelData[BOREWELL_MOTOR_IDX] = message.getInt();
			break;
		case OVERHEAD_TANK_01_NODE_ID:
			channelData[OVERHEAD_TANK01_IDX] = message.getFloat();
			break;
		case OVERHEAD_TANK_02_NODE_ID:
			channelData[OVERHEAD_TANK02_IDX] = message.getFloat();
			break;
		case UNDERGROUND_NODE_ID:
			channelData[SUMP_TANK_IDX] = message.getFloat();
			break;
		case PH3_NODE_ID:
			channelData[PHASE3_METER_IDX] = message.getLong();
			break;
		case PH1_NODE_ID:
			channelData[PHASE1_METER_IDX] = message.getLong();
			break;
		case SOLAR_VOLTAGE_NODE_ID:
			channelData[SOLAR_VOLTAGE_IDX] = message.getFloat();
			break;
		case BATT_VOLTAGE_NODE_ID:
			channelData[BATTERY_VOLTAGE_IDX] = message.getFloat();
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
			switch (channelId)
			{
				case BALCONY_LIGHTS_IDX:
					if (ThingSpeak.writeField(myChannelNumber, BALCONY_LIGHTS_FIELD, channelData[channelId], myWriteAPIKey) == OK_SUCCESS)
					{
						channelData[channelId] = -1;
						channelDataNotFound = false;
					}
					break;
				case STAIRCASE_LIGHTS_IDX:
					if (ThingSpeak.writeField(myChannelNumber, STAIRCASE_LIGHTS_FIELD, channelData[channelId], myWriteAPIKey) == OK_SUCCESS)
					{
						channelData[channelId] = -1;
						channelDataNotFound = false;
					}
					break;
				case GATE_LIGHTS_IDX:
					if (ThingSpeak.writeField(myChannelNumber, GATE_LIGHTS_FIELD, channelData[channelId], myWriteAPIKey) == OK_SUCCESS)
					{
						channelData[channelId] = -1;
						channelDataNotFound = false;
					}
					break;
				case SUMP_MOTOR_IDX:
					if (ThingSpeak.writeField(myChannelNumber, SUMP_MOTOR_FIELD, channelData[channelId], myWriteAPIKey) == OK_SUCCESS)
					{
						channelData[channelId] = -1;
						channelDataNotFound = false;
					}
					break;
				case BOREWELL_MOTOR_IDX:
					if (ThingSpeak.writeField(myChannelNumber, BOREWELL_FIELD, channelData[channelId], myWriteAPIKey) == OK_SUCCESS)
					{
						channelData[channelId] = -1;
						channelDataNotFound = false;
					}
					break;
				case OVERHEAD_TANK01_IDX:
					if (ThingSpeak.writeField(realtimeChannelNumber, OVERHEAD_TANK01_FIELD, channelData[channelId], myrealtimeWriteAPIKey) == OK_SUCCESS)
					{
						channelData[channelId] = -1;
						channelDataNotFound = false;
					}
					break;
				case OVERHEAD_TANK02_IDX:
					if (ThingSpeak.writeField(realtimeChannelNumber, OVERHEAD_TANK02_FIELD, channelData[channelId], myrealtimeWriteAPIKey) == OK_SUCCESS)
					{
						channelData[channelId] = -1;
						channelDataNotFound = false;
					}
					break;
				case SUMP_TANK_IDX:
					if (ThingSpeak.writeField(realtimeChannelNumber, SUMP_TANK_FIELD, channelData[channelId], myrealtimeWriteAPIKey) == OK_SUCCESS)
					{
						channelData[channelId] = -1;
						channelDataNotFound = false;
					}
					break;
				case PHASE3_METER_IDX:
					if (ThingSpeak.writeField(realtimeChannelNumber, PHASE3_METER_FIELD, channelData[channelId], myrealtimeWriteAPIKey) == OK_SUCCESS)
					{
						channelData[channelId] = -1;
						channelDataNotFound = false;
					}
					break;
				case PHASE1_METER_IDX:
					if (ThingSpeak.writeField(realtimeChannelNumber, PHASE1_METER_FIELD, channelData[channelId], myrealtimeWriteAPIKey) == OK_SUCCESS)
					{
						channelData[channelId] = -1;
						channelDataNotFound = false;
					}
					break;
				case SOLAR_VOLTAGE_IDX:
					if (ThingSpeak.writeField(realtimeChannelNumber, SOLAR_VOLT_FIELD, channelData[channelId], myrealtimeWriteAPIKey) == OK_SUCCESS)
					{
						channelData[channelId] = -1;
						channelDataNotFound = false;
					}
					break;
				case BATTERY_VOLTAGE_IDX:
					if (ThingSpeak.writeField(realtimeChannelNumber, BATTERY_VOLT_FIELD, channelData[channelId], myrealtimeWriteAPIKey) == OK_SUCCESS)
					{
						channelData[channelId] = -1;
						channelDataNotFound = false;
					}
					break;
			}
		}
		channelId = (channelId + 1) % 12;
		if (channelId == startPos)
			channelDataNotFound = false;

	}
}