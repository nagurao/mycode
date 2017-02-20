#define PRINT_DEBUG_MESSAGES
#define PRINT_HTTP
#include <ESP8266WiFi.h>
#include <WiFiClient.h>

#include <TimeAlarms.h>
#include <TimeLib.h>
#include <Time.h>
#include <ThingSpeak.h>
#include <SPI.h>

/*
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPUpdateServer.h>
*/
#define WIFI_NODE
#define WATT_METER_NODE
#define SOLAR_BATT_VOLTAGE_NODE
#define NODE_INTERACTS_WITH_RELAY
#define NODE_INTERACTS_WITH_WIFI_AND_LCD

#define MY_RADIO_NRF24
//#define MY_REPEATER_FEATURE
#define MY_NODE_ID THINGSPEAK_NODE_ID
#define MY_DEBUG

#include <MyNodes.h>
#include <MySensors.h>
#include <MyConfig.h>

#define APPLICATION_NAME "Thingspeak Node"

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

//inverter cumulative logging channel
#define INV_IN_HOURLY_FIELD 1
#define INV_IN_DAILY_FIELD 2
#define INV_IN_MONTHLY_FIELD 3
#define INV_OUT_HOURLY_FIELD 4
#define INV_OUT_DAILY_FIELD 5
#define INV_OUT_MONTHLY_FIELD 6
#define INV_IN_OUT_DELTA_DAILY_FIELD 7
//
#define INV_IN_HOURLY_IDX 0
#define INV_IN_DAILY_IDX 1
#define INV_IN_MONTHLY_IDX 2
#define INV_OUT_HOURLY_IDX 3
#define INV_OUT_DAILY_IDX 4
#define INV_OUT_MONTHLY_IDX 5
#define INV_IN_OUT_DELTA_DAILY_IDX 6

//inverter power logging channel
#define INV_IN_CURR_WATT_FIELD 1
#define INV_OUT_CURR_WATT_FIELD 2
#define INV_IN_OUT_REAL_TIME_DELTA_FIELD 3
//
#define INV_IN_CURR_WATT_IDX 0
#define INV_OUT_CURR_WATT_IDX 1
#define INV_IN_OUT_REAL_TIME_DELTA_IDX 2

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

#define IN_BALCONY_LIGHT_OPER_MODE_IDX 0
#define IN_GATE_LIGHT_OPER_MODE_IDX 1
#define IN_BOREWELL_ON_IDX 2
#define IN_BOREWELL_OFF_IDX 3
#define IN_SUMP_MOTOR_IDX 4

#define THINGSPEAK_INTERVAL 60
#define DEFAULT_CHANNEL_VALUE -99.00
#define DEFAULT_CHANNEL_VALUE_INT 0

#define GET_INCOMING_DATA  0
#define SEND_POWER_CUMMLATIVE_DATA 1
#define SEND_POWER_REALTIME_DATA 2
#define SEND_INVETER_CUMMLATIVE_DATA 3
#define SEND_INVETER_REALTIME_DATA 4
#define SEND_VOLTAGE_DATA 5
#define SEND_WATER_LEVEL_DATA 6
#define SEND_STATIC_DATA 7

#define REQ_INV_IN_CURR_WATT 8
#define REQ_INV_OUT_CURR_WATT 9
#define REQ_INV_IN_OUT_CURR_DELTA 10
#define REQ_3PHASE_CURR_WATT 11
#define REQ_1PHASE_CURR_WATT 12
#define REQ_PH3_PH1_CURR_DELTA 13
#define TYPES_OF_DATA 8

#define FIELDS_PER_CHANNEL 8

byte currentDataToSend;

float waterLevelChannelData[FIELDS_PER_CHANNEL];
float powerCumulativeChannelData[FIELDS_PER_CHANNEL];
float powerRealtimeChannelData[FIELDS_PER_CHANNEL];
float staticChannelData[FIELDS_PER_CHANNEL];
float voltageChannelData[FIELDS_PER_CHANNEL];
float inverterCumulativeChannelData[FIELDS_PER_CHANNEL];
float inverterRealtimeChannelData[FIELDS_PER_CHANNEL];
int incomingChannelData[FIELDS_PER_CHANNEL];

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

