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
#define APPLICATION_VERSION "13Dec2016"

//water level logging channel
#define TANK_01_FIELD 1
#define TANK_02_FILED 2
#define TANK_03_FIELD 3
//
#define TANK_01_IDX 0
#define TANK_02_IDX 1
#define TANK_03_IDX 2

//power cumulative logging channel
#define PH3_HOURLY_FIELD 1
#define PH3_DAILY_FIELD 2
#define PH3_MONTHLY_FIELD 3
#define PH1_HOURLY_FIELD 4
#define PH1_DAILY_FIELD 5
#define PH1_MONTHLY_FIELD 6
#define PH3_PH1_DELTA_DAILY_FIELD 7
//
#define PH3_HOURLY_IDX 0
#define PH3_DAILY_IDX 1
#define PH3_MONTHLY_IDX 2
#define PH1_HOURLY_IDX 3
#define PH1_DAILY_IDX 4
#define PH1_MONTHLY_IDX 5
#define PH3_PH1_DELTA_DAILY_IDX 6

//realtime power logging channel
#define PH3_CURR_WATT_FIELD 1
#define PH1_CURR_WATT_FIELD 2
#define PH3_PH1_REAL_TIME_DELTA_FIELD 3
//
#define PH3_CURR_WATT_IDX 0
#define PH1_CURR_WATT_IDX 1
#define PH3_PH1_REAL_TIME_DELTA_IDX 2

//static data logging channel
#define BALCONY_LIGHTS_FIELD 1
#define STAIRCASE_LIGHTS_FIELD 2
#define GATE_LIGHTS_FIELD 3
#define BOREWELL_FIELD 4
#define SUMP_MOTOR_FIELD 5
//
#define BALCONY_LIGHTS_IDX 0
#define STAIRCASE_LIGHTS_IDX 1
#define GATE_LIGHTS_IDX 2
#define BOREWELL_IDX 3
#define SUMP_MOTOR_IDX 4

//battery & solar voltage
#define BATTERY_VOLT_FIELD 1
#define SOLAR_VOLT_FIELD 2
//
#define BATTERY_VOLT_IDX 0
#define SOLAR_VOLT_IDX 1

#define THINGSPEAK_INTERVAL 20
#define DEFAULT_CHANNEL_VALUE -99.00

#define SEND_WATER_LEVEL_DATA 0
#define SEND_POWER_CUMMLATIVE_DATA 1
#define SEND_POWER_REALTIME_DATA 2
#define SEND_STATIC_DATA 3
#define SEND_VOLTAGE_DATA 4

#define TYPES_OF_DATA 5

#define FIELDS_PER_CHANNEL 8
byte currentDataToSend;

float waterLevelChannelData[FIELDS_PER_CHANNEL];
float powerCumulativeChannelData[FIELDS_PER_CHANNEL];
float powerRealtimeChannelData[FIELDS_PER_CHANNEL];
float staticChannelData[FIELDS_PER_CHANNEL];
float voltageChannelData[FIELDS_PER_CHANNEL];

int status = WL_IDLE_STATUS;
WiFiClient  client;

unsigned long waterLevelChannelNumber = 203220;
const char * waterLevelWriteAPIKey = "Q9OTOMNCUK5E8W00";
const char * waterLevelReadAPIKey = "ZN1BBM94RJAULC1S";

unsigned long powerCumulativeChannelNumber = 203222;
const char * powerCumulativeWriteAPIKey = "33QK8SG054YRX6CB";
const char * powerCumulativeReadAPIKey = "Z7LPAEKBJO56APU9";

unsigned long powerRealtimeChannelNumber = 203224;
const char * powerRealtimeWriteAPIKey = "L6WT1ZVB7POJE947";
const char * powerRealtimeReadAPIKey = "48M7YBUGQFVBEZ3Q";

