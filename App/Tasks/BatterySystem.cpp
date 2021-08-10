//
// Created by Damian Bigajczyk on 10.08.2021.
//

#include "BatterySystem.hpp"

BatterySystem::BatterySystem() : Task({}, 500, 2), pack(batteryAddress) {}

void BatterySystem::initialize() {
    Hardware::configureClocks();
    Hardware::uart1.ChangeModeToBlocking(1000);
    Hardware::uart1.SetBaudRate(9600);
    Hardware::uart1.Initialize();
    Hardware::uart2.ChangeModeToBlocking(1000);
    Hardware::uart2.SetBaudRate(9600);
    Hardware::uart2.Initialize();
    Hardware::enableGpio(GPIOA, GPIO_PIN_1, Gpio::Mode::Output);
}
void BatterySystem::run() {
    getData();
    sendData();
}
void BatterySystem::getData() {
    //while (!Hardware::uart2.IsRxComplete()) {}
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);
    Hardware::uart2.Send(pack.getPointerToAddress(), ADDRESS_LENGTH);
    //while (!Hardware::uart2.IsTxComplete()) {}
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
    Hardware::uart2.Receive(receivedFrame, FRAME_LENGTH);
}

void BatterySystem::sendData() {
    if (isFrameValid()) {
        //TODO (DAMIN) send data via CAN
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

void BatterySystem::convertToString(uint16_t value) {
    uint16_t digit = 0;
    for (int i = 4; i >= 0; --i) {
        digit = value%10;
        value /= 10;
        valueToSend[i] = static_cast<uint8_t>(digit) + 48;
    }
    valueToSend[5] = '\n';
}