unsigned long inverterCumulativeChannelNumber = 217341;
const char * inverterCumulativeWriteAPIKey = "PMRIK4ECEHZSDWB9";
const char * inverterCumulativeReadAPIKey = "U81EQ15HQUXOUOFM";

unsigned long inverterRealtimeChannelNumber = 217342;
const char * inverterRealtimeWriteAPIKey = "F3U8OCLMQUX9QLHV";
const char * inverterRealtimeReadAPIKey = "CNGCV1JI5E8L71ZU";

unsigned long incomingChannelNumber = 203604;
const char * incomingWriteAPIKey = "NGZUM98NSGJ4EAG9";
const char * incomingReadAPIKey = "81B9VBW1WKVPVIZK";

boolean incomingDataFound;

AlarmId heartbeatTimer;
AlarmId thingspeakTimer;
AlarmId incomingThingspeakTimer;

MyMessage lightNodeMessage(CURR_MODE_ID, V_VAR1);
MyMessage borewellNodeMessage;
MyMessage sumpMotorMessage(RELAY_ID, V_STATUS);
MyMessage lcdNodeMessage;
MyMessage currWattMessage;
MyMessage currDeltaMessage;

/*
const char* host = "esp8266-webupdate";
const char* update_path = "/firmware";
const char* update_username = "admin";
const char* update_password = "admin"; 
ESP8266WebServer httpServer(80);
ESP8266HTTPUpdateServer httpUpdater;*/
//
void before()
{
	//WiFi.mode(WIFI_AP_STA);
	WiFi.begin(ssid, pass);
	ThingSpeak.begin(client);
	//MDNS.begin(host);
	//httpUpdater.setup(&httpServer, update_path, update_username, update_password);
	//httpServer.begin();
	//MDNS.addService("http", "tcp", 80);
}

void setup()
{
	heartbeatTimer = Alarm.timerRepeat(HEARTBEAT_INTERVAL, sendHeartbeat);
	thingspeakTimer = Alarm.timerRepeat(THINGSPEAK_INTERVAL, processThingspeakData);

	for (byte channelId = 0; channelId < FIELDS_PER_CHANNEL; channelId++)
	{
		waterLevelChannelData[channelId] = DEFAULT_CHANNEL_VALUE;
		powerCumulativeChannelData[channelId] = DEFAULT_CHANNEL_VALUE;
		powerRealtimeChannelData[channelId] = DEFAULT_CHANNEL_VALUE;
		inverterCumulativeChannelData[channelId] = DEFAULT_CHANNEL_VALUE;
		inverterRealtimeChannelData[channelId] = DEFAULT_CHANNEL_VALUE;
		staticChannelData[channelId] = DEFAULT_CHANNEL_VALUE;
		voltageChannelData[channelId] = DEFAULT_CHANNEL_VALUE;
		incomingChannelData[channelId] = DEFAULT_CHANNEL_VALUE_INT;
	}
	currentDataToSend = SEND_POWER_CUMMLATIVE_DATA;
	incomingDataFound = false;
	lcdNodeMessage.setDestination(LCD_NODE_ID);
}

void presentation()
{
	sendSketchInfo(APPLICATION_NAME, __DATE__);
}

