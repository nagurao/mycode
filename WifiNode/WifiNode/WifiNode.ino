#define PRINT_DEBUG_MESSAGES
#define PRINT_HTTP
#include <ESP8266WiFi.h>
#include <TimeAlarms.h>
#include <TimeLib.h>
#include <Time.h>
#include <ThingSpeak.h>
#include <SPI.h>

#define WIFI_NODE
#define WATT_METER_NODE
#define SOLAR_BATT_VOLTAGE_NODE

#define MY_RADIO_NRF24
#define MY_REPEATER_FEATURE
#define MY_NODE_ID THINGSPEAK_NODE_ID
#define MY_DEBUG

#include <MyNodes.h>
#include <MySensors.h>
#include <MyConfig.h>

#define APPLICATION_NAME "Thingspeak Node"
#define APPLICATION_VERSION "25Sep2016"

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
#define CURR_WATT_PH3_FIELD 6
#define CURR_WATT_PH1_FIELD 7
#define REAL_TIME_DELTA_PH3_PH1_FIELD 8

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
//
#define OVERHEAD_TANK01_IDX 5
#define OVERHEAD_TANK02_IDX 6
#define SUMP_TANK_IDX 7
#define SOLAR_VOLTAGE_IDX 8
#define BATTERY_VOLTAGE_IDX 9
#define CURR_3PH_IDX 10
#define CURR_1PH_IDX 11
#define REAL_TIME_DELTA_KWH_IDX 12
//
#define HOURLY_3PH_IDX 13
#define DAILY_3PH_IDX 14
#define MONTHLY_3PH_IDX 15
#define HOURLY_1PH_IDX 16
#define DAILY_1PH_IDX 17
#define MONTHLY_1PH_IDX 18
#define DELTA_KWH_IDX 19

#define THINGSPEAK_INTERVAL 20
#define DEFAULT_CHANNEL_VALUE -99.00

#define SEND_STATIC_DATA 0
#define SEND_REAL_TIME_DATA 1
#define SEND_POWER_STAT_DATA 2

#define TYPES_OF_DATA 3
byte currentDataToSend;

byte maxDataInputs = 20;
float channelData[20];

int status = WL_IDLE_STATUS;
WiFiClient  client;

unsigned long staticChannelNumber = 140352;
const char * myStaticWriteAPIKey = "E1Y9BE8CO5E7J8WR";
const char * myStaticReadAPIKey = "4LVCQYHL7A58MOTU";

unsigned long realtimeChannelNumber = 141630;
const char * myrealtimeWriteAPIKey = "QC5K9DN9COI9P4KU";
const char * myrealtimeReadAPIKey = "X3JM31N6KQRNUG2P";

unsigned long powerStatChannelNumber = 141628;
const char * myPowerStatWriteAPIKey = "145BJIZPLWRVGREN";
const char * myPowerStatReadAPIKey = "Q44VV3W71YNRNN3I";


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
	for (byte channelId = 0; channelId < maxDataInputs; channelId++)
		channelData[channelId] = DEFAULT_CHANNEL_VALUE;
	currentDataToSend = SEND_STATIC_DATA;
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
			channelData[OVERHEAD_TANK01_IDX] = message.getInt();
			break;
		case OVERHEAD_TANK_02_NODE_ID:
			channelData[OVERHEAD_TANK02_IDX] = message.getInt();
			break;
		case UNDERGROUND_NODE_ID:
			channelData[SUMP_TANK_IDX] = message.getInt();
			break;
		case PH3_NODE_ID:
			switch (message.sensor)
			{
			case CURR_WATT_ID:
				channelData[CURR_3PH_IDX] = message.getFloat();
				break;
			case HOURLY_WATT_CONSUMPTION_ID:
				channelData[HOURLY_3PH_IDX] = message.getFloat();
				break;
			case DAILY_WATT_CONSUMPTION_ID:
				channelData[DAILY_3PH_IDX] = message.getFloat();
				break;
			case MONTHLY_WATT_CONSUMPTION_ID:
				channelData[MONTHLY_3PH_IDX] = message.getFloat();
				break;
			case DELTA_WATT_CONSUMPTION_ID:
				channelData[DELTA_KWH_IDX] = message.getFloat();
				break;
			}
			break;
		case PH1_NODE_ID:
			switch (message.sensor)
			{
			case CURR_WATT_ID:
				channelData[CURR_1PH_IDX] = message.getFloat();
				break;
			case HOURLY_WATT_CONSUMPTION_ID:
				channelData[HOURLY_1PH_IDX] = message.getFloat();
				break;
			case DAILY_WATT_CONSUMPTION_ID:
				channelData[DAILY_1PH_IDX] = message.getFloat();
				break;
			case MONTHLY_WATT_CONSUMPTION_ID:
				channelData[MONTHLY_1PH_IDX] = message.getFloat();
				break;
			case DELTA_WATT_CONSUMPTION_ID:
				channelData[REAL_TIME_DELTA_KWH_IDX] = message.getFloat();
				break;
			}
			break;
		case SOLAR_VOLTAGE_NODE_ID:
			break;
		case BATT_VOLTAGE_NODE_ID:
			switch (message.sensor)
			{
			case SOLAR_VOLTAGE_ID:
				channelData[SOLAR_VOLTAGE_IDX] = message.getFloat();
				break;
			case BATTERY_VOLTAGE_ID:
				channelData[BATTERY_VOLTAGE_IDX] = message.getFloat();
				break;
			}
			break;
		}
		break;
	}
}

