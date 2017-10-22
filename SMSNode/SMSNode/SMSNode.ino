#include <SoftwareSerial.h>
#include <Adafruit_FONA.h>
#include <TimeAlarms.h>
#include <TimeLib.h>
#include <Time.h>
#include <SPI.h>

#define SMS_NODE

#define MY_RADIO_NRF24

#define MY_NODE_ID SMS_NODE_ID
//#define MY_PARENT_NODE_ID BALCONY_REPEATER_NODE_ID
//#define MY_PARENT_NODE_IS_STATIC
#define MY_DEBUG

#include <MyNodes.h>
#include <MySensors.h>
#include <MyConfig.h>

#define APPLICATION_NAME "SMS Node"

boolean adminConfirmed;
boolean smsMessageReceived;
uint16_t smsLength;
int8_t smsNumber;

char smsBuffer[MAXSMSLENGTH + 1];
char tempBuffer[MAXSMSLENGTH + 1];
char adminRegisterBuffer[MAXSMSLENGTH + 1];
char adminDeregisterBuffer[MAXSMSLENGTH + 1];
char adminNumber[MAXNUMBERLENGTH + 1];
char smsFromNumber[MAXNUMBERLENGTH + 1];
char validNumbers[11];

AlarmId requestTimer;
AlarmId heartbeatTimer;

byte adminNumberRequestCount;
boolean sendAdminNumberRequest;
boolean adminNumberReceived;

boolean sim800LStatus;
boolean sim800LStatusSent;

Adafruit_FONA fona = Adafruit_FONA(FONA_RST);
SoftwareSerial fonaSS = SoftwareSerial(FONA_TX, FONA_RX);
SoftwareSerial *fonaSerial = &fonaSS;

MyMessage smsMessageToGateway(SMS_TEXT_ID, V_TEXT);
MyMessage smsMessageToNode(SMS_TEXT_ID, V_TEXT);
MyMessage adminNumberMessage(USER_ADMIN_ID, V_VAR1);
MyMessage smsDataToNodeMessage;

void before()
{
	adminConfirmed = false;
	smsMessageReceived = false;
	smsLength = 0;
	strcpy(smsBuffer,"");
	strcpy(validNumbers, "0123456789");
	strcpy(adminNumber, "");
	strcpy(smsFromNumber, "");
	adminNumberRequestCount = 0;
	sendAdminNumberRequest = true;
	adminNumberReceived = false;
	strcpy(adminRegisterBuffer, "");
	strcpy(adminDeregisterBuffer, "");
	strcat(adminRegisterBuffer, "ADMIN:");
	strcat(adminRegisterBuffer, __DATE__);
	strcat(adminDeregisterBuffer, "RELEASE:");
	strcat(adminDeregisterBuffer, __DATE__);
	sim800LStatus = false;
	sim800LStatusSent = false;
}

void setup()
{
	heartbeatTimer = Alarm.timerRepeat(HEARTBEAT_INTERVAL, sendHeartbeat);
	fonaSerial->begin(4800);
	Alarm.delay(WAIT_1SEC);
	if (fona.begin(*fonaSerial))
	{
		sim800LStatus = true;
		clearSMS();
	}
	else
		sim800LStatus = false;
}

void presentation()
{
	sendSketchInfo(APPLICATION_NAME, __DATE__);
	Alarm.delay(WAIT_AFTER_SEND_MESSAGE);
	present(USER_ADMIN_ID, S_CUSTOM, "Admin Number");
	Alarm.delay(WAIT_AFTER_SEND_MESSAGE);
	present(SMS_TEXT_ID, S_INFO, "SMS Message");
	Alarm.delay(WAIT_AFTER_SEND_MESSAGE);
}

