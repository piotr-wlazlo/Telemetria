#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

#include <oled.h>

const uint8_t OLED_DIN = 21;
const uint8_t OLED_CLK = 22;

const uint8_t OLED_1_ADDRESS = 0x3C;
const uint8_t OLED_2_ADDRESS = 0x3D;

const uint8_t OLED_WIDTH = 128;
const uint8_t OLED_HEIGHT = 128;

Adafruit_SH1107 oled1(
    OLED_WIDTH,
    OLED_HEIGHT,
    &Wire,
    -1
);

Adafruit_SH1107 oled2(
    OLED_WIDTH,
    OLED_HEIGHT,
    &Wire,
    -1
);

bool oled_setup() {
    Wire.begin(OLED_DIN, OLED_CLK);
    Wire.setClock(400000);

    if (!oled1.begin(OLED_1_ADDRESS, true)) {
        Serial.printf("Error while initializing OLED 0x3C.\n");

        return false;
    }

    if (!oled2.begin(OLED_2_ADDRESS, false)) {
        Serial.printf("Error while initializing OLED 0x3D.\n");

        return false;
    }

    oled1.clearDisplay();
    oled2.clearDisplay();

    oled1.setTextColor(SH110X_WHITE);
    oled2.setTextColor(SH110X_WHITE);

    oled1.display();
    oled2.display();

    return true;
}

void oled_test() {
    oled1.setCursor(0, 0);
    oled1.printf("0x3C");
    oled1.display();

    oled2.setCursor(0, 0);
    oled2.printf("0x3D");
    oled2.display();
}