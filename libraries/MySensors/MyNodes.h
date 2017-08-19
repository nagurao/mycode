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
#define QUATER_HOUR 900
#define HALF_HOUR 1800
#define ONE_HOUR 3600
#define REQUEST_INTERVAL 60
#define HEARTBEAT_INTERVAL 600
/*
The following are the Node Ids assigned, populated here as comments for easy reference.
0 - Gateway
1 - Balcony Lights
2 - Staircase Lights
3 - Gate Lights
4 - Tank 01
5 - Tank 02
6 - Tank 03
7 - Borewell Motor
8 - Sump Motor
9 - Tap Motor
10 - Repeater Node 01
11 - LCD
12 - Battery Voltage
13 - Solar Voltage
14 - Controller
15 - Repeater Node 02
16 - Inverter In
17 - Inverter Out
18 - 3Phase Wattmeter
19 - 1Phase Wattmeter
20 - Temperature & Humidity 
250 - Default Firmware Node
254 - Wifi Node
*/

// NodeMCU Wifi Node details
#define THINGSPEAK_NODE_ID 254
#define WIFI_NODEMCU_ID 1

#define BALCONY_REPEATER_NODE_ID 10
#define DB_REPEATER_NODE_ID 15
#define DEFAULT_FIRMWARE_NODE_ID 250

#if defined LIGHT_NODE

#define BALCONYLIGHT_NODE_ID 1
#define STAIRCASE_LIGHT_NODE_ID 2
#define GATELIGHT_NODE_ID 3

#define LIGHT_RELAY_ID 1
#define CURR_MODE_ID 2
#define LIGHT_DURATION_ID 3

#define STANDBY_MODE 0
#define DUSKLIGHT_MODE 1

#define LIGHT_RELAY_PIN 7

#endif

// Overhead Tank 01
#if defined TANK_01_NODE
#define TANK_01_NODE_ID 4
#define DRY_RUN_POLL_DURATION 900
#define BOREWELL_NODE_ID 7
#endif

// Overhead Tank 02
#if defined TANK_02_NODE
#define TANK_02_NODE_ID 5
#define SUMP_MOTOR_NODE_ID 8
#endif

//Underground Tank
#if defined TANK_03_NODE
#define TANK_03_NODE_ID 6

#endif

#if defined BOREWELL_NODE
#define BOREWELL_NODE_ID 7
#define BOREWELL_MOTOR_STATUS_ID 1
#define BORE_ON_RELAY_ID 2
#define BORE_OFF_RELAY_ID 3
#define RELAY_TRIGGER_INTERVAL 3

#define DRY_RUN_POLL_DURATION 900
#define MOTOR_STATUS_PIN 3
#define BORE_ON_RELAY_PIN 7
#define BORE_OFF_RELAY_PIN 8
#endif

#if defined SUMP_RELATED_NODE
#define SUMP_MOTOR_NODE_ID 8
#define TAP_MOTOR_NODE_ID 9

#define CURR_WATER_LEVEL_ID 1
#define RELAY_ID 1
#define RELAY_PIN 7

#define MOTOR_STATUS_PIN 3
#endif

#if defined REPEATER_NODE
#define BALCONY_REPEATER_NODE_ID 10
#define DB_REPEATER_NODE_ID 15
#endif

#if defined STATUS_LEDS
#define MY_DEFAULT_ERR_LED_PIN 4  
#define MY_DEFAULT_RX_LED_PIN  5
#define MY_DEFAULT_TX_LED_PIN  6
#endif

#if defined LCD_NODE

#define LCD_NODE_ID 11
#define BATT_VOLTAGE_NODE_ID 12
#define INV_IN_NODE_ID 16
#define INV_OUT_NODE_ID 17
#define PH3_NODE_ID 18
#define PH1_NODE_ID 19
#define CURR_WATT_ID 1
#define BATTERY_VOLTAGE_ID 4
#define SOLAR_VOLTAGE_ID 5

#define WATER_TANK_NODE_IDS

#define LCD_I2C_ADDR 0x27
#define LCD_ROWS 4
#define LCD_COLUMNS 20
#define LCD_BACKLIGHT_ID 1
#define ROW_1 0
#define ROW_2 1
#define ROW_3 2
#define ROW_4 3

#endif

#if defined SOLAR_BATT_VOLTAGE_NODE
#define BATT_VOLTAGE_NODE_ID 12
#define SOLAR_VOLTAGE_NODE_ID 13

#define R1_VALUE_ID 1
#define R2_VALUE_ID 2
#define SCALE_FACTOR_ID 3
#define BATTERY_VOLTAGE_ID 4
#define SOLAR_VOLTAGE_ID 5
#define RESET_RELAY_ID 6

#define VOLTAGE_SENSE_PIN A0
#define THRESHOLD_VOLTAGE_PIN A1
#define RELAY_PIN 7
#define UP 1

#endif

