/***
 *  This file defines the Sensor ids used in MySensors network.
 */
#ifndef MyNodes_h
#define MyNodes_h

#define WAIT_10MS 10
#define WAIT_50MS 50
#define ONE_MINUTE 60
#define ONE_HOUR 3600
#define HEARTBEAT_INTERVAL 600
/*
The following are the Nod Ids assigned, populated here as comments for easy reference.
0 - Gateway
1 - Balcony Lights
2 - Staircase Lights
3 - Gate Lights
4 - Sump Motor
5 - Borewell Motor
6 - Overhead Tank 01
7 - Overhead Tank 02
8 - Underground Tank
9 - 3-Phase Meter
10 - 1-Phase Meter
11 - Solar Voltage
12 - Battery Voltage
254 - Wifi Node
*/

#if defined NODE_HAS_RELAY
#define RELAY_ON 1
#define RELAY_OFF 0
#endif

#if defined NODE_INTERACTS_WITH_RELAY
#define RELAY_ON 1
#define RELAY_OFF 0
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

#endif

#if defined NODE_INTERACTS_WITH_LCD
#define LCD_DISPLAY_NODE_ID 9
#define LCD_DISPLAY_ID 1
#endif

#if defined STAIRCASE_NODE

#define STAIRCASE_LIGHT_NODE_ID 2
#define STAIRCASE_LIGHT_RELAY_ID 1

#define BALCONYLIGHT_WITH_PIR_NODE_ID 1
#define GATELIGHT_WITH_PIR_NODE_ID 3
#endif

#if defined SUMP_MOTOR_NODE
#define SUMP_RELAY_NODE_ID 4
#define SUMP_RELAY_ID 1
#define SUMP_RELAY_PIN 7
#endif

#if defined BOREWELL_NODE
#define BOREWELL_RELAY_NODE_ID 5
#define BOREWELL_MOTOR_ID 1
#define BORE_ON_RELAY_ID 2
#define BORE_OFF_RELAY_ID 3
#define BORE_ON_RELAY_PIN 7
#define BORE_OFF_RELAY_PIN 8
#define RELAY_TRIGGER_INTERVAL 3

#endif

// Overhead Tank 01
#if defined OVERHEAD_TANK_01_NODE
#define OVERHEAD_TANK_01_NODE_ID 6
#define SUMP_RELAY_NODE_ID 5
#define SUMP_RELAY_ID 1


#define SENSOR_1 0
#define SENSOR_2 1
#define SENSOR_3 2
#define SENSOR_4 3
#define SENSOR_5 4
#define LOW_LEVEL_SENSOR_ID 1

#endif

// Overhead Tank 02
#if defined OVERHEAD_TANK_02_NODE
#define OVERHEAD_TANK_02_NODE_ID 7
#define SUMP_RELAY_NODE_ID 4
#define SUMP_RELAY_ID 1

#define MAX_DISTANCE 250
#define MIN_DISTANCE 25

#define SENSOR_1 0
#define SENSOR_2 1
#define SENSOR_3 2
#define SENSOR_4 3
#define SENSOR_5 4

#define LOW_LEVEL_SENSOR_ID 1

#endif

//Underground Tank
#if defined UNDERGROUND_TANK_NODE
#define UNDERGROUND_NODE_ID 8
#define SUMP_RELAY_NODE_ID 5
#define SUMP_RELAY_ID 1

#define SENSOR_1 0
#define SENSOR_2 1
#define SENSOR_3 2
#define SENSOR_4 3
#define SENSOR_5 4

#define LOW_LEVEL_SENSOR_ID 1

#endif


#if defined PHASE3_METER
#define PH3_NODE_ID 10
#define CURR_WATT_ID 1
#define HOURLY_WATT_CONSUMPTION_ID 2
#define DAILY_WATT_CONSUMPTION_ID 3
#define ACCUMULATED_WATT_CONSUMPTION_ID 4
#define CURR_PULSE_COUNT_ID 5
#endif

#if defined PHASE3_METER
#define PH1_NODE_ID 11
#endif

#if defined SOLAR_VOLTAGE_NODE
#define SOLAR_VOLTAGE_NODE_ID 12
#endif

#if defined BATT_VOLTAGE_NODE
#define BATT_VOLTAGE_NODE_ID 13
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

// NodeMCU Wifi Node details
#define THINGSPEAK_NODE_ID 254
#define WIFI_NODEMCU_ID 1

/*
#define WATER_SENSOR_ID 1
#define TRIP_SENSOR_ID 2
#define EMPTY_SENSOR_ID 3
#define FULL_SENSOR_ID 4

#define TRIP_PIN 3

*/