#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

#include <oled.h>

const uint8_t OLED_DIN = 21;
const uint8_t OLED_CLK = 22;

const uint8_t OLED_1_ADDRESS = 0x3C;
const uint8_t OLED_2_ADDRESS = 0x3D;

const uint8_t OLED_WIDTH = 128;
const uint8_t OLED_HEIGHT = 128;

constexpr uint8_t FLAG_WIDTH = 16;
constexpr uint8_t FLAG_HEIGHT = 16;

const uint8_t FLAG_ICON[] PROGMEM = {
    0x20, 0x00,
    0x33, 0xCC,
    0x39, 0xCC,
    0x39, 0xFC,
    0x26, 0x70,
    0x26, 0x70,
    0x27, 0xCC,
    0x39, 0xCC,
    0x39, 0xFC,
    0x20, 0x00,
    0x20, 0x00,
    0x20, 0x00,
    0x20, 0x00,
    0x20, 0x00,
    0x20, 0x00,
    0x20, 0x00
};

Adafruit_SH1107 oled_0x3c(
    OLED_WIDTH,
    OLED_HEIGHT,
    &Wire,
    -1
);

Adafruit_SH1107 oled_0x3d(
    OLED_WIDTH,
    OLED_HEIGHT,
    &Wire,
    -1
);

bool oled_setup() {
    Wire.begin(OLED_DIN, OLED_CLK);
    Wire.setClock(400000);

    if (!oled_0x3c.begin(OLED_1_ADDRESS, true)) {
        Serial.printf("Error while initializing OLED 0x3C.\n");

        return false;
    }

    if (!oled_0x3d.begin(OLED_2_ADDRESS, false)) {
        Serial.printf("Error while initializing OLED 0x3D.\n");

        return false;
    }

    oled_0x3c.clearDisplay();
    oled_0x3d.clearDisplay();

    oled_0x3c.setTextColor(SH110X_WHITE);
    oled_0x3d.setTextColor(SH110X_WHITE);

    oled_0x3c.display();
    oled_0x3d.display();

    // oled_0x3d.setRotation(2);

    return true;
}

void oled_test() {
    oled_0x3c.setCursor(0, 0);
    oled_0x3c.printf("0x3C");
    oled_0x3c.display();

    oled_0x3d.setCursor(0, 0);
    oled_0x3d.printf("0x3D");
    oled_0x3d.display();
}

void oled_draw_0x3c(uint8_t speed) {
    oled_0x3c.clearDisplay();

    oled_0x3c.setCursor(OLED_WIDTH / 5, OLED_HEIGHT / 4);
    oled_0x3c.setTextSize(8);
    oled_0x3c.printf("%d", speed);

    oled_0x3c.setCursor(OLED_WIDTH / 2 - 46 / 2, OLED_HEIGHT * 3 / 4);
    oled_0x3c.setTextSize(2);
    oled_0x3c.printf("KM/H");

    oled_0x3c.display();
}

void oled_draw_0x3d(uint8_t current_lap, uint8_t total_laps, int16_t delta) {
    oled_0x3d.clearDisplay();

    oled_0x3d.drawBitmap(
        8,
        8,
        FLAG_ICON,
        FLAG_WIDTH,
        FLAG_HEIGHT,
        SH110X_WHITE
    );

    oled_0x3d.setCursor(30, 10);
    oled_0x3d.setTextSize(2);
    oled_0x3d.printf("%d/%d", current_lap, total_laps);

    oled_0x3d.setCursor(0, OLED_HEIGHT / 2);
    if (delta >= 0) {
        oled_0x3d.setTextSize(3);
        oled_0x3d.printf("D:+%d s", delta);
        oled_0x3d.setCursor(OLED_WIDTH / 4, OLED_HEIGHT * 3 / 4);
        oled_0x3d.setTextSize(2);
        oled_0x3d.printf("ZAPAS");
    } else {
        oled_0x3d.setTextSize(3);
        oled_0x3d.printf("D:%d s", delta);
        oled_0x3d.setCursor(OLED_WIDTH / 4, OLED_HEIGHT * 3 / 4);
        oled_0x3d.setTextSize(2);
        oled_0x3d.printf("STRATA");
    }

    oled_0x3d.display();
}