unsigned long staticChannelNumber = 203226;
const char * staticWriteAPIKey = "O0VIVXGA0Q7Q3HI1";
const char * staticReadAPIKey = "4BJLHZM72HFNF71R";

unsigned long voltageChannelNumber = 203228;
const char * voltageWriteAPIKey = "DD8U0IJQVH32AXR3";
const char * voltageReadAPIKey = "KFFC286643GSSAAT";

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
	for (byte channelId = 0; channelId < FIELDS_PER_CHANNEL; channelId++)
	{
		waterLevelChannelData[channelId] = DEFAULT_CHANNEL_VALUE;
		powerCumulativeChannelData[channelId] = DEFAULT_CHANNEL_VALUE;
		powerRealtimeChannelData[channelId] = DEFAULT_CHANNEL_VALUE;
		staticChannelData[channelId] = DEFAULT_CHANNEL_VALUE;
		voltageChannelData[channelId] = DEFAULT_CHANNEL_VALUE;
	}
	currentDataToSend = SEND_WATER_LEVEL_DATA;
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
		case BALCONYLIGHT_NODE_ID:
			staticChannelData[BALCONY_LIGHTS_IDX] = message.getInt();
			break;
		case STAIRCASE_LIGHT_NODE_ID:
			staticChannelData[STAIRCASE_LIGHTS_IDX] = message.getInt();
			break;
		case GATELIGHT_NODE_ID:
			staticChannelData[GATE_LIGHTS_IDX] = message.getInt();
			break;
		case TANK_01_NODE_ID:
			waterLevelChannelData[TANK_01_IDX] = message.getInt();
			break;
		case TANK_02_NODE_ID:
			waterLevelChannelData[TANK_02_IDX] = message.getInt();
			break;
		case TANK_03_NODE_ID:
			waterLevelChannelData[TANK_03_IDX] = message.getInt();
			break;
		case BOREWELL_NODE_ID:
			staticChannelData[BOREWELL_IDX] = message.getInt();
			break;
		case SUMP_MOTOR_NODE_ID:
			staticChannelData[SUMP_MOTOR_IDX] = message.getInt();
			break;
		case BATT_VOLTAGE_NODE_ID:
			switch (message.sensor)
			{
			case BATTERY_VOLTAGE_ID:
				voltageChannelData[BATTERY_VOLT_IDX] = message.getFloat();
				break;
			case SOLAR_VOLTAGE_ID:
				voltageChannelData[SOLAR_VOLT_IDX] = message.getFloat();
				break;
			}
			break;
		case SOLAR_VOLTAGE_NODE_ID:
			break;
		case PH3_NODE_ID:
			switch (message.sensor)
			{
			case CURR_WATT_ID:
				powerRealtimeChannelData[PH3_CURR_WATT_IDX] = message.getFloat();
				break;
			case HOURLY_WATT_CONSUMPTION_ID:
				powerCumulativeChannelData[PH3_HOURLY_IDX] = message.getFloat();
				break;
			case DAILY_WATT_CONSUMPTION_ID:
				powerCumulativeChannelData[PH3_DAILY_IDX] = message.getFloat();
				break;
			case MONTHLY_WATT_CONSUMPTION_ID:
				powerCumulativeChannelData[PH3_MONTHLY_IDX] = message.getFloat();
				break;
			case DELTA_WATT_CONSUMPTION_ID:
				powerCumulativeChannelData[PH3_PH1_DELTA_DAILY_IDX] = message.getFloat();
				break;
			}
			break;
		case PH1_NODE_ID:
			switch (message.sensor)
			{
			case CURR_WATT_ID:
				powerRealtimeChannelData[PH1_CURR_WATT_FIELD] = message.getFloat();
				break;
			case HOURLY_WATT_CONSUMPTION_ID:
				powerCumulativeChannelData[PH1_HOURLY_IDX] = message.getFloat();
				break;
			case DAILY_WATT_CONSUMPTION_ID:
				powerCumulativeChannelData[PH1_DAILY_IDX] = message.getFloat();
				break;
			case MONTHLY_WATT_CONSUMPTION_ID:
				powerCumulativeChannelData[PH1_MONTHLY_IDX] = message.getFloat();
				break;
			case DELTA_WATT_CONSUMPTION_ID:
				powerRealtimeChannelData[PH3_PH1_REAL_TIME_DELTA_FIELD] = message.getFloat();
				break;
			}
			break;
		}
		break;
	}
}

