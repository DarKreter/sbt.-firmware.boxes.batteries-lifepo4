//
// Created by Damian Bigajczyk on 10.08.2021.
//

#include "BatterySystem.hpp"

BatterySystem::BatterySystem() : Task({}, 1000, 2), pack(batteryAddress) {}

void BatterySystem::initialize() {
    Hardware::configureClocks();

    Hardware::uart1.ChangeModeToBlocking(1000);
    Hardware::uart1.SetBaudRate(9600);
    Hardware::uart1.Initialize();
    Hardware::uart2.ChangeModeToBlocking(1000);
    Hardware::uart2.SetBaudRate(9600);
    Hardware::uart2.Initialize();

    Hardware::can.Initialize(0x10, {});

    Hardware::enableGpio(GPIOA, GPIO_PIN_1, Gpio::Mode::Output);
}
void BatterySystem::run() {
    getData();
    sendData();
}
void BatterySystem::getData() {
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);
    Hardware::uart2.Send(pack.getPointerToAddress(), ADDRESS_LENGTH);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
    Hardware::uart2.Receive(receivedFrame, FRAME_LENGTH);
}

void BatterySystem::sendData() {
    if (isFrameValid()) {
        sendDataCan();
        sendDataUart();
    } else
        Hardware::uart1.Send(receivedFrame, sizeof(receivedFrame));
}

bool BatterySystem::isFrameValid() {
    if (receivedFrame[0] == ':' && receivedFrame[165] == '~') {
        pack.setFrame(receivedFrame);
        return true;
    } else {
        return false;
    }
}

void BatterySystem::sendDataCan() {
    Hardware::can.Send(0x60, static_cast<int32_t>(pack.getChargingCurrent()));
    Hardware::can.Send(0x61, static_cast<int32_t>(pack.getDischargingCurrent()));
    Hardware::can.Send(0x62, static_cast<int32_t>(pack.getState()));
    Hardware::can.Send(0x63, static_cast<int32_t>(pack.getChargeLevelPercentage()));
    Hardware::can.Send(0x64, static_cast<int32_t>(pack.getChargeLevelAh()));
    Hardware::can.Send(0x65, static_cast<int32_t>(pack.getCapacity()));
    Hardware::can.Send(0x66, static_cast<int32_t>(pack.getBattVol()));
    Hardware::can.Send(0x67, static_cast<int32_t>(pack.getCellVol(Cell::cell_1)));
    Hardware::can.Send(0x68, static_cast<int32_t>(pack.getCellVol(Cell::cell_2)));
    Hardware::can.Send(0x69, static_cast<int32_t>(pack.getCellVol(Cell::cell_3)));
}

void BatterySystem::sendDataUart() {
    convertToString(pack.getCellVol(Cell::cell_1));
    Hardware::uart1.Send(valueToSend, sizeof(valueToSend));
    convertToString(pack.getCellVol(Cell::cell_2));
    Hardware::uart1.Send(valueToSend, sizeof(valueToSend));
    convertToString(pack.getCellVol(Cell::cell_3));
    Hardware::uart1.Send(valueToSend, sizeof(valueToSend));
    convertToString(pack.getBattVol());
    Hardware::uart1.Send(valueToSend, sizeof(valueToSend));
    convertToString(pack.getChargeLevelAh());
    Hardware::uart1.Send(valueToSend, sizeof(valueToSend));
    convertToString(pack.getChargeLevelPercentage());
    Hardware::uart1.Send(valueToSend, sizeof(valueToSend));
    convertToString(static_cast<uint16_t>(pack.getState()));
    Hardware::uart1.Send(valueToSend, sizeof(valueToSend));
}

void BatterySystem::convertToString(uint16_t value) {
    uint16_t digit = 0;
    for (int i = 4; i >= 0; --i) {
        digit = value%10;
        value /= 10;
        valueToSend[i] = static_cast<uint8_t>(digit) + 48;
    }
    valueToSend[5] = '\n';
}