void sendDataToThingspeak()
{
	switch (currentDataToSend)
	{
	case SEND_STATIC_DATA:
		for (byte channelId = BALCONY_LIGHTS_IDX; channelId <= BOREWELL_MOTOR_IDX; channelId++)
		{
			if (channelData[channelId] != DEFAULT_CHANNEL_VALUE)
			{
				switch (channelId)
				{
				case BALCONY_LIGHTS_IDX:
					ThingSpeak.setField(BALCONY_LIGHTS_FIELD, channelData[channelId]);
					break;
				case STAIRCASE_LIGHTS_IDX:
					ThingSpeak.setField(STAIRCASE_LIGHTS_FIELD, channelData[channelId]);
					break;
				case GATE_LIGHTS_IDX:
					ThingSpeak.setField(GATE_LIGHTS_FIELD, channelData[channelId]);
					break;
				case SUMP_MOTOR_IDX:
					ThingSpeak.setField(SUMP_MOTOR_FIELD, channelData[channelId]);
					break;
				case BOREWELL_MOTOR_IDX:
					ThingSpeak.setField(BOREWELL_FIELD, channelData[channelId]);
					break;
				}
			}
		}
		if (ThingSpeak.writeFields(staticChannelNumber, myStaticWriteAPIKey) == OK_SUCCESS)
		{
			for (byte channelId = BALCONY_LIGHTS_IDX; channelId <= BOREWELL_MOTOR_IDX; channelId++)
			{
				if (channelData[channelId] != DEFAULT_CHANNEL_VALUE)
					channelData[channelId] = DEFAULT_CHANNEL_VALUE;
			}
		}
		break;
	case SEND_REAL_TIME_DATA:
		for (byte channelId = OVERHEAD_TANK01_IDX; channelId <= REAL_TIME_DELTA_KWH_IDX; channelId++)
		{
			if (channelData[channelId] != DEFAULT_CHANNEL_VALUE)
			{
				switch (channelId)
				{
				case OVERHEAD_TANK01_IDX:
					ThingSpeak.setField(OVERHEAD_TANK01_FIELD, channelData[channelId]);
					break;
				case OVERHEAD_TANK02_IDX:
					ThingSpeak.setField(OVERHEAD_TANK02_FIELD, channelData[channelId]);
					break;
				case SUMP_TANK_IDX:
					ThingSpeak.setField(SUMP_TANK_FIELD, channelData[channelId]);
					break;
				case SOLAR_VOLTAGE_IDX:
					ThingSpeak.setField(SOLAR_VOLT_FIELD, channelData[channelId]);
					break;
				case BATTERY_VOLTAGE_IDX:
					ThingSpeak.setField(BATTERY_VOLT_FIELD, channelData[channelId]);
					break;
				case CURR_3PH_IDX:
					ThingSpeak.setField(CURR_WATT_PH3_FIELD, channelData[channelId]);
					break;
				case CURR_1PH_IDX:
					ThingSpeak.setField(CURR_WATT_PH1_FIELD, channelData[channelId]);
					break;
				case REAL_TIME_DELTA_KWH_IDX:
					ThingSpeak.setField(REAL_TIME_DELTA_PH3_PH1_FIELD, channelData[channelId]);
				}
			}
		}
		if (ThingSpeak.writeFields(realtimeChannelNumber, myrealtimeWriteAPIKey) == OK_SUCCESS)
		{
			for (byte channelId = OVERHEAD_TANK01_IDX; channelId <= CURR_1PH_IDX; channelId++)
			{
				if (channelData[channelId] != DEFAULT_CHANNEL_VALUE)
					channelData[channelId] = DEFAULT_CHANNEL_VALUE;
			}
		}
		break;
	case SEND_POWER_STAT_DATA:
		for (byte channelId = HOURLY_3PH_IDX; channelId <= DELTA_KWH_IDX; channelId++)
		{
			if (channelData[channelId] != DEFAULT_CHANNEL_VALUE)
			{
				switch (channelId)
				{
				case HOURLY_3PH_IDX:
					ThingSpeak.setField(HOURLY_PH3_FIELD, channelData[channelId]);
					break;
				case DAILY_3PH_IDX:
					ThingSpeak.setField(DAILY_PH3_FIELD, channelData[channelId]);
					break;
				case MONTHLY_3PH_IDX:
					ThingSpeak.setField(MONTHLY_PH3_FIELD, channelData[channelId]);
					break;
				case HOURLY_1PH_IDX:
					ThingSpeak.setField(HOURLY_PH1_FIELD, channelData[channelId]);
					break;
				case DAILY_1PH_IDX:
					ThingSpeak.setField(DAILY_PH1_FIELD, channelData[channelId]);
					break;
				case MONTHLY_1PH_IDX:
					ThingSpeak.setField(MONTHLY_PH1_FIELD, channelData[channelId]);
					break;
				case DELTA_KWH_IDX:
					ThingSpeak.setField(DELTA_PH3_PH1_FIELD, channelData[channelId]);
					break;
				}
			}
		}
		if (ThingSpeak.writeFields(powerStatChannelNumber, myPowerStatWriteAPIKey) == OK_SUCCESS)
		{
			for (byte channelId = HOURLY_3PH_IDX; channelId <= DELTA_KWH_IDX; channelId++)
			{
				if (channelData[channelId] != DEFAULT_CHANNEL_VALUE)
					channelData[channelId] = DEFAULT_CHANNEL_VALUE;
			}
		}
		break;
	}
	currentDataToSend = (currentDataToSend + 1) % TYPES_OF_DATA;
}