void sendDataToThingspeak()
{
	byte channelId;
	switch (currentDataToSend)
	{
	case SEND_WATER_LEVEL_DATA:
		for (channelId = 0; channelId <= FIELDS_PER_CHANNEL; channelId++)
		{
			if (waterLevelChannelData[channelId] != DEFAULT_CHANNEL_VALUE)
			{
				switch (channelId)
				{
				case TANK_01_IDX:
					ThingSpeak.setField(TANK_01_FIELD, waterLevelChannelData[channelId]);
					break;
				case TANK_02_IDX:
					ThingSpeak.setField(TANK_02_FILED, waterLevelChannelData[channelId]);
					break;
				case TANK_03_IDX:
					ThingSpeak.setField(TANK_03_FIELD, waterLevelChannelData[channelId]);
					break;
				}
			}
		}
		if (ThingSpeak.writeFields(waterLevelChannelNumber, waterLevelWriteAPIKey) == OK_SUCCESS)
		{
			for (channelId = 0; channelId <= FIELDS_PER_CHANNEL; channelId++)
			{
				if (waterLevelChannelData[channelId] != DEFAULT_CHANNEL_VALUE)
					waterLevelChannelData[channelId] = DEFAULT_CHANNEL_VALUE;
			}
		}
		break;
	case SEND_POWER_CUMMLATIVE_DATA:
		for (channelId = 0; channelId <= FIELDS_PER_CHANNEL; channelId++)
		{
			if (powerCumulativeChannelData[channelId] != DEFAULT_CHANNEL_VALUE)
			{
				switch (channelId)
				{
				case PH3_HOURLY_IDX:
					ThingSpeak.setField(PH3_HOURLY_FIELD, powerCumulativeChannelData[channelId]);
					break;
				case PH3_DAILY_IDX:
					ThingSpeak.setField(PH3_DAILY_FIELD, powerCumulativeChannelData[channelId]);
					break;
				case PH3_MONTHLY_IDX:
					ThingSpeak.setField(PH3_MONTHLY_FIELD, powerCumulativeChannelData[channelId]);
					break;
				case PH1_HOURLY_IDX:
					ThingSpeak.setField(PH1_HOURLY_FIELD, powerCumulativeChannelData[channelId]);
					break;
				case PH1_DAILY_IDX:
					ThingSpeak.setField(PH1_DAILY_FIELD, powerCumulativeChannelData[channelId]);
					break;
				case PH1_MONTHLY_IDX:
					ThingSpeak.setField(PH1_MONTHLY_FIELD, powerCumulativeChannelData[channelId]);
					break;
				case PH3_PH1_DELTA_DAILY_IDX:
					ThingSpeak.setField(PH3_PH1_DELTA_DAILY_FIELD, powerCumulativeChannelData[channelId]);
					break;
				}
			}
		}
		if (ThingSpeak.writeFields(powerCumulativeChannelNumber, powerCumulativeWriteAPIKey) == OK_SUCCESS)
		{
			for (channelId = 0; channelId <= FIELDS_PER_CHANNEL; channelId++)
			{
				if (powerCumulativeChannelData[channelId] != DEFAULT_CHANNEL_VALUE)
					powerCumulativeChannelData[channelId] = DEFAULT_CHANNEL_VALUE;
			}
		}
		break;
	case SEND_POWER_REALTIME_DATA:
		for (channelId = 0; channelId <= FIELDS_PER_CHANNEL; channelId++)
		{
			if (powerRealtimeChannelData[channelId] != DEFAULT_CHANNEL_VALUE)
			{
				switch (channelId)
				{
				case PH3_CURR_WATT_IDX:
					ThingSpeak.setField(PH3_CURR_WATT_FIELD, powerRealtimeChannelData[channelId]);
					break;
				case PH1_CURR_WATT_IDX:
					ThingSpeak.setField(PH1_CURR_WATT_FIELD, powerRealtimeChannelData[channelId]);
					break;
				case PH3_PH1_REAL_TIME_DELTA_IDX:
					ThingSpeak.setField(PH3_PH1_REAL_TIME_DELTA_FIELD, powerRealtimeChannelData[channelId]);
					break;
				}
			}
		}
		if (ThingSpeak.writeFields(powerRealtimeChannelNumber, powerRealtimeWriteAPIKey) == OK_SUCCESS)
		{
			for (channelId = 0; channelId <= FIELDS_PER_CHANNEL; channelId++)
			{
				if (powerRealtimeChannelData[channelId] != DEFAULT_CHANNEL_VALUE)
					powerRealtimeChannelData[channelId] = DEFAULT_CHANNEL_VALUE;
			}
		}
		break;
	case SEND_STATIC_DATA:
		for (channelId = 0; channelId <= FIELDS_PER_CHANNEL; channelId++)
		{
			if (staticChannelData[channelId] != DEFAULT_CHANNEL_VALUE)
			{
				switch (channelId)
				{
				case BALCONY_LIGHTS_IDX:
					ThingSpeak.setField(BALCONY_LIGHTS_FIELD, staticChannelData[channelId]);
					break;
				case STAIRCASE_LIGHTS_IDX:
					ThingSpeak.setField(STAIRCASE_LIGHTS_FIELD, staticChannelData[channelId]);
					break;
				case GATE_LIGHTS_IDX:
					ThingSpeak.setField(GATE_LIGHTS_FIELD, staticChannelData[channelId]);
					break;
				case BOREWELL_IDX:
					ThingSpeak.setField(BOREWELL_FIELD, staticChannelData[channelId]);
					break;
				case SUMP_MOTOR_IDX:
					ThingSpeak.setField(SUMP_MOTOR_FIELD, staticChannelData[channelId]);
					break;
				}
			}
		}
		if (ThingSpeak.writeFields(staticChannelNumber, staticWriteAPIKey) == OK_SUCCESS)
		{
			for (channelId = 0; channelId <= FIELDS_PER_CHANNEL; channelId++)
			{
				if (staticChannelData[channelId] != DEFAULT_CHANNEL_VALUE)
					staticChannelData[channelId] = DEFAULT_CHANNEL_VALUE;
			}
		}
		break;
	case SEND_VOLTAGE_DATA:
		for (channelId = 0; channelId <= FIELDS_PER_CHANNEL; channelId++)
		{
			if (voltageChannelData[channelId] != DEFAULT_CHANNEL_VALUE)
			{
				switch (channelId)
				{
				case BATTERY_VOLT_IDX:
					ThingSpeak.setField(BATTERY_VOLT_FIELD, voltageChannelData[channelId]);
					break;
				case SOLAR_VOLT_IDX:
					ThingSpeak.setField(SOLAR_VOLT_FIELD, voltageChannelData[channelId]);
					break;
				}
			}
		}
		if (ThingSpeak.writeFields(voltageChannelNumber, voltageWriteAPIKey) == OK_SUCCESS)
		{
			for (channelId = 0; channelId <= FIELDS_PER_CHANNEL; channelId++)
			{
				if (voltageChannelData[channelId] != DEFAULT_CHANNEL_VALUE)
					voltageChannelData[channelId] = DEFAULT_CHANNEL_VALUE;
			}
		}
		break;
	}
	currentDataToSend = (currentDataToSend + 1) % TYPES_OF_DATA;
}