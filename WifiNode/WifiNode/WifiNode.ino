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
#define APPLICATION_VERSION "09Sep2016"

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
#define SOLAR_VOLT_FIELD 4
#define BATTERY_VOLT_FIELD 5

//power stat logging channel
#define HOURLY_PH3_FIELD 1
#define DAILY_PH3_FIELD 2
#define MONTHLY_PH3_FIELD 3
#define HOURLY_PH1_FIELD 4
#define DAILY_PH1_FIELD 5
#define MONTHLY_PH1_FIELD 6
#define DELTA_PH3_PH1_FIELD 7

#define BALCONY_LIGHTS_IDX 0
#define STAIRCASE_LIGHTS_IDX 1
#define GATE_LIGHTS_IDX 2
#define SUMP_MOTOR_IDX 3
#define BOREWELL_MOTOR_IDX 4
#define OVERHEAD_TANK01_IDX 5
#define OVERHEAD_TANK02_IDX 6
#define SUMP_TANK_IDX 7
#define SOLAR_VOLTAGE_IDX 8
#define BATTERY_VOLTAGE_IDX 9
#define HOURLY_3PH_IDX 10
#define DAILY_3PH_IDX 11
#define MONTHLY_3PH_IDX 12
#define HOURLY_1PH_IDX 13
#define DAILY_1PH_IDX 14
#define MONTHLY_1PH_IDX 15
#define DELTA_KWH_IDX 16

#define THINGSPEAK_INTERVAL 20

byte maxDataInputs = 17;
long channelData[17];

int status = WL_IDLE_STATUS;
WiFiClient  client;

unsigned long myChannelNumber = 140352;
const char * myWriteAPIKey = "E1Y9BE8CO5E7J8WR";
const char * myReadAPIKey = "4LVCQYHL7A58MOTU";

unsigned long realtimeChannelNumber = 141630;
const char * myrealtimeWriteAPIKey = "QC5K9DN9COI9P4KU";
const char * myrealtimeReadAPIKey = "X3JM31N6KQRNUG2P";

unsigned long powerStatChannelNumber = 141628;
const char * myPowerStatWriteAPIKey = "145BJIZPLWRVGREN";
const char * myPowerStatReadAPIKey = "Q44VV3W71YNRNN3I";



byte channelId;
AlarmId heartbeatTimer;
AlarmId thingspeakTimer;

