#include <Arduino.h>

#include <oled.h>

void setup() {
	Serial.begin(115200);

	if (!oled_setup()) {
		Serial.printf("Error while initializing OLEDs.\n");
		return;
	}

	Serial.printf("OLEDs initialized.");

	oled_test();
}

void loop() {

}