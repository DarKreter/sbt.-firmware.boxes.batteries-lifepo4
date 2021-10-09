//
// Created by Damian Bigajczyk on 10.08.2021.
//
#include "BatterySystem.hpp"

typedef uint16_t (BatteryPack::*BatMemFn)(void);
BatMemFn wsk[] = {&BatteryPack::getChargingCurrent,
                  &BatteryPack::getDischargingCurrent,
                  &BatteryPack::getState,
                  &BatteryPack::getChargeLevelPercentage,
                  &BatteryPack::getChargeLevelAh,
                  &BatteryPack::getCapacity,
                  &BatteryPack::getBatVol
};

BatterySystem::BatterySystem(UART& uart, GPIO_TypeDef* gpio, uint32_t pin, ParameterId batID)
                                : Task({}, 1000, 2),
                                uart(uart),
                                selectedPort(gpio),
                                selectedPin(pin),
                                batteryID(batID),
                                pack(batteryAddress){}

void BatterySystem::initialize() {
    Hardware::configureClocks();
    Hardware::enableGpio(selectedPort, selectedPin, Gpio::Mode::Output);
    Hardware::enableGpio(selectedPort, selectedPin, Gpio::Mode::Output);
    Hardware::enableGpio(GPIOC, GPIO_PIN_13, Gpio::Mode::Output);

    if (!Hardware::can.IsInitialized())
        Hardware::can.Initialize(BoxId::BOX3, {});

    uart.ChangeModeToBlocking(1000);
    uart.SetBaudRate(9600);
    uart.Initialize();
}

void BatterySystem::run() {
    HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
    getData();
    verifyFrame();
    sendDataCAN();
}

void BatterySystem::getData() {
    HAL_GPIO_WritePin(selectedPort, selectedPin, GPIO_PIN_SET);
    uart.Send(pack.getPointerToAddress(), ADDRESS_LENGTH);
    HAL_GPIO_WritePin(selectedPort, selectedPin, GPIO_PIN_RESET);
    uart.Receive(receivedFrame, FRAME_LENGTH);
}

void BatterySystem::verifyFrame() {
    pack.setFrame((receivedFrame[0] == ':' && receivedFrame[165] == '~')? receivedFrame : clearFrame);
}

void BatterySystem::sendDataCAN() {
    for (int i = 0; i < RANGE_OF_POINTER_ARRAY; ++i) {
        Hardware::can.Send(static_cast<ParameterId>(static_cast<int>(batteryID) + i),\
        static_cast<int32_t>(CALL_MEMBER_FN(pack, wsk[i])()));
        vTaskDelay(5);
    }

    Hardware::can.Send(static_cast<ParameterId>(static_cast<int>(batteryID) + 7), pack.getPower());
    vTaskDelay(5);

    for (int i = 0, j = OFFSET_ID_PARAMETERS; i < NUM_OF_CELLS; ++i, ++j) {
        Hardware::can.Send(static_cast<ParameterId>(static_cast<int>(batteryID) + j), static_cast<int32_t>(pack.getCellVol(static_cast<Cell>(i))));
        vTaskDelay(5);
    }
}
/**
 ** functions to debug
**/
bool BatterySystem::isFrameValid() {
    if (receivedFrame[0] == ':' && receivedFrame[165] == '~') {
        pack.setFrame(receivedFrame);
        return true;
    } else {
        pack.setFrame(clearFrame);
        return false;
    }
}

void BatterySystem::sendDataUart() {
    convertToString(pack.getBatVol());
    Hardware::uart1.Send(valueToSend, sizeof(valueToSend));
    convertToString(pack.getCellVol(Cell::cell_1));
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