void before()
{
	WiFi.begin(ssid, pass);
	ThingSpeak.begin(client);
	for(byte index = 0; index < maxDataInputs; index++)
		channelData[index]  = -99.99;
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
			switch (message.sensor)
			{

			}
			break;
		case PH1_NODE_ID:
			switch (message.sensor)
			{

			}
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
		if (channelData[channelId] != -99.99)
		{
			switch (channelId)
			{
				case BALCONY_LIGHTS_IDX:
					if (ThingSpeak.writeField(myChannelNumber, BALCONY_LIGHTS_FIELD, channelData[channelId], myWriteAPIKey) == OK_SUCCESS)
					{
						channelData[channelId] = -99.99;
						channelDataNotFound = false;
					}
					break;
				case STAIRCASE_LIGHTS_IDX:
					if (ThingSpeak.writeField(myChannelNumber, STAIRCASE_LIGHTS_FIELD, channelData[channelId], myWriteAPIKey) == OK_SUCCESS)
					{
						channelData[channelId] = -99.99;
						channelDataNotFound = false;
					}
					break;
				case GATE_LIGHTS_IDX:
					if (ThingSpeak.writeField(myChannelNumber, GATE_LIGHTS_FIELD, channelData[channelId], myWriteAPIKey) == OK_SUCCESS)
					{
						channelData[channelId] = -99.99;
						channelDataNotFound = false;
					}
					break;
				case SUMP_MOTOR_IDX:
					if (ThingSpeak.writeField(myChannelNumber, SUMP_MOTOR_FIELD, channelData[channelId], myWriteAPIKey) == OK_SUCCESS)
					{
						channelData[channelId] = -99.99;
						channelDataNotFound = false;
					}
					break;
				case BOREWELL_MOTOR_IDX:
					if (ThingSpeak.writeField(myChannelNumber, BOREWELL_FIELD, channelData[channelId], myWriteAPIKey) == OK_SUCCESS)
					{
						channelData[channelId] = -99.99;
						channelDataNotFound = false;
					}
					break;
				case OVERHEAD_TANK01_IDX:
					if (ThingSpeak.writeField(realtimeChannelNumber, OVERHEAD_TANK01_FIELD, channelData[channelId], myrealtimeWriteAPIKey) == OK_SUCCESS)
					{
						channelData[channelId] = -99.99;
						channelDataNotFound = false;
					}
					break;
				case OVERHEAD_TANK02_IDX:
					if (ThingSpeak.writeField(realtimeChannelNumber, OVERHEAD_TANK02_FIELD, channelData[channelId], myrealtimeWriteAPIKey) == OK_SUCCESS)
					{
						channelData[channelId] = -99.99;
						channelDataNotFound = false;
					}
					break;
				case SUMP_TANK_IDX:
					if (ThingSpeak.writeField(realtimeChannelNumber, SUMP_TANK_FIELD, channelData[channelId], myrealtimeWriteAPIKey) == OK_SUCCESS)
					{
						channelData[channelId] = -99.99;
						channelDataNotFound = false;
					}
					break;
				case SOLAR_VOLTAGE_IDX:
					if (ThingSpeak.writeField(realtimeChannelNumber, SOLAR_VOLT_FIELD, channelData[channelId], myrealtimeWriteAPIKey) == OK_SUCCESS)
					{
						channelData[channelId] = -99.99;
						channelDataNotFound = false;
					}
					break;
				case BATTERY_VOLTAGE_IDX:
					if (ThingSpeak.writeField(realtimeChannelNumber, BATTERY_VOLT_FIELD, channelData[channelId], myrealtimeWriteAPIKey) == OK_SUCCESS)
					{
						channelData[channelId] = -99.99;
						channelDataNotFound = false;
					}
					break;
				case HOURLY_3PH_IDX:
					if (ThingSpeak.writeField(powerStatChannelNumber, HOURLY_PH3_FIELD, channelData[channelId], myPowerStatWriteAPIKey) == OK_SUCCESS)
					{
						channelData[channelId] = -99.99;
						channelDataNotFound = false;
					}
					break;
				case DAILY_3PH_IDX:
					if (ThingSpeak.writeField(powerStatChannelNumber, DAILY_PH3_FIELD, channelData[channelId], myPowerStatWriteAPIKey) == OK_SUCCESS)
					{
						channelData[channelId] = -99.99;
						channelDataNotFound = false;
					}
					break;
				case MONTHLY_3PH_IDX:
					if (ThingSpeak.writeField(powerStatChannelNumber, MONTHLY_PH3_FIELD, channelData[channelId], myPowerStatWriteAPIKey) == OK_SUCCESS)
					{
						channelData[channelId] = -99.99;
						channelDataNotFound = false;
					}
					break;
				case HOURLY_1PH_IDX:
					if (ThingSpeak.writeField(powerStatChannelNumber, HOURLY_PH1_FIELD, channelData[channelId], myPowerStatWriteAPIKey) == OK_SUCCESS)
					{
						channelData[channelId] = -99.99;
						channelDataNotFound = false;
					}
					break;
				case DAILY_1PH_IDX:
					if (ThingSpeak.writeField(powerStatChannelNumber, DAILY_PH1_FIELD, channelData[channelId], myPowerStatWriteAPIKey) == OK_SUCCESS)
					{
						channelData[channelId] = -99.99;
						channelDataNotFound = false;
					}
					break;
				case MONTHLY_1PH_IDX:
					if (ThingSpeak.writeField(powerStatChannelNumber, MONTHLY_PH1_FIELD, channelData[channelId], myPowerStatWriteAPIKey) == OK_SUCCESS)
					{
						channelData[channelId] = -99.99;
						channelDataNotFound = false;
					}
					break;
				case DELTA_KWH_IDX:
					if (ThingSpeak.writeField(powerStatChannelNumber, DELTA_PH3_PH1_FIELD, channelData[channelId], myPowerStatWriteAPIKey) == OK_SUCCESS)
					{
						channelData[channelId] = -99.99;
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