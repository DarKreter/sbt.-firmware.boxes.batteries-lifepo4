//
// Created by Damian Bigajczyk on 10.08.2021.
//

#ifndef F1XX_PROJECT_TEMPLATE_BATTERYSYSTEM_HPP
#define F1XX_PROJECT_TEMPLATE_BATTERYSYSTEM_HPP

#include <TaskManager.hpp>
#include <Hardware.hpp>
#include <BatteryPack.hpp>

constexpr bool WORK_WITH_TWO_UARTs = true;
constexpr uint8_t RANGE_OF_POINTER_ARRAY = 7;
constexpr uint8_t OFFSET_ID_PARAMETERS = 8;

enum class battery : uint16_t{
    bat1 = 0x60,
    bat2 = 0x80,
    bat3 = 0xA0,
    bat4 = 0xC0
};

struct BatterySystem : public Task {
    BatterySystem(UART& uart, GPIO_TypeDef* gpio, uint32_t pin, ParameterId battID);
    void initialize() override;
    void run() override;

private:
    const uint8_t batteryAddress[ADDRESS_LENGTH] = {':', '0', '0', '0', '2', '0', '0', '0', '0', '1', '1', 'B', 'M', 'S', '3', '9', '~'};
    uint8_t receivedFrame[FRAME_LENGTH] = {0};
    const uint8_t clearFrame[FRAME_LENGTH] = {0};
    uint8_t valueToSend[6] = {0};
    UART& uart;
    GPIO_TypeDef* selectedPort;
    uint32_t selectedPin;
    ParameterId batteryID;


    BatteryPack pack;

    void getData();
    void verifyFrame();
    void sendDataCAN();
    bool isFrameValid();


    void sendDataUart();
    void convertToString(uint16_t value);
};

#endif //F1XX_PROJECT_TEMPLATE_BATTERYSYSTEM_HPP
