//
// Created by Damian Bigajczyk on 10.08.2021.
//

#include "BatterySystem.hpp"

BatterySystem::BatterySystem() : Task({}, 1000, 2), pack(batteryAddress), pack_2(batteryAddress) {}

void BatterySystem::initialize() {
    Hardware::configureClocks();

    Hardware::uart1.ChangeModeToBlocking(1000);
    Hardware::uart1.SetBaudRate(9600);
    Hardware::uart1.Initialize();
    Hardware::uart2.ChangeModeToBlocking(1000);
    Hardware::uart2.SetBaudRate(9600);
    Hardware::uart2.Initialize();
    Hardware::uart3.ChangeModeToBlocking(1000);
    Hardware::uart3.SetBaudRate(9600);
    Hardware::uart3.Initialize();

    Hardware::can.Initialize(0x100, {});

    Hardware::enableGpio(GPIOA, GPIO_PIN_1, Gpio::Mode::Output);
}
void BatterySystem::run() {
    getData(1);
    isFrameValid(pack_1);
    getData(2);
    isFrameValid(pack_2);
    sendData();
}
void BatterySystem::getData(const uint8_t choice) {
    switch (choice) {
        case 2:
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);
            Hardware::uart3.Send(pack_2.getPointerToAddress(), ADDRESS_LENGTH);
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
            Hardware::uart3.Receive(receivedFrame, FRAME_LENGTH);
        default:
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);
            Hardware::uart2.Send(pack_1.getPointerToAddress(), ADDRESS_LENGTH);
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
            Hardware::uart2.Receive(receivedFrame, FRAME_LENGTH);
    }
}

/*void BatterySystem::sendData() {
    if (isFrameValid()) {
        sendDataCan();
    } else
        Hardware::uart1.Send(receivedFrame, sizeof(receivedFrame));
}*/

bool BatterySystem::isFrameValid(BatteryPack& pack) {
    if (receivedFrame[0] == ':' && receivedFrame[165] == '~') {
        pack.setFrame(receivedFrame);
        return true;
    } else {
        pack.setFrame(clearFrame);
        return false;
    }
}

void BatterySystem::sendDataCan() {
    Hardware::can.Send(0x60, static_cast<int32_t>(pack_1.getChargingCurrent()));
    Hardware::can.Send(0x61, static_cast<int32_t>(pack_1.getDischargingCurrent()));
    Hardware::can.Send(0x62, static_cast<int32_t>(pack_1.getState()));
    Hardware::can.Send(0x63, static_cast<int32_t>(pack_1.getChargeLevelPercentage()));
    Hardware::can.Send(0x64, static_cast<int32_t>(pack_1.getChargeLevelAh()));
    Hardware::can.Send(0x65, static_cast<int32_t>(pack_1.getCapacity()));
    Hardware::can.Send(0x66, static_cast<int32_t>(pack_1.getBattVol()));
    Hardware::can.Send(0x67, static_cast<int32_t>(pack_1.getCellVol(Cell::cell_1)));
    Hardware::can.Send(0x68, static_cast<int32_t>(pack_1.getCellVol(Cell::cell_2)));
    Hardware::can.Send(0x69, static_cast<int32_t>(pack_1.getCellVol(Cell::cell_3)));

    Hardware::can.Send(0x70, static_cast<int32_t>(pack_2.getChargingCurrent()));
    Hardware::can.Send(0x71, static_cast<int32_t>(pack_2.getDischargingCurrent()));
    Hardware::can.Send(0x72, static_cast<int32_t>(pack_2.getState()));
    Hardware::can.Send(0x73, static_cast<int32_t>(pack_2.getChargeLevelPercentage()));
    Hardware::can.Send(0x74, static_cast<int32_t>(pack_2.getChargeLevelAh()));
    Hardware::can.Send(0x75, static_cast<int32_t>(pack_2.getCapacity()));
    Hardware::can.Send(0x76, static_cast<int32_t>(pack_2.getBattVol()));
    Hardware::can.Send(0x77, static_cast<int32_t>(pack_2.getCellVol(Cell::cell_1)));
    Hardware::can.Send(0x78, static_cast<int32_t>(pack_2.getCellVol(Cell::cell_2)));
    Hardware::can.Send(0x79, static_cast<int32_t>(pack_2.getCellVol(Cell::cell_3)));
}

void BatterySystem::sendDataUart() {
    convertToString(pack_1.getBattVol());
    Hardware::uart1.Send(valueToSend, sizeof(valueToSend));
    convertToString(pack_1.getCellVol(Cell::cell_1));
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

