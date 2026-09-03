#include <Arduino.h>

// car_data = {"R": 0, "S": 0, "OT": 0, "CLT": 0, "IAT": 0, "EGT": 0, "V": 0.0}
// base_data = {"lap": "01", "total_laps": "10", "delta": "0"}

struct CarData {
    uint16_t rpm = 0;
    uint8_t speed = 0;
    uint8_t oil_temperature = 0;
    uint8_t coolant_temperature = 0;
    uint8_t intake_temperature = 0;
    uint16_t exhaust_temperature = 0;
    float voltage = 0.0F;
};

struct BaseData {
    uint8_t current_lap = 1;
    uint8_t total_laps = 10;
    int16_t delta = 0;
};