void loop()
{
	//httpServer.handleClient();
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
				lcdNodeMessage.setSensor(BATTERY_VOLTAGE_ID);
				lcdNodeMessage.setType(V_VOLTAGE);
				lcdNodeMessage.set(voltageChannelData[BATTERY_VOLT_IDX], 2);
				send(lcdNodeMessage);
				wait(WAIT_AFTER_SEND_MESSAGE);
				break;
			case SOLAR_VOLTAGE_ID:
				voltageChannelData[SOLAR_VOLT_IDX] = message.getFloat();
				lcdNodeMessage.setSensor(SOLAR_VOLTAGE_ID);
				lcdNodeMessage.setType(V_VOLTAGE);
				lcdNodeMessage.set(voltageChannelData[SOLAR_VOLT_IDX], 2);
				send(lcdNodeMessage);
				wait(WAIT_AFTER_SEND_MESSAGE);
				break;
			}
			break;
		case SOLAR_VOLTAGE_NODE_ID:
			break;
		case INV_IN_NODE_ID:
			switch (message.sensor)
			{
			case CURR_WATT_ID:
				inverterRealtimeChannelData[INV_IN_CURR_WATT_IDX] = message.getFloat();
				lcdNodeMessage.setSensor(INV_IN_CURR_WATT_ID);
				lcdNodeMessage.setType(V_WATT);
				lcdNodeMessage.set(inverterRealtimeChannelData[INV_IN_CURR_WATT_IDX],2);
				send(lcdNodeMessage);
				wait(WAIT_AFTER_SEND_MESSAGE);
				break;
			case HOURLY_WATT_CONSUMPTION_ID:
				inverterCumulativeChannelData[INV_IN_HOURLY_IDX] = message.getFloat();
				break;
			case DAILY_WATT_CONSUMPTION_ID:
				inverterCumulativeChannelData[INV_IN_DAILY_IDX] = message.getFloat();
				break;
			case MONTHLY_WATT_CONSUMPTION_ID:
				inverterCumulativeChannelData[INV_IN_MONTHLY_IDX] = message.getFloat();
				break;
			case DELTA_WATT_CONSUMPTION_ID:
				inverterCumulativeChannelData[INV_IN_OUT_DELTA_DAILY_IDX] = message.getFloat();
				break;
			}
			break;
		case INV_OUT_NODE_ID:
			switch (message.sensor)
			{
			case CURR_WATT_ID:
				inverterRealtimeChannelData[INV_OUT_CURR_WATT_IDX] = message.getFloat();
				lcdNodeMessage.setSensor(INV_OUT_CURR_WATT_ID);
				lcdNodeMessage.setType(V_WATT);
				lcdNodeMessage.set(inverterRealtimeChannelData[INV_OUT_CURR_WATT_IDX], 2);
				send(lcdNodeMessage);
				wait(WAIT_AFTER_SEND_MESSAGE);
				break;
			case HOURLY_WATT_CONSUMPTION_ID:
				inverterCumulativeChannelData[INV_OUT_HOURLY_IDX] = message.getFloat();
				break;
			case DAILY_WATT_CONSUMPTION_ID:
				inverterCumulativeChannelData[INV_OUT_DAILY_IDX] = message.getFloat();
				break;
			case MONTHLY_WATT_CONSUMPTION_ID:
				inverterCumulativeChannelData[INV_OUT_MONTHLY_IDX] = message.getFloat();
				break;
			case DELTA_WATT_CONSUMPTION_ID:
				inverterRealtimeChannelData[INV_IN_OUT_REAL_TIME_DELTA_IDX] = message.getFloat();
				lcdNodeMessage.setSensor(INV_IN_OUT_DELTA_ID);
				lcdNodeMessage.setType(V_KWH);
				lcdNodeMessage.set(inverterRealtimeChannelData[INV_IN_OUT_REAL_TIME_DELTA_IDX], 2);
				send(lcdNodeMessage);
				wait(WAIT_AFTER_SEND_MESSAGE);
				break;
			}
			break;
		case PH3_NODE_ID:
			switch (message.sensor)
			{
			case CURR_WATT_ID:
				powerRealtimeChannelData[PH3_CURR_WATT_IDX] = message.getFloat();
				lcdNodeMessage.setSensor(PH3_CURR_WATT_ID);
				lcdNodeMessage.setType(V_WATT);
				lcdNodeMessage.set(powerRealtimeChannelData[PH3_CURR_WATT_IDX], 2);
				send(lcdNodeMessage);
				wait(WAIT_AFTER_SEND_MESSAGE);
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
				powerRealtimeChannelData[PH1_CURR_WATT_IDX] = message.getFloat();
				lcdNodeMessage.setSensor(PH1_CURR_WATT_ID);
				lcdNodeMessage.setType(V_WATT);
				lcdNodeMessage.set(powerRealtimeChannelData[PH1_CURR_WATT_IDX], 2);
				send(lcdNodeMessage);
				wait(WAIT_AFTER_SEND_MESSAGE);
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
				powerRealtimeChannelData[PH3_PH1_REAL_TIME_DELTA_IDX] = message.getFloat();
				lcdNodeMessage.setSensor(PH3_PH1_DELTA_ID);
				lcdNodeMessage.setType(V_KWH);
				lcdNodeMessage.set(powerRealtimeChannelData[PH3_PH1_REAL_TIME_DELTA_IDX], 2);
				send(lcdNodeMessage);
				wait(WAIT_AFTER_SEND_MESSAGE);
				break;
			}
			break;
		}
		break;
	}
}


