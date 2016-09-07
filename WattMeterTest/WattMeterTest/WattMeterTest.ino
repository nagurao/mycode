#include <TimeAlarms.h>
#include <TimeLib.h>
#include <Time.h>
#include <SPI.h>

#define MY_RADIO_NRF24
#define MY_REPEATER_FEATURE
#define MY_NODE_ID 253
#define MY_DEBUG 

#include <MyNodes.h>
#include <MySensors.h>
#include <MyConfig.h>

#define APPLICATION_NAME "WattMeter Test"
#define APPLICATION_VERSION "28Aug2016"

AlarmId heartbeatTimer;

#define DIGITAL_INPUT_SENSOR 3  // The digital input you attached your light sensor.  (Only 2 and 3 generates interrupt!)
#define PULSE_FACTOR 6400       // Nummber of blinks per KWH of your meeter
#define SLEEP_MODE false        // Watt-value can only be reported when sleep mode is false.
#define MAX_WATT 10000          // Max watt value to report. This filetrs outliers.
#define INTERRUPT DIGITAL_INPUT_SENSOR-2 // Usually the interrupt = pin -2 (on uno/nano anyway)
#define CHILD_ID 1              // Id of the sensor child
unsigned long SEND_FREQUENCY = 20000; // Minimum time between send (in milliseconds). We don't wnat to spam the gateway.
double ppwh = ((double)PULSE_FACTOR) / 1000; // Pulses per watt hour
boolean pcReceived = false;
volatile unsigned long pulseCount = 0;
volatile unsigned long lastBlink = 0;
volatile unsigned long watt = 0;
unsigned long oldPulseCount = 0;
unsigned long oldWatt = 0;
double oldKwh;
unsigned long lastSend;
MyMessage wattMsg(CHILD_ID, V_WATT);
MyMessage kwhMsg(CHILD_ID, V_KWH);
MyMessage pcMsg(CHILD_ID, V_VAR1);

byte requestCount = 0;
void before()
{
	
}

void setup()
{
	heartbeatTimer = Alarm.timerRepeat(HEARTBEAT_INTERVAL, sendHeartbeat);
	attachInterrupt(INTERRUPT, onPulse, RISING);
	lastSend = millis();
}

void presentation()
{
	sendSketchInfo(APPLICATION_NAME, APPLICATION_VERSION);
	present(CHILD_ID, S_POWER);
	request(CHILD_ID, V_VAR1);
	
}

void loop()
{
	unsigned long now = millis();
	bool sendTime = now - lastSend > SEND_FREQUENCY;
	if (pcReceived && (SLEEP_MODE || sendTime)) {
		if (!SLEEP_MODE && watt != oldWatt) {
			if (watt < ((unsigned long)MAX_WATT)) {
				send(wattMsg.set(watt));
			}
			oldWatt = watt;
		}
		if (pulseCount != oldPulseCount) {
			send(pcMsg.set(pulseCount));
			double kwh = ((double)pulseCount / ((double)PULSE_FACTOR));
			oldPulseCount = pulseCount;
			if (kwh != oldKwh) {
				send(kwhMsg.set(kwh, 4));
				oldKwh = kwh;
			}
		}
		lastSend = now;
	}
	else if (sendTime && !pcReceived) {
		request(CHILD_ID, V_VAR1);
		requestCount++;
		if(requestCount == 10)
			send(pcMsg.set(0));
		lastSend = now;
	}
	if (SLEEP_MODE) {
		sleep(SEND_FREQUENCY);
	}
	Alarm.delay(1);
}

void receive(const MyMessage &message)
{
	if (message.type == V_VAR1)
	{
		pulseCount = oldPulseCount = message.getLong();
		pcReceived = true;
	}
}

/*
void receiveTime(unsigned long controllerTime)
{

	/*send(statMessage.set("recTime"));
	hr = hour(controllerTime);
	min = minute(controllerTime);
	sec = second(controllerTime);
	send(hrMessage.set(hr));
	Alarm.free(hourlyTimer);
	Alarm.free(midnightTimer);
	Alarm.free(refreshTimeTimer);
	unsigned int secondsForNextHour = (60 - sec) + ((59 - min) * 60);
	Alarm.timerOnce(secondsForNextHour, createHourlyTimer);
	send(statMessage.set(secondsForNextHour));
	unsigned int secondsForMidnight = (60 - sec) + ((23 - hr) * 60 + (59 - min)) * 60;
	midnightTimer = Alarm.timerOnce(secondsForMidnight, resetWattDay);
	refreshTimeTimer = Alarm.timerOnce(secondsForMidnight + ONE_HOUR, refreshNodeTime);
	send(statMessage.set(secondsForMidnight));
	
	setTime(controllerTime);
}
*/

/*void createHourlyTimer()
{
	Serial.println("CRTHRLYTMR");
	send(statMessage.set("CRTHRLYTMR"));
	hourlyTimer = Alarm.timerRepeat(ONE_HOUR, resetWattHour);
	hr = (hr + 1) % 24;
} */

/*void refreshNodeTime()
{
	send(statMessage.set("REFRTIME"));
	Serial.println("REFRTIME");
	requestTime();
}*/

void onPulse()
{
	if (!SLEEP_MODE) {
		unsigned long newBlink = micros();
		unsigned long interval = newBlink - lastBlink;
		if (interval<10000L) { // Sometimes we get interrupt on RISING
			return;
		}
		watt = (3600000000.0 / interval) / ppwh;
		lastBlink = newBlink;
	}
	pulseCount++;
}
