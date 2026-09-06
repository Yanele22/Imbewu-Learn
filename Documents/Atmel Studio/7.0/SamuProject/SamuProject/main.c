/*
 * main.c
 *
 * Created: 5/13/2026 11:16:38 PM
 *  Author: samke
 */ 

#include <LiquidCrystal.h>
#include <Wire.h>
#include <RTClib.h>
#include <SoftwareSerial.h>

// Hardware Pin Assignments
#define LM35 A0
#define LDR A1
#define RS485_TX 11
#define RS485_RX 10
#define RS485_EN 8

LiquidCrystal lcd(7, 6, 5, 4, 3, 2);
RTC_DS1307 rtc;
SoftwareSerial rs485(RS485_RX, RS485_TX);

// Global Variables & States
float temperature;
int lightLevel;
String tempStatus, lightStatus;
unsigned long highTempStart = 0, darkStart = 0;
unsigned long highTempDuration = 0, darkDuration = 0;

void setup() {
	pinMode(RS485_EN, OUTPUT);
	digitalWrite(RS485_EN, LOW); // Set MAX485 to Receive Mode by default
	
	lcd.begin(16, 2);
	Serial.begin(9600);
	rs485.begin(9600);
	Wire.begin();
	rtc.begin();
	
	lcd.clear();
	lcd.print("System Ready");
	delay(1000);
}

void loop() {
	readSensors();
	checkConditions();
	displayLCD();
	sendPacket();
	delay(1000); // 1-second operational loop cycle
}

void readSensors() {
	// Convert LM35 raw voltage to Celsius scale
	int rawTemp = analogRead(LM35);
	temperature = (rawTemp * 5.0 * 100.0) / 1023.0;
	
	if (temperature < 15) tempStatus = "LOW";
	else if (temperature <= 30) tempStatus = "NORMAL";
	else tempStatus = "HIGH";

	// Map LDR voltage divider reading to inverted 0-100% scale
	int rawLight = analogRead(LDR);
	lightLevel = map(rawLight, 0, 1023, 0, 100);
	
	if (lightLevel > 70) lightStatus = "BRIGHT";
	else if (lightLevel >= 30) lightStatus = "NORMAL";
	else lightStatus = "DARK";
}

void checkConditions() {
	// Continuous tracking of High Temperature condition duration
	if (tempStatus == "HIGH") {
		if (highTempStart == 0) {
			highTempStart = millis();
		}
		highTempDuration = (millis() - highTempStart) / 1000;
		} else {
		highTempStart = 0;
		highTempDuration = 0;
	}

	// Continuous tracking of Dark condition duration
	if (lightStatus == "DARK") {
		if (darkStart == 0) {
			darkStart = millis();
		}
		darkDuration = (millis() - darkStart) / 1000;
		} else {
		darkStart = 0;
		darkDuration = 0;
	}
}

void displayLCD() {
	DateTime now = rtc.now();
	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print("T:"); lcd.print((int)temperature); lcd.print("C "); lcd.print(tempStatus);
	lcd.setCursor(12, 0);
	lcd.print(lightLevel); lcd.print("%");
	
	lcd.setCursor(0, 1);
	if (now.hour() < 10) lcd.print("0");
	lcd.print(now.hour()); lcd.print(":");
	if (now.minute() < 10) lcd.print("0");
	lcd.print(now.minute()); lcd.print(":");
	if (now.second() < 10) lcd.print("0");
	lcd.print(now.second());
}

// Safely prints durations padded to 6 digits (HHMMSS)
void printFormattedDuration(unsigned long duration) {
	int h = (duration / 3600) % 24;
	int m = (duration / 60) % 60;
	int s = duration % 60;
	
	char durBuf[7];
	snprintf(durBuf, sizeof(durBuf), "%02d%02d%02d", h, m, s);
	rs485.print(durBuf);
}

void sendPacket() {
	DateTime now = rtc.now();
	
	// Set MAX485 to Transmission Mode
	digitalWrite(RS485_EN, HIGH);
	delay(10); // Hardware stabilization buffer
	
	rs485.print("T"); rs485.print((int)temperature);
	rs485.print(",L"); rs485.print(lightLevel);
	
	// FIXED: Replaced unsafe SoftwareSerial.printf with string buffer parsing
	char timeBuf[7];
	snprintf(timeBuf, sizeof(timeBuf), "%02d%02d%02d", now.hour(), now.minute(), now.second());
	rs485.print(",T"); rs485.print(timeBuf);
	
	rs485.print(",HT"); printFormattedDuration(highTempDuration);
	rs485.print(",LL"); printFormattedDuration(darkDuration);
	rs485.println(); // Delimit the packet ending string
	
	delay(20);
	rs485.flush(); // Finish byte shifting output
	digitalWrite(RS485_EN, LOW); // Return to Receive Mode
}