void loop()
{
	if (!sim800LStatusSent)
	{
		strcpy(tempBuffer, "");
		if(sim800LStatus)
			strcat(tempBuffer, SIM800L_OK);
		else
			strcat(tempBuffer, SIM800L_ERROR);
		smsMessageToGateway.set(tempBuffer);
		send(smsMessageToGateway);
		wait(WAIT_AFTER_SEND_MESSAGE);
		sim800LStatusSent = true;
	}
	if (sendAdminNumberRequest)
	{
		sendAdminNumberRequest = false;
		request(USER_ADMIN_ID, V_VAR1);
		requestTimer = Alarm.timerOnce(REQUEST_INTERVAL, checkAdminNumberRequestStatus);
		adminNumberRequestCount++;
		if (adminNumberRequestCount == 10)
		{
			adminConfirmed = false;
			strcpy(tempBuffer, "");
			strcpy(adminNumber, "");
			adminNumberMessage.set(adminNumber);
			send(adminNumberMessage);
			wait(WAIT_AFTER_SEND_MESSAGE);
		}
	}

	if (smsMessageReceived)
	{
		smsMessageReceived = false;
		smsMessageToGateway.set(smsBuffer);
		send(smsMessageToGateway);
		wait(WAIT_AFTER_SEND_MESSAGE);
		if (adminConfirmed)
		{
			if (fona.sendSMS(adminNumber, smsBuffer))
				smsMessageToGateway.set("SMS Status : OK");
			else
				smsMessageToGateway.set("SMS Status : Failed");
			send(smsMessageToGateway);
			wait(WAIT_AFTER_SEND_MESSAGE);
		}
		strcpy(smsBuffer, "");
	}

	smsNumber = fona.getNumSMS();
	if (smsNumber > 0)
	{
		if (fona.readSMS(smsNumber, smsBuffer, MAXSMSLENGTH, &smsLength))
		{
			fona.getSMSSender(smsNumber, smsFromNumber, MAXNUMBERLENGTH);
			if (!adminConfirmed)
			{
				if (strcmp(smsBuffer, adminRegisterBuffer) == 0)
				{
					adminConfirmed = true;
					strcpy(adminNumber, smsFromNumber);
					adminNumberMessage.set(smsFromNumber);
					send(adminNumberMessage);
					wait(WAIT_AFTER_SEND_MESSAGE);
					strcpy(tempBuffer, "");
					strcat(tempBuffer, "New Admin:");
					strcat(tempBuffer, smsFromNumber);
					smsMessageToGateway.set(tempBuffer);
					send(smsMessageToGateway);
					wait(WAIT_AFTER_SEND_MESSAGE);
					strcpy(smsBuffer, CONFIRM_MESSAGE);
					if (fona.sendSMS(adminNumber, smsBuffer))
						smsMessageToGateway.set("Admin Informed");
					else
						smsMessageToGateway.set("Admin Not Informed");
					send(smsMessageToGateway);
					wait(WAIT_AFTER_SEND_MESSAGE);
				}
				else
				{
					strcat(tempBuffer, "Ignored SMS :");
					strcat(tempBuffer, smsFromNumber);
					smsMessageToGateway.set(tempBuffer);
					send(smsMessageToGateway);
					wait(WAIT_AFTER_SEND_MESSAGE);
					smsMessageToGateway.set(smsBuffer);
					send(smsMessageToGateway);
					wait(WAIT_AFTER_SEND_MESSAGE);
				}
				strcpy(smsBuffer, "");
			}

			if (adminConfirmed)
			{
				if (strcmp(adminNumber, smsFromNumber) == 0)
				{

					if (strcmp(smsBuffer, adminDeregisterBuffer) == 0)
					{
						adminConfirmed = false;
						strcpy(tempBuffer, "");
						strcat(tempBuffer, "Deregistered Admin");
						smsMessageToGateway.set(tempBuffer);
						send(smsMessageToGateway);
						wait(WAIT_AFTER_SEND_MESSAGE);
						strcpy(smsBuffer, CONFIRM_MESSAGE);
						if (fona.sendSMS(adminNumber, smsBuffer))
							smsMessageToGateway.set("Admin Informed");
						else
							smsMessageToGateway.set("Admin Not Informed");
						send(smsMessageToGateway);
						wait(WAIT_AFTER_SEND_MESSAGE);
						strcpy(adminNumber, "");
						adminNumberMessage.set(adminNumber);
						send(adminNumberMessage);
						wait(WAIT_AFTER_SEND_MESSAGE);
						strcpy(smsBuffer, "");
					}

					if (nodeAddressedMessage())
					{
						switch (nodeId())
						{
						case BALCONYLIGHT_NODE_ID:
							smsDataToNodeMessage.setDestination(BALCONYLIGHT_NODE_ID);
							smsDataToNodeMessage.setType(V_VAR1);
							smsDataToNodeMessage.setSensor(CURR_MODE_ID);
							smsDataToNodeMessage.set(getSMSMessageContentNumeric(smsBuffer));
							send(smsDataToNodeMessage);
							break;
						case GATELIGHT_NODE_ID:
							smsDataToNodeMessage.setDestination(GATELIGHT_NODE_ID);
							smsDataToNodeMessage.setType(V_VAR1);
							smsDataToNodeMessage.setSensor(CURR_MODE_ID);
							smsDataToNodeMessage.set(getSMSMessageContentNumeric(smsBuffer));
							send(smsDataToNodeMessage);
							break;
						case BOREWELL_NODE_ID:
							smsDataToNodeMessage.setDestination(BOREWELL_NODE_ID);
							smsDataToNodeMessage.setType(V_STATUS);
							if (sensorId() == BORE_ON_RELAY_ID)
								smsDataToNodeMessage.setSensor(BORE_ON_RELAY_ID);
							if (sensorId() == BORE_OFF_RELAY_ID)
								smsDataToNodeMessage.setSensor(BORE_OFF_RELAY_ID);
							smsDataToNodeMessage.set(getSMSMessageContentNumeric(smsBuffer));
							send(smsDataToNodeMessage);
							break;
						case SUMP_MOTOR_NODE_ID:
							smsDataToNodeMessage.setDestination(SUMP_MOTOR_NODE_ID);
							smsDataToNodeMessage.setType(V_STATUS);
							smsDataToNodeMessage.setSensor(RELAY_ID);
							smsDataToNodeMessage.set(getSMSMessageContentNumeric(smsBuffer));
							send(smsDataToNodeMessage);
							break;
						case TAP_MOTOR_NODE_ID:
							break;
						case BATT_VOLTAGE_NODE_ID:
							break;
						}
						wait(WAIT_AFTER_SEND_MESSAGE);
						smsMessageToGateway.set(smsBuffer);
						send(smsMessageToGateway);
						wait(WAIT_AFTER_SEND_MESSAGE);
						strcpy(smsBuffer, "");
					}

					if (strcmp(smsBuffer, "") != 0)
					{
						strcpy(tempBuffer, "");
						strcat(tempBuffer, INVALID_MESSAGE);
						smsMessageToGateway.set(tempBuffer);
						send(smsMessageToGateway);
						wait(WAIT_AFTER_SEND_MESSAGE);
						smsMessageToGateway.set(smsBuffer);
						send(smsMessageToGateway);
						wait(WAIT_AFTER_SEND_MESSAGE);
						strcpy(smsBuffer, "");
					}
				}
				else
				{
					strcpy(tempBuffer, "");
					strcat(tempBuffer, UNKNOWN_MESSAGE);
					strcat(tempBuffer, smsFromNumber);
					smsMessageToGateway.set(tempBuffer);
					send(smsMessageToGateway);
					wait(WAIT_AFTER_SEND_MESSAGE);
					smsMessageToGateway.set(smsBuffer);
					send(smsMessageToGateway);
					wait(WAIT_AFTER_SEND_MESSAGE);
					strcpy(smsBuffer, "");
				}
			}
		}
		clearSMS();
	}

	wait(WAIT_1SEC);
	Alarm.delay(1);
}

