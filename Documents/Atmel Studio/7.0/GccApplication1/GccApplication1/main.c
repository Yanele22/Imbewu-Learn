// BTS7960 Motor Test Code
// Left driver: D5, D6
// Right driver: D9, D10
#include <WiFi.h>
#include <PubSubClient.h>
#define LEFT_RPWM 5
#define LEFT_LPWM 6

#define RIGHT_RPWM 9
#define RIGHT_LPWM 10

int speed = 150;  // Speed range: 0 - 255

void setup() {
	pinMode(LEFT_RPWM, OUTPUT);
	pinMode(LEFT_LPWM, OUTPUT);

	pinMode(RIGHT_RPWM, OUTPUT);
	pinMode(RIGHT_LPWM, OUTPUT);

	stopMotors();
}

void loop() {

	// Move forward
	moveForward();
	delay(5000);

	// Stop
	stopMotors();
	delay(2000);

	// Move backward
	moveBackward();
	delay(5000);

	// Stop
	stopMotors();
	delay(2000);
}


void moveForward() {

	// Left side forward
	analogWrite(LEFT_RPWM, speed);
	analogWrite(LEFT_LPWM, 0);

	// Right side forward
	analogWrite(RIGHT_RPWM, speed);
	analogWrite(RIGHT_LPWM, 0);
}


void moveBackward() {

	// Left side backward
	analogWrite(LEFT_RPWM, 0);
	analogWrite(LEFT_LPWM, speed);

	// Right side backward
	analogWrite(RIGHT_RPWM, 0);
	analogWrite(RIGHT_LPWM, speed);
}


void stopMotors() {

	analogWrite(LEFT_RPWM, 0);
	analogWrite(LEFT_LPWM, 0);

	analogWrite(RIGHT_RPWM, 0);
	analogWrite(RIGHT_LPWM, 0);
}