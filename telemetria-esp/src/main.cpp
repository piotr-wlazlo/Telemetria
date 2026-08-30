#include <Arduino.h>

#include "oled.h"

void setup() {
	Serial.begin(115200);

	if (!oled_setup()) {
		Serial.printf("OLEDs not initialized.\n");
		return;
	}

	Serial.printf("OLEDs initialized.\n");
}

void loop() {
	static uint8_t speed = 0;
	static int16_t delta = -100;

	if (speed >= 99) {
		speed = 0;
	} else {
		speed++;
	}

	if (delta >= 100) {
		delta = -100;
	} else {
		delta++;
	}

	oled_draw_0x3c(speed);
	oled_draw_0x3d(3, 10, delta);
}