void receive(const MyMessage &message)
{	
	switch (message.type)
	{
	case V_VAR1:
		if (strlen(message.getString()) == 12)
		{
			strcpy(adminNumber, message.getString());
			adminConfirmed = true;
			if (!adminNumberReceived)
			{
				adminNumberReceived = true;
				Alarm.free(requestTimer);
				request(USER_ADMIN_ID, V_VAR1);
				wait(WAIT_AFTER_SEND_MESSAGE);
			}
		}
		else
		{
			adminConfirmed = false;
			strcpy(adminNumber, "");
		}
		break;
	case V_TEXT:
		if (!smsMessageReceived)
		{
			strcpy(smsBuffer, "");
			sprintf(smsBuffer, "N03%dS02%d:", message.sender, message.sensor);
			strcat(smsBuffer, message.getString());
			//strcpy(messageBuffer, message.getString());
			smsMessageReceived = true;
		}
		break;
	default:
		break;
	}
}

int nodeId()
{
	int nodeId = -1;
	nodeId = ((smsBuffer[1] - 48) * 100) + ((smsBuffer[2] - 48) * 10) + ((smsBuffer[3] - 48) * 1);
	return nodeId;
}

byte sensorId()
{
	byte sensorId = -1;
	sensorId = ((smsBuffer[5] - 48) * 10) + ((smsBuffer[6] - 48) * 1);
	return sensorId;
}

boolean nodeAddressedMessage()
{
	if (strlen(smsBuffer) > 8)
	{
		if ((smsBuffer[0] == 'N') && (strchr(validNumbers, smsBuffer[1]) != NULL) && (strchr(validNumbers, smsBuffer[2]) != NULL) && (strchr(validNumbers, smsBuffer[3]) != NULL))
		{
			if ((smsBuffer[4] == 'S') && (strchr(validNumbers, smsBuffer[5]) != NULL) && (strchr(validNumbers, smsBuffer[6]) != NULL) && (smsBuffer[7] == ':'))
			{
				if ((nodeId() <= MAXNODEID) && (sensorId() <= MAXSENSORID))
					return true;
				else
					return false;
			}
			else
				return false;
		}
		else
			return false;
	}
	else
		return false;
}

char* getSMSMessageContent(char incomingSMSBuffer[MAXSMSLENGTH])
{
	char buffer[MAXMSGLENGTH + 1] = "";
	for (byte index = 8; index < MAXSMSLENGTH; index++)
		buffer[index - 8] = incomingSMSBuffer[index];
	return buffer;
}

byte getSMSMessageContentNumeric(char incomingSMSBuffer[MAXSMSLENGTH])
{
	return ((incomingSMSBuffer[8] - 48) * 1);
}

void clearSMS()
{
	int8_t smsMessageIndex = fona.getNumSMS();
	while (smsMessageIndex > 0)
	{
		fona.deleteSMS(smsMessageIndex);
		smsMessageIndex--;
	}
}

/*
012345678901234567890123456789
ADD:ADMIN:passcode
ADD:USER1
ADD:USER2
ADD:USER3
ADD:USER4
RELEASE:ADMIN:passcode
RELEASE:USER1:+919701387490
RELEASE:USER2:+919701387490
RELEASE:USER3:+919701387490
RELEASE:USER4:+919701387490
*/

void checkAdminNumberRequestStatus()
{
	if (!adminNumberReceived)
		sendAdminNumberRequest = true;
}

