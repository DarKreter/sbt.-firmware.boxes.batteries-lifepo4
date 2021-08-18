//
// Created by Damian Bigajczyk on 10.08.2021.
//

#include "BatteryPack.hpp"

BatteryPack::BatteryPack(const uint8_t *addr) {
    for (int i = 0; i < ADDRESS_LENGTH; ++i) {
        address[i] = addr[i];
    }
}

void BatteryPack::setAddress(const uint8_t *addr) {
    for (int i = 0; i < ADDRESS_LENGTH; i++) {
        address[i] = addr[i];
    }
}

void BatteryPack::setFrame(uint8_t *data) {
    if (*data == ':' && *(data+165) == '~') {
        for (int i = 0; i<FRAME_LENGTH; i++) {
            frame[i] = data[i];
        }
    }
}

uint8_t* BatteryPack::getPointerToAddress() {
    return address;
}

uint8_t* BatteryPack::getPointerToFrame() {
    return frame;
}

Status BatteryPack::getState() {
    setState();
    return state;
}

void BatteryPack::setState() {
    state = static_cast<Status>(convertHexToDec(111, 114));
}

uint16_t BatteryPack::getCellVol(Cell x) {
    setCellVol(static_cast<uint8_t>(x));
    return cellVoltage[static_cast<uint8_t>(x)];
}

void BatteryPack::setCellVol(uint8_t cell) {
    cellVoltage[cell] = convertHexToDec(31+cell*4, 34+cell*4);
}

uint16_t BatteryPack::getBattVol() {
    setBattVol();
    return battVoltage;
}

void BatteryPack::setBattVol() {
    battVoltage = 0;
    for (int i = 0; i<NUM_OF_CELLS; ++i) {
        setCellVol(i);
        battVoltage += cellVoltage[i];
    }
}

uint16_t BatteryPack::getChargingCurrent() {
    setChargingCurrent();
    return chargingCurrent;
}

void BatteryPack::setChargingCurrent() {
    chargingCurrent = convertHexToDec(87, 90);
}

uint16_t BatteryPack::getDischargingCurrent() {
    setDischargingCurrent();
    return dischargingCurrent;
}

void BatteryPack::setDischargingCurrent() {
    dischargingCurrent = convertHexToDec(91, 94);
}

uint16_t BatteryPack::getChargeLevelPercentage() {
    setChargeLevelPercentage();
    return chargeLevelPercentage;
}

void BatteryPack::setChargeLevelPercentage() {
    chargeLevelPercentage = convertHexToDec(153, 154);
}

uint16_t BatteryPack::getChargeLevelAh() {
    setChargeLevelAh();
    return chargeLevelAh;
}

void BatteryPack::setChargeLevelAh() {
    chargeLevelAh = convertHexToDec(157, 158);
}

uint16_t BatteryPack::getCapacity() {
    setCapacity();
    return capacity;
}

void BatteryPack::setCapacity() {
    capacity = convertHexToDec(163, 163);
}

uint16_t BatteryPack::convertHexToDec(uint8_t start, uint8_t end) {
    uint16_t result = 0;

    while (start <= end) {
        if (frame[start] >= 48 && frame[start] <= 57) {
            result += (frame[start]-48) * (2<<(end-start)*4)>>1;
        } else {
            result += (frame[start]-55) * (2<<(end-start)*4)>>1;
        }
        ++start;
    }

    return result;
}