#if defined REMOTE_CONTROLLER_NODE
#define REMOTE_CONTROLLER_NODE_ID 14
#define KEYPAD_ID 1
#define SCL_PIN 3
#define SDA_PIN 4
#define KEYPAD_SIZE 16
#define KEYPAD_READ_INTERVAL 2

#define RELAY_ID 1
#define CURR_MODE_ID 2
#define BORE_ON_RELAY_ID 2
#define BORE_OFF_RELAY_ID 3

#define BOREWELL_NODE_ID 7
#define SUMP_MOTOR_NODE_ID 8
#define TAP_MOTOR_NODE_ID 9

#endif

#if defined WATT_METER_NODE

#define INV_IN_NODE_ID 16
#define INV_OUT_NODE_ID 17
#define PH3_NODE_ID 18
#define PH1_NODE_ID 19
#define BATT_VOLTAGE_NODE_ID 12

#define ZERO_PULSE 0
#define ACCUMULATION_FREQUENCY_SECS 30
#define MAX_WATT 6000
#define MAX_WATT_INVERTER 2000

#define CURR_WATT_ID 1
#define HOURLY_WATT_CONSUMPTION_ID 2
#define DAILY_WATT_CONSUMPTION_ID 3
#define MONTHLY_WATT_CONSUMPTION_ID 4
#define ACCUMULATED_WATT_CONSUMPTION_ID 5
#define DELTA_WATT_CONSUMPTION_ID 6
#define CURR_PULSE_COUNT_ID 7
#define BLINKS_PER_KWH_ID 8
#define RESET_TYPE_ID 9
#define INCOMING_REQUEST_ID 10

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

#define REQ_CURR_WATT 1
#define REQ_HOURLY_WATT 2
#define REQ_DAILY_WATT 3
#define REQ_MONTHLY_WATT 4
#define REQ_DAILY_DELTA_WATT 5
#define REQ_CURR_DELTA_WATT 6

#endif

#if defined WIFI_NODE
#define BALCONYLIGHT_NODE_ID 1
#define STAIRCASE_LIGHT_NODE_ID 2
#define GATELIGHT_NODE_ID 3
#define TANK_01_NODE_ID 4
#define TANK_02_NODE_ID 5
#define TANK_03_NODE_ID 6
#define BOREWELL_NODE_ID 7
#define SUMP_MOTOR_NODE_ID 8
#define LCD_NODE_ID 11
#define BATT_VOLTAGE_NODE_ID 12
#define SOLAR_VOLTAGE_NODE_ID 13
#define INV_IN_NODE_ID 16
#define INV_OUT_NODE_ID 17
#define PH3_NODE_ID 18
#define PH1_NODE_ID 19

#define STANDBY_MODE 0
#define DUSKLIGHT_MODE 1

#define RELAY_ID 1
#define CURR_MODE_ID 2
#define BORE_ON_RELAY_ID 2
#define BORE_OFF_RELAY_ID 3

char ssid[] = "NAGU";          //  your network SSID (name) 
char pass[] = "4ever.Nagu";   // your network password

#endif

#if defined NODE_INTERACTS_WITH_WIFI_AND_LCD

#define INV_IN_CURR_WATT_ID 1
#define INV_OUT_CURR_WATT_ID 2
#define PH3_CURR_WATT_ID 3
#define PH1_CURR_WATT_ID 4
#define INV_IN_OUT_DELTA_ID 5
#define PH3_PH1_DELTA_ID 6


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

#if defined NODE_WITH_HIGH_LOW_FEATURE
#define HIGH_LEVEL 1
#define NOT_HIGH_LEVEL 0
#define LOW_LEVEL 1
#define NOT_LOW_LEVEL 0
#endif

#if defined NODE_INTERACTS_WITH_LCD
#define LCD_NODE_ID 11
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

#if defined NODE_HAS_TEMP_HUMIDITY
#define TEMP_AND_HUMIDITY_NODE_ID 20
#define HUMIDITY_ID 1
#define TEMPERATURE_ID 2

#define DHT_SENSOR_PIN 2
#define SENSOR_TEMP_OFFSET 0
#endif

# if defined WATER_TANK_NODE_IDS
#define TANK_01_NODE_ID 4
#define TANK_02_NODE_ID 5
#define TANK_03_NODE_ID 6
#endif

#if defined WATER_TANK_NODE
#define PRESSURE_SENSOR_PIN A0
#define REFERENCE_VOLTAGE_PIN A1

#define CURR_WATER_LEVEL_ID 1
#define WATER_LOW_LEVEL_IND_ID 2
#define ANALOG_TANK_EMPTY_ID 3
#define ANALOG_TANK_FULL_ID 4

#define DEFAULT_LOW_LEVEL 40

#define RISING_LEVEL_POLL_DURATION 30
#define DEFAULT_LEVEL_POLL_DURATION 300

#endif

#if defined NODE_HAS_ULTRASONIC_SENSOR
#define ECHO_PIN 5
#define TRIGGER_PIN 6
#define MAX_DISTANCE 300
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