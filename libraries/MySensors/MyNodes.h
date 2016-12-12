/***
 *  This file defines the Sensor ids used in MySensors network.
 */
#ifndef MyNodes_h
#define MyNodes_h

#define WAIT_AFTER_SEND_MESSAGE 20
#define WAIT_5MS 5
#define WAIT_10MS 10
#define WAIT_50MS 50
#define ONE_MINUTE 60
#define FIVE_MINUTES 300
#define ONE_HOUR 3600
#define REQUEST_INTERVAL 20
#define HEARTBEAT_INTERVAL 600
/*
The following are the Node Ids assigned, populated here as comments for easy reference.
0 - Gateway
1 - LCD Node
2 - Controller
3 - Balcony Lights
4 - Staircase Lights
5 - Gate Lights
6 - Tank 01
7 - Borewell Motor
8 - Tank 02
9 - Sump Motor
10 - Tank 03
11 - Water Motor
12 - Battery Voltage
13 - Solar Voltage
14 - 3Phase Wattmeter
15 - 1Phase Wattmeter
254 - Wifi Node
*/

// NodeMCU Wifi Node details
#define THINGSPEAK_NODE_ID 254
#define WIFI_NODEMCU_ID 1

// Overhead Tank 01
#if defined TANK_01_NODE
#define TANK_01_NODE_ID 6
#define DRY_RUN_POLL_DURATION 900

#define BOREWELL_NODE
#define WATER_TANK_NODE
#define NODE_INTERACTS_WITH_RELAY
#define NODE_INTERACTS_WITH_LCD
#define NODE_WITH_ON_OFF_FEATURE

#endif

#if defined NODE_HAS_RELAY
#define RELAY_ON 1
#define RELAY_OFF 0
#endif

#if defined NODE_INTERACTS_WITH_RELAY
#define RELAY_ON 1
#define RELAY_OFF 0
#endif

#if defined NODE_WITH_ON_OFF_FEATURE
#define TURN_ON 1
#define TURN_OFF 0
#endif

#if defined NODE_INTERACTS_WITH_LCD
#define LCD_NODE_ID 1
#endif

#if defined KEYPAD_1R_2C
#define ROWS 1
#define COLS 2
#define ON '1'
#define OFF '2'

char keys[1][2] = { ON,OFF };
byte rowsPins[1] = { 6 };
byte colsPins[2] = { 4,5 };

#endif

#if defined WATT_METER_NODE

#define PH3_NODE_ID 10
#define PH1_NODE_ID 11

#define ZERO_PULSE 0
#define ACCUMULATION_FREQUENCY_SECS 5
#define MAX_WATT 10000

#define CURR_WATT_ID 1
#define HOURLY_WATT_CONSUMPTION_ID 2
#define DAILY_WATT_CONSUMPTION_ID 3
#define MONTHLY_WATT_CONSUMPTION_ID 4
#define ACCUMULATED_WATT_CONSUMPTION_ID 5
#define CURR_PULSE_COUNT_ID 6
#define BLINKS_PER_KWH_ID 7
#define RESET_TYPE_ID 8
#define DELTA_WATT_CONSUMPTION_ID 9

#define PULSE_SENSOR_PIN 3
#define INTERRUPT_PULSE 1 // PULSE_SENSOR_PIN - 2

#define RESET_NONE 0
#define RESET_HOUR 1
#define RESET_DAY 2
#define RESET_MONTH 3
#define RESET_ALL 4
#define ZERO 0

#define GET_HOURLY_KWH 0
#define GET_DAILY_KWH 1
#define GET_MONTHLY_KWH 2
#define ALL_DONE 3

#endif

#if defined NODE_HAS_ULTRASONIC_SENSOR
#define ECHO_PIN 5
#define TRIGGER_PIN 6
#define MAX_DISTANCE 300

#define OVERHEAD_TANK_02_NODE_ID 8
#define WATER_LEVEL_SENSOR_ID 1

#endif

#if defined MOTION_SENSOR_WITH_LIGHT
#define MOTION_SENSOR_PIN 3
#define INTERRUPT_MOTION 1 // MOTION_SENSOR_PIN - 2
#define LIGHT_RELAY_PIN 7

#define MOTION_SENSOR_ID 1
#define LIGHT_RELAY_ID 2
#define CURR_MODE_ID 3
#define LIGHT_DURATION_ID 4

#define STANDBY_MODE 0
#define DUSKLIGHT_MODE 1
#define SENSOR_MODE 2
#define ADHOC_MODE 3

#define MOTION_DETECTED 1
#define NO_MOTION_DETECTED 0

#define BALCONYLIGHT_WITH_PIR_NODE_ID 1
#define GATELIGHT_WITH_PIR_NODE_ID 3

#endif

#if defined LCD_NODE

#define LCD_NODE_ID 1
#define PH3_NODE_ID 14
#define PH1_NODE_ID 15
#define BATT_VOLTAGE_NODE_ID 12

#define BATTERY_VOLTAGE_ID 1
#define SOLAR_VOLTAGE_ID 2

#define WATER_TANK_NODE_IDS

#endif

#if defined REMOTE_CONTROLLER_NODE
#define REMOTE_CONTROLLER_NODE_ID 2
#endif

#if defined LIGHT_NODE

#define BALCONYLIGHT_NODE_ID 3
#define STAIRCASE_LIGHT_NODE_ID 4
#define GATELIGHT_NODE_ID 5

#define LIGHT_RELAY_ID 1
#define CURR_MODE_ID 2
#define LIGHT_DURATION_ID 3

