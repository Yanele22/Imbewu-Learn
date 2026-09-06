#include <SoftwareSerial.h>

#define RS485_TX 11
#define RS485_RX 10
#define RS485_EN 8

SoftwareSerial rs485(RS485_RX, RS485_TX);
String currentPacket = "";

void setup() {
	pinMode(RS485_EN, OUTPUT);
	digitalWrite(RS485_EN, LOW);
	Serial.begin(9600);
	rs485.begin(9600);
}

void loop() {
	if (rs485.available()) {
		currentPacket = rs485.readStringUntil('\n');
	}

	if (Serial.available()) {
		char option = Serial.read();
		if (option == '1' || option == '2') {
			Serial.println("\n--- DATA RECEIVED ---");
			Serial.println(currentPacket);
			Serial.println("Press any key to return...");
			while (!Serial.available());
		}
	}
}