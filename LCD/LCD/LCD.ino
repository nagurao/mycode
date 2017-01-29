#include <LCD_I2C.h>
#include <TimeAlarms.h>
#include <TimeLib.h>
#include <Time.h>
#include <SPI.h>

#define LCD_NODE
#define NODE_INTERACTS_WITH_WIFI_AND_LCD
#define NODE_WITH_ON_OFF_FEATURE

#define MY_RADIO_NRF24
#define MY_REPEATER_FEATURE
#define MY_NODE_ID LCD_NODE_ID
//#define MY_DEBUG

#include <MyNodes.h>
#include <MySensors.h>
#include <MyConfig.h>

#define APPLICATION_NAME "LCD Node"

AlarmId heartbeatTimer;
AlarmId backlightTimer;

LCD_I2C lcd(LCD_I2C_ADDR, LCD_COLUMNS, LCD_ROWS);

uint8_t phi[8] = {0x4,0x4,0xE,0x15,0x15,0xE,0x4,0x4};
uint8_t delta[8] = {0x0,0x4,0xE,0x1B,0x11,0x11,0x1F,0x0};
uint8_t inverterIn[8] = { 0x04,0x04,0x04,0x04,0x04,0x15,0x0E,0x04 };
uint8_t inverterOut[8] = { 0x04,0x0E,0x15,0x04,0x04,0x04,0x04,0x04 };
uint8_t inverterDelta[8] = { 0x04,0x0E,0x15,0x04,0x04,0x15,0x0E,0x04 };
boolean lcdBackLightFlag;
byte lcdBackLightFlagRequestCount;
boolean sendBackLightFlagRequest;
boolean lcdBackLightFlagReceived;

MyMessage lcdBackLightFlagMessage(LCD_BACKLIGHT_ID, V_STATUS);

void before()
{
	lcd.begin();
	lcd.home();
    lcd.createChar(1, inverterIn);
	lcd.createChar(2, inverterOut);
	lcd.createChar(3, inverterDelta);
	lcd.createChar(4, delta);
	lcd.createChar(5, phi);
	/*printLCDVal(0, ROW_1, "TANK1:",true);
	printLCDVal(0, ROW_2, "TANK2:",true);
	printLCDVal(0, ROW_3, "TANK3:", true);*/
	lcd.setCursor(0, ROW_1);
	lcd.write(1);
	printLCDVal(1, ROW_1, ":", true);
	lcd.setCursor(0, ROW_2);
	lcd.write(2);
	printLCDVal(1, ROW_2, ":", true);
	lcd.setCursor(0, ROW_3);
	lcd.write(3);
	printLCDVal(1, ROW_3, ":", true);
	lcd.setCursor(0, ROW_4);
	lcd.write(4);
	printLCDVal(1, ROW_4, ":", true);

	printLCDVal(9, ROW_1, "|SOL:",true);
	printLCDVal(19, ROW_1, "V", true);
	printLCDVal(9, ROW_2, "|BAT:", true);
	printLCDVal(19, ROW_2, "V", true);
	printLCDVal(9, ROW_3, "|3",true);
	lcd.setCursor(11, ROW_3);
	lcd.write(5);
	printLCDVal(12, ROW_3, ":", true);
	printLCDVal(9, ROW_4, "|1", true);
	lcd.setCursor(11, ROW_4);
	lcd.write(5);
	printLCDVal(12, ROW_4, ":", true);
}

void setup()
{
	lcdBackLightFlag = true;
	lcdBackLightFlagRequestCount = 0;
	lcdBackLightFlagReceived = false;
	sendBackLightFlagRequest = true;
	heartbeatTimer = Alarm.timerRepeat(HEARTBEAT_INTERVAL, sendHeartbeat);
}

void presentation()
{
	sendSketchInfo(APPLICATION_NAME, __DATE__);
	present(LCD_BACKLIGHT_ID, S_BINARY, "LCD Backlit Light");
}

void loop()
{
	if (sendBackLightFlagRequest)
	{
		sendBackLightFlagRequest = false;
		request(LCD_BACKLIGHT_ID, V_STATUS);
		backlightTimer = Alarm.timerOnce(REQUEST_INTERVAL, checkBackLightFlagRequestStatus);
		lcdBackLightFlagRequestCount++;
		if (lcdBackLightFlagRequestCount == 10)
			send(lcdBackLightFlagMessage.set(TURN_ON));
	}
	Alarm.delay(1);
}
void receive(const MyMessage &message)
{
	float currWatt;
	char dispValue[7];
	float currVoltage;
	char dispVoltValue[5];
	float currWaterLevel;
	char dispWaterLevel[3];
	byte column;
	byte row;
	switch (message.type)
	{
	case V_WATT:
		currWatt = message.getFloat();
		ftoa(currWatt, dispValue, 4, 2);
		switch (message.sensor)
		{
		case PH3_CURR_WATT_ID:
			column = 13;
			row = ROW_3;
			break;
		case PH1_CURR_WATT_ID:
			column = 13;
			row = ROW_4;
			break;
		}
		lcd.backlight();
		for (byte index = 0; index < 7; index++, column++)
			printLCDVal(column, row, dispValue[index], true);
		Alarm.timerOnce(ONE_MINUTE, turnOffLCDLight);
		break;
	case V_KWH:
		currWatt = message.getFloat();
		ftoa(currWatt, dispValue, 4, 2);
		switch (message.sensor)
		{
		case PH3_PH1_DELTA_ID:
			column = 2;
			row = ROW_4;
			break;
		}
		
		lcd.backlight();
		for (byte index = 0; index < 7; index++, column++)
			printLCDVal(column, row, dispValue[index], true);
		Alarm.timerOnce(ONE_MINUTE, turnOffLCDLight);
		break;
	case V_VOLTAGE:
		currVoltage = message.getFloat();
		ftoa(currVoltage, dispVoltValue, 2, 2);
		switch (message.sensor)
		{
		case BATTERY_VOLTAGE_ID:
			column = 14;
			row = ROW_2;
			break;
		case SOLAR_VOLTAGE_ID:
			column = 14;
			row = ROW_1;
			break;
		}
		lcd.backlight();
		for (byte index = 0; index < 5; index++, column++)
			printLCDVal(column, row, dispVoltValue[index], true);
		Alarm.timerOnce(ONE_MINUTE, turnOffLCDLight);
		break;
	case V_STATUS:
		lcdBackLightFlag = message.getInt();
		if (!lcdBackLightFlagReceived)
		{
			lcdBackLightFlagReceived = true;
			Alarm.free(backlightTimer);
			sendBackLightFlagRequest = false;
		}
		if (lcdBackLightFlag)
		{
			lcd.backlight();
		}
		else
		{
			lcd.noBacklight();
		}
		break;
	case V_VOLUME:
		currWaterLevel = (float)message.getInt();
		ftoa(currWaterLevel, dispWaterLevel, 3, 0);
		switch(message.sender)
		{
			case TANK_01_NODE_ID:
				column = 6;
				row = ROW_1;
				break;
			case TANK_02_NODE_ID:
				column = 6;
				row = ROW_2;
				break;
			case TANK_03_NODE_ID:
				column = 6;
				row = ROW_3;
				break;
		}
		lcd.backlight();
		for (byte index = 0; index < 3; index++, column++)
			printLCDVal(column, row, dispWaterLevel[index], true);
		Alarm.timerOnce(ONE_MINUTE, turnOffLCDLight);
		break;
	}
}

void checkBackLightFlagRequestStatus()
{
	if (!lcdBackLightFlagReceived)
		sendBackLightFlagRequest = true;
}

void printLCDVal(byte column, byte row, char* text, boolean clearFlag)
{
	byte stringLength = strlen(text);
	if (clearFlag)
	{
		lcd.setCursor(column, row);
		for (byte i = 1; i <= stringLength; i++)
			lcd.print(" ");
	}
	lcd.setCursor(column, row);
	lcd.print(text);
}

void printLCDVal(byte column, byte row, char text, boolean clearFlag)
{
	lcd.setCursor(column, row);
	if (clearFlag)
		lcd.print(" ");
	lcd.setCursor(column, row);
	lcd.print(text);
}

/*void printLCDVal(byte column, byte row, byte num)
{
	lcd.setCursor(column, row);
	if (num < 10)
		lcd.print("0");
	lcd.print(num);
	lcd.backlight();
	Alarm.timerOnce(ONE_MINUTE, turnOffLCDLight);
}*/

void turnOffLCDLight()
{
	if(!lcdBackLightFlag)
		lcd.noBacklight();
}

void ftoa(float floatNum, char *resultString, byte digitsInIntegerPart, byte resolution)
{
	boolean isNegVal = false;
	if (floatNum < 0)
	{
		isNegVal = true;
		floatNum = floatNum * -1;
		digitsInIntegerPart = digitsInIntegerPart - 1;
	}

	int intergerPart = (int)floatNum;
	float fractionPart = floatNum - (float)intergerPart;
	int i = intToString(intergerPart, resultString, digitsInIntegerPart,isNegVal);
	if (resolution != 0)
	{
		resultString[i] = '.';
		fractionPart = fractionPart * pow(10, resolution);
		intToString((int)fractionPart, resultString + i + 1, resolution);
	}
}

int intToString(int intValue, char str[], byte digitsInIntegerPart,boolean isNegVal)
{
	byte i = 0;
	while (intValue)
	{
		str[i++] = (intValue % 10) + '0';
		intValue = intValue / 10;
	}
	while (i < digitsInIntegerPart)
		str[i++] = '0';
	if (isNegVal)
		str[i++] = '-';
	reverse(str, i);
	//str[i] = '\0';
	return i;
}

int intToString(int intValue, char str[], byte resolution)
{
	byte i = 0;
	while (intValue)
	{
		str[i++] = (intValue % 10) + '0';
		intValue = intValue / 10;
	}
	while (i < resolution)
		str[i++] = '0';
	reverse(str, i);
	return i;
}

void reverse(char *str, byte len)
{
	byte i = 0, j = len - 1, temp;
	while (i < j)
	{
		temp = str[i];
		str[i] = str[j];
		str[j] = temp;
		i++;
		j--;
	}
}