#define STANDBY_MODE 0
#define DUSKLIGHT_MODE 1
#define ADHOC_MODE 2

#define LIGHT_RELAY_PIN 7

#endif

#if defined SUMP_MOTOR_NODE
#define SUMP_RELAY_NODE_ID 4
#define SUMP_RELAY_ID 1
#define SUMP_RELAY_PIN 7
#define CURR_WATER_LEVEL_ID 1
#define WATER_RELAY_NODE_ID 14
#endif

#if defined BOREWELL_NODE
#define BOREWELL_RELAY_NODE_ID 7
#define BOREWELL_MOTOR_ID 1
#define BORE_ON_RELAY_ID 2
#define BORE_OFF_RELAY_ID 3
#define BORE_ON_RELAY_PIN 7
#define BORE_OFF_RELAY_PIN 8
#define RELAY_TRIGGER_INTERVAL 3
#endif

#if defined WATER_MOTOR_NODE
#define WATER_RELAY_NODE_ID 14
#define WATER_RELAY_ID 1
#define  WATER_RELAY_PIN 7
#define MOTOR_STATUS_PIN 3

#define SUMP_RELAY_NODE_ID 4
#define SUMP_RELAY_ID 1

#endif

# if defined WATER_TANK_NODE_IDS
#define TANK_01_NODE_ID 6
#define TANK_02_NODE_ID 8
#define TANK_03_NODE_ID 10

#define MOTOR_STATUS_PIN 3
#endif

#if defined WATER_TANK_NODE
#define CURR_WATER_LEVEL_ID 1
#define WATER_LOW_LEVEL_IND_ID 2
#define WATER_FULL_LEVEL_ID 3
#define WATER_EMPTY_LEVEL_ID 4
#define DEFAULT_LOW_LEVEL 40
#define MAX_SENSORS 6
#define RISING_LEVEL_POLL_DURATION 30
#define DEFAULT_LEVEL_POLL_DURATION 300

#define SENSOR_1_PIN 2
#define SENSOR_2_PIN 3
#define SENSOR_3_PIN 4
#define SENSOR_4_PIN 5
#define SENSOR_5_PIN 6
#define SENSOR_6_PIN 7

#define LEVEL_110 110
#define LEVEL_100 100
#define LEVEL_80 80
#define LEVEL_60 60
#define LEVEL_40 40
#define LEVEL_20 20
#define LEVEL_0 0

#define HIGH_LEVEL 1
#define NOT_HIGH_LEVEL 0
#define LOW_LEVEL 1
#define NOT_LOW_LEVEL 0

byte sensorPinArray[MAX_SENSORS] = { SENSOR_1_PIN,
									 SENSOR_2_PIN,
									 SENSOR_3_PIN,
									 SENSOR_4_PIN,
									 SENSOR_5_PIN,
									 SENSOR_6_PIN 
								   };

byte sensorArray[MAX_SENSORS] = { 0,0,0,0,0,0 };
byte binToDecArray[MAX_SENSORS] = { 1,2,4,8,16,32 };
#endif



// Overhead Tank 02
#if defined TANK_02_NODE
#define OVERHEAD_TANK_02_NODE_ID 7
#define SUMP_RELAY_NODE_ID 4
#define SUMP_RELAY_ID 1
#endif

//Underground Tank
#if defined TANK_03_NODE
#define TANK03_NODE_ID 8

#define SENSOR_1 0
#define SENSOR_2 1
#define SENSOR_3 2
#define SENSOR_4 3
#define SENSOR_5 4

#define LOW_LEVEL_SENSOR_ID 1

#endif


#if defined PHASE3_METER
#define PH3_NODE_ID 10
#define PH1_NODE_ID 11


#endif

#if defined PHASE1_METER
#define PH1_NODE_ID 11
#endif

#if defined SOLAR_BATT_VOLTAGE_NODE
#define SOLAR_VOLTAGE_NODE_ID 12
#define BATT_VOLTAGE_NODE_ID 13

#define BATTERY_VOLTAGE_ID 1
#define SOLAR_VOLTAGE_ID 2

#define VOLTAGE_SENSE_PIN A0
#define INPUT_VOLTAGE_PIN A1

#define UP 1

#endif

#if defined REPEATER_NODE
#define BALCONY_REPEATER_NODE_ID 14
#define DB_REPEATER_NODE_ID 15
#endif

#if defined WIFI_NODE
#define BALCONYLIGHT_WITH_PIR_NODE 1
#define STAIRCASE_LIGHT_NODE 2
#define GATELIGHT_WITH_PIR_NODE 3
#define SUMP_RELAY_NODE_ID 4
#define BOREWELL_RELAY_NODE_ID 5
#define OVERHEAD_TANK_01_NODE_ID 6
#define OVERHEAD_TANK_02_NODE_ID 7
#define UNDERGROUND_NODE_ID 8
#define PH3_NODE_ID 10
#define PH1_NODE_ID 11
#define SOLAR_VOLTAGE_NODE_ID 12
#define BATT_VOLTAGE_NODE_ID 13

char ssid[] = "NAGU";          //  your network SSID (name) 
char pass[] = "4ever.Nagu";   // your network password

#endif

void LOG(char *logmessage)
{
#if defined LOG_THIS_NODE_DATA
	Serial.println(logmessage);
#endif
}

#endif

/*
#define WATER_SENSOR_ID 1
#define TRIP_SENSOR_ID 2
#define EMPTY_SENSOR_ID 3
#define FULL_SENSOR_ID 4

#define TRIP_PIN 3

*/