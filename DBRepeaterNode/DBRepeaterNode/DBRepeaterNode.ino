#include <TimeAlarms.h>
#include <TimeLib.h>
#include <Time.h>
#include <SPI.h>

#define REPEATER_NODE
#define STATUS_LEDS
#define MY_RADIO_NRF24
#define MY_REPEATER_FEATURE
#define MY_NODE_ID DB_REPEATER_NODE_ID
#define MY_DEBUG

#define MY_LEDS_BLINKING_FEATURE
#define MY_DEFAULT_LED_BLINK_PERIOD 300
#define MY_WITH_LEDS_BLINKING_INVERSE

#include <MyNodes.h>
#include <MySensors.h>
#include <MyConfig.h>

#define APPLICATION_NAME "Repeater Node 02"

AlarmId heartbeatTimer;

void before()
{

}

void setup()
{
	heartbeatTimer = Alarm.timerRepeat(HEARTBEAT_INTERVAL, sendHeartbeat);
}

void presentation()
{
	sendSketchInfo(APPLICATION_NAME, __DATE__);
}

void loop()
{
	Alarm.delay(1);
}
void receive(const MyMessage &message)
{

}