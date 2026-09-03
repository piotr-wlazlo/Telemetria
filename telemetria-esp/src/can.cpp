#include <Arduino.h>
#include <ESP32-TWAI-CAN.hpp>

#include "can.h"

const uint8_t CAN_RX = 25;
const uint8_t CAN_TX = 26;

const uint32_t CAN_DATA_ADDRESS_1 = 0x607;
const uint32_t CAN_DATA_ADDRESS_2 = 0x608;

bool can_setup() {
    return ESP32Can.begin(TWAI_SPEED_500KBPS, CAN_TX, CAN_RX);
}

bool can_receive(CarData& car_data) {
    CanFrame frame;

    if (!ESP32Can.readFrame(frame, 0)) {
        return false;
    }

    // nie wiem po co to ograniczenie (było w xbee_sender) - zweryfikować
    if (!frame.extd) {
        return false;
    }

    if (frame.identifier == CAN_DATA_ADDRESS_1) {
        if (frame.data_length_code < 4) {
            return false;
        }

        car_data.rpm = frame.data[0] * 256 + frame.data[1];
        car_data.speed = frame.data[2];
        car_data.voltage = frame.data[3] / 10.0F;

        return true;
    }

    if (frame.identifier == CAN_DATA_ADDRESS_2) {
        if (frame.data_length_code < 5) {
            return false;
        }

        car_data.oil_temperature = frame.data[0];
        car_data.intake_temperature = frame.data[1];
        car_data.coolant_temperature = frame.data[2];
        car_data.exhaust_temperature = frame.data[3] * 256 + frame.data[4];

        return true;
    }

    return false;
}