void processThingspeakData()
{
	byte channelId;
	switch (currentDataToSend)
	{
	case GET_INCOMING_DATA:
		processIncomingData();
		break;
	case SEND_POWER_CUMMLATIVE_DATA:
		for (channelId = 0; channelId < FIELDS_PER_CHANNEL; channelId++)
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
			for (channelId = 0; channelId < FIELDS_PER_CHANNEL; channelId++)
			{
				if (powerCumulativeChannelData[channelId] != DEFAULT_CHANNEL_VALUE)
					powerCumulativeChannelData[channelId] = DEFAULT_CHANNEL_VALUE;
			}
		}
		break;
	case SEND_POWER_REALTIME_DATA:
		for (channelId = 0; channelId < FIELDS_PER_CHANNEL; channelId++)
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
			for (channelId = 0; channelId < FIELDS_PER_CHANNEL; channelId++)
			{
				if (powerRealtimeChannelData[channelId] != DEFAULT_CHANNEL_VALUE)
					powerRealtimeChannelData[channelId] = DEFAULT_CHANNEL_VALUE;
			}
		}
		break;
	case SEND_INVETER_CUMMLATIVE_DATA:
		for (channelId = 0; channelId < FIELDS_PER_CHANNEL; channelId++)
		{
			if (inverterCumulativeChannelData[channelId] != DEFAULT_CHANNEL_VALUE)
			{
				switch (channelId)
				{
				case INV_IN_HOURLY_IDX:
					ThingSpeak.setField(INV_IN_HOURLY_FIELD, inverterCumulativeChannelData[channelId]);
					break;
				case INV_IN_DAILY_IDX:
					ThingSpeak.setField(INV_IN_DAILY_FIELD, inverterCumulativeChannelData[channelId]);
					break;
				case INV_IN_MONTHLY_IDX:
					ThingSpeak.setField(INV_IN_MONTHLY_FIELD, inverterCumulativeChannelData[channelId]);
					break;
				case INV_OUT_HOURLY_IDX:
					ThingSpeak.setField(INV_OUT_HOURLY_FIELD, inverterCumulativeChannelData[channelId]);
					break;
				case INV_OUT_DAILY_IDX:
					ThingSpeak.setField(INV_OUT_DAILY_FIELD, inverterCumulativeChannelData[channelId]);
					break;
				case INV_OUT_MONTHLY_IDX:
					ThingSpeak.setField(INV_OUT_MONTHLY_FIELD, inverterCumulativeChannelData[channelId]);
					break;
				case INV_IN_OUT_DELTA_DAILY_IDX:
					ThingSpeak.setField(INV_IN_OUT_DELTA_DAILY_FIELD, inverterCumulativeChannelData[channelId]);
					break;
				}
			}
		}
		if (ThingSpeak.writeFields(inverterCumulativeChannelNumber, inverterCumulativeWriteAPIKey) == OK_SUCCESS)
		{
			for (channelId = 0; channelId < FIELDS_PER_CHANNEL; channelId++)
			{
				if (inverterCumulativeChannelData[channelId] != DEFAULT_CHANNEL_VALUE)
					inverterCumulativeChannelData[channelId] = DEFAULT_CHANNEL_VALUE;
			}
		}
		break;
	case SEND_INVETER_REALTIME_DATA:
		for (channelId = 0; channelId < FIELDS_PER_CHANNEL; channelId++)
		{
			if (inverterRealtimeChannelData[channelId] != DEFAULT_CHANNEL_VALUE)
			{
				switch (channelId)
				{
				case INV_IN_CURR_WATT_IDX:
					ThingSpeak.setField(INV_IN_CURR_WATT_FIELD, inverterRealtimeChannelData[channelId]);
					break;
				case INV_OUT_CURR_WATT_IDX:
					ThingSpeak.setField(INV_OUT_CURR_WATT_FIELD, inverterRealtimeChannelData[channelId]);
					break;
				case INV_IN_OUT_REAL_TIME_DELTA_IDX:
					ThingSpeak.setField(INV_IN_OUT_REAL_TIME_DELTA_FIELD, inverterRealtimeChannelData[channelId]);
					break;
				}
			}
		}
		if (ThingSpeak.writeFields(inverterRealtimeChannelNumber, inverterRealtimeWriteAPIKey) == OK_SUCCESS)
		{
			for (channelId = 0; channelId < FIELDS_PER_CHANNEL; channelId++)
			{
				if (inverterRealtimeChannelData[channelId] != DEFAULT_CHANNEL_VALUE)
					inverterRealtimeChannelData[channelId] = DEFAULT_CHANNEL_VALUE;
			}
		}
		break;
	case SEND_VOLTAGE_DATA:
		for (channelId = 0; channelId < FIELDS_PER_CHANNEL; channelId++)
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
			for (channelId = 0; channelId < FIELDS_PER_CHANNEL; channelId++)
			{
				if (voltageChannelData[channelId] != DEFAULT_CHANNEL_VALUE)
					voltageChannelData[channelId] = DEFAULT_CHANNEL_VALUE;
			}
		}
		break;
	case SEND_WATER_LEVEL_DATA:
		for (channelId = 0; channelId < FIELDS_PER_CHANNEL; channelId++)
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
			for (channelId = 0; channelId < FIELDS_PER_CHANNEL; channelId++)
			{
				if (waterLevelChannelData[channelId] != DEFAULT_CHANNEL_VALUE)
					waterLevelChannelData[channelId] = DEFAULT_CHANNEL_VALUE;
			}
		}
		break;
	case SEND_STATIC_DATA:
		for (channelId = 0; channelId < FIELDS_PER_CHANNEL; channelId++)
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
			for (channelId = 0; channelId < FIELDS_PER_CHANNEL; channelId++)
			{
				if (staticChannelData[channelId] != DEFAULT_CHANNEL_VALUE)
					staticChannelData[channelId] = DEFAULT_CHANNEL_VALUE;
			}
		}
		break;
	case REQ_INV_IN_CURR_WATT:
		currWattMessage.setDestination(INV_IN_NODE_ID);
		currWattMessage.setType(V_VAR5);
		currWattMessage.setSensor(CURR_WATT_ID);
		currWattMessage.set(0);
		send(currWattMessage);
		break;
	case REQ_INV_OUT_CURR_WATT:
		currWattMessage.setDestination(INV_OUT_NODE_ID);
		currWattMessage.setType(V_VAR5);
		currWattMessage.setSensor(CURR_WATT_ID);
		currWattMessage.set(0);
		send(currWattMessage);
		break;
	case REQ_INV_IN_OUT_CURR_DELTA:
		currDeltaMessage.setDestination(INV_OUT_NODE_ID);
		currDeltaMessage.setType(V_VAR5);
		currDeltaMessage.setSensor(DELTA_WATT_CONSUMPTION_ID);
		currDeltaMessage.set(5);
		send(currDeltaMessage);
		break;
	case REQ_3PHASE_CURR_WATT:
		currWattMessage.setDestination(PH3_NODE_ID);
		currWattMessage.setType(V_VAR5);
		currWattMessage.setSensor(CURR_WATT_ID);
		currWattMessage.set(0);
		send(currWattMessage);
		break;
	case REQ_1PHASE_CURR_WATT:
		currWattMessage.setDestination(PH1_NODE_ID);
		currWattMessage.setType(V_VAR5);
		currWattMessage.setSensor(CURR_WATT_ID);
		currWattMessage.set(0);
		send(currWattMessage);
		break;
	case REQ_PH3_PH1_CURR_DELTA:
		currDeltaMessage.setDestination(PH1_NODE_ID);
		currDeltaMessage.setType(V_VAR5);
		currDeltaMessage.setSensor(DELTA_WATT_CONSUMPTION_ID);
		currDeltaMessage.set(5);
		send(currDeltaMessage);
		break;
	}
	currentDataToSend = (currentDataToSend + 1) % TYPES_OF_DATA;
}

void processIncomingData()
{
	byte channelId;
	int valueToSend = 0;
	incomingDataFound = false;
	for (channelId = 0; channelId < FIELDS_PER_CHANNEL; channelId++)
	{
		incomingChannelData[channelId] = (int)ThingSpeak.readIntField(incomingChannelNumber, channelId + 1, incomingReadAPIKey);
		if (ThingSpeak.getLastReadStatus() != OK_SUCCESS)
			incomingChannelData[channelId] = DEFAULT_CHANNEL_VALUE_INT;
		if (ThingSpeak.getLastReadStatus() == OK_SUCCESS && (incomingChannelData[channelId] <= 0 ))// || incomingChannelData[channelId] > 2))
			incomingChannelData[channelId] = DEFAULT_CHANNEL_VALUE_INT;
		if (isDigit(incomingChannelData[channelId]))
		{
			incomingChannelData[channelId] = incomingChannelData[channelId] + '0';
			if(incomingChannelData[channelId] > 2)
				incomingChannelData[channelId] = DEFAULT_CHANNEL_VALUE_INT;
		}
	}

	for (channelId = 0; channelId < FIELDS_PER_CHANNEL; channelId++)
	{
		if (incomingChannelData[channelId] != DEFAULT_CHANNEL_VALUE_INT)
		{
			boolean inVal = incomingChannelData[channelId];
			incomingDataFound = true;
			switch (channelId)
			{
			case IN_BALCONY_LIGHT_OPER_MODE_IDX:
				lightNodeMessage.setDestination(BALCONYLIGHT_NODE_ID);
				lightNodeMessage.setSensor(CURR_MODE_ID);
				lightNodeMessage.setType(V_VAR1);
				switch (incomingChannelData[channelId])
				{
				case 1:
					lightNodeMessage.set(STANDBY_MODE);
					send(lightNodeMessage);
					break;
				case 2:
					lightNodeMessage.set(DUSKLIGHT_MODE);
					send(lightNodeMessage);
					break;
				}
				break;
			case IN_GATE_LIGHT_OPER_MODE_IDX:
				lightNodeMessage.setDestination(GATELIGHT_NODE_ID);
				lightNodeMessage.setSensor(CURR_MODE_ID);
				lightNodeMessage.setType(V_VAR1);
				lightNodeMessage.set((incomingChannelData[channelId] == 1) ? RELAY_ON : RELAY_OFF);
				send(lightNodeMessage);
				break;
			case IN_BOREWELL_ON_IDX:
				if (incomingChannelData[channelId] == 1)
				{
					borewellNodeMessage.setDestination(BOREWELL_NODE_ID);
					borewellNodeMessage.setSensor(BORE_ON_RELAY_ID);
					borewellNodeMessage.setType(V_STATUS);
					borewellNodeMessage.set(RELAY_ON);
					send(borewellNodeMessage);
				}
				break;
			case IN_BOREWELL_OFF_IDX:
				if (incomingChannelData[channelId] == 1)
				{
					borewellNodeMessage.setDestination(BOREWELL_NODE_ID);
					borewellNodeMessage.setSensor(BORE_OFF_RELAY_ID);
					borewellNodeMessage.setType(V_STATUS);
					borewellNodeMessage.set(RELAY_ON);
					send(borewellNodeMessage);
				}
				break;
			case IN_SUMP_MOTOR_IDX:
				sumpMotorMessage.setDestination(SUMP_MOTOR_NODE_ID);
				sumpMotorMessage.setSensor(RELAY_ID);
				sumpMotorMessage.set((incomingChannelData[channelId] == 1) ? (byte)RELAY_ON : (byte)RELAY_OFF);
				send(sumpMotorMessage);
				break;
			}
			ThingSpeak.setField(channelId + 1, DEFAULT_CHANNEL_VALUE_INT);
		}
	}
	if (incomingDataFound)
		ThingSpeak.writeFields(incomingChannelNumber, incomingWriteAPIKey);
}
