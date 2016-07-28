/***
 *  This file defines the Sensor ids used in MySensors network.
 */
#ifndef MyNodes_h
#define MyNodes_h

#define WAIT_50MS 50
#define ONE_MINUTE 60
#define HEARTBEAT_INTERVAL 600

// Static Repeater Nodes //
#define HALL_REPEATER_NODE 51

#define BALCONYLIGHT_WITH_PIR_NODE 200
#define GATELIGHT_WITH_PIR_NODE 201
#define STAIRCASE_LIGHT_NODE 202

#if defined NODE_HAS_RELAY
#define RELAY_ON 1
#define RELAY_OFF 0
#endif

#if defined NODE_INTERACTS_WITH_RELAY
#define RELAY_ON 1
#define RELAY_OFF 0
#endif

// Sensor Nodes and there child sensors

#if defined TANK01_NODE
#define TANK_01_NODE_ID 101
#endif

#if defined OVERHEAD_TANK_02_NODE
#define OVERHEAD_TANK_02_NODE_ID 102
#define SUMP_RELAY_NODE_ID 110
#define SUMP_RELAY_ID 1
#define LCD_DISPLAY_NODE_ID 150
#define LCD_DISPLAY_ID 1

#define SENSOR_1 0
#define SENSOR_2 1
#define SENSOR_3 2
#define SENSOR_4 3
#define SENSOR_5 4

#define LOW_LEVEL_SENSOR_ID 1

#endif

#if defined TANK03_NODE
#define TANK_03_NODE_ID 103
#endif

#if defined TANK04_NODE
#define TANK_01_NODE_ID 104
#endif


/*
#define WATER_SENSOR_ID 1
#define TRIP_SENSOR_ID 2
#define EMPTY_SENSOR_ID 3
#define FULL_SENSOR_ID 4

#define TRIP_PIN 3
#define ECHO_PIN 5
#define TRIGGER_PIN 6
*/
#if defined SUMP_NODE
#define SUMP_RELAY_NODE_ID 100
#define SUMP_RELAY_SENSOR_ID 1
#endif

#if defined BOREWELL_NODE
#define BOREWELL_RELAY_NODE_ID 106
#define BORE_ON_RELAY_SENSOR_ID 1
#define BORE_OFF_RELAY_SENSOR_ID 2
#endif



// Non-sensor Nodes and there child sensors
#if defined MONITOR_NODE
#define MONITOR_NODE_ID 151
#define MONITOR_NODE_SENSOR_ID 1
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

#if defined STAIRCASE_NODE
#define STAIRCASE_LIGHT_RELAY_ID 1

#endif

void LOG(char *logmessage)
{
#if defined LOG_THIS_NODE_DATA
	Serial.println(logmessage);
#endif
}

#endif

