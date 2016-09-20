#include <LCD_I2C.h>
#include <TimeAlarms.h>
#include <TimeLib.h>
#include <Time.h>
#include <SPI.h>
#define LCD_NODE

#define MY_RADIO_NRF24
#define MY_REPEATER_FEATURE
#define MY_NODE_ID LCD_DISPLAY_NODE_ID
#define MY_DEBUG

#include <MyNodes.h>
#include <MySensors.h>
#include <MyConfig.h>

#define APPLICATION_NAME "LCD Node"
#define APPLICATION_VERSION "20Sep2016"

#define LCD_I2C_ADDR 0x27
#define LCD_ROWS 4
#define LCD_COLUMNS 20
#define LCD_BACKLIGHT_TIME 10
#define ROW_1 0
#define ROW_2 1
#define ROW_3 2
#define ROW_4 3

AlarmId heartbeatTimer;

LCD_I2C lcd(LCD_I2C_ADDR, LCD_COLUMNS, LCD_ROWS);

uint8_t phi[8] = {0x4,0x4,0xe,0x15,0x15,0xe,0x4,0x4};
uint8_t delta[8] = {0x0,0x4,0xe,0x1b,0x11,0x11,0x1f,0x0};

void before()
{
	lcd.begin();
	lcd.home();
	lcd.createChar(0, phi);
	lcd.createChar(1, delta);
	printLCDVal(0, ROW_1, "TANK1:",true);
	printLCDVal(0, ROW_2, "TANK2:",true);
	printLCDVal(0, ROW_3, "TANK3:", true);
	lcd.setCursor(0, ROW_4);
	lcd.write(1);
	printLCDVal(1, ROW_4, ":", true);
	printLCDVal(9, ROW_1, "|SOL:",true);
	printLCDVal(9, ROW_2, "|BAT:", true);
	printLCDVal(9, ROW_3, "|3",true);
	lcd.setCursor(11, ROW_3);
	lcd.write(0);
	printLCDVal(12, ROW_3, ":", true);
	printLCDVal(9, ROW_4, "|1", true);
	lcd.setCursor(11, ROW_4);
	lcd.write(0);
	printLCDVal(12, ROW_4, ":", true);
}

void setup()
{
	heartbeatTimer = Alarm.timerRepeat(HEARTBEAT_INTERVAL, sendHeartbeat);
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
	case V_WATT:
		switch (message.sender)
		{
		case PH3_NODE_ID:
			float currWatt;
			currWatt = message.getFloat();
			char dispValue[8];
			break;
		case PH1_NODE_ID:
			break;
		}
		break;
	case V_KWH:
		break;
	}
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
	lcd.backlight();
}

void printLCDVal(byte column, byte row, byte num)
{
	lcd.setCursor(column, row);
	if (num < 10)
		lcd.print("0");
	lcd.print(num);
	lcd.backlight();
}

void ftoa(float floatNum, char *resultString, byte resolution)
{
	int intergerPart = (int)floatNum;
	float fractionPart = floatNum - (float)intergerPart;
	int i = intToString(intergerPart, resultString, 0);
	if (resolution != 0)
	{
		resultString[i] = '.';
		fractionPart = fractionPart * pow(10, resolution);
		intToString((int)fractionPart, resultString + i + 1, resolution);
	}
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
	str[i] = '\0';
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