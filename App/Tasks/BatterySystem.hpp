//
// Created by Damian Bigajczyk on 10.08.2021.
//

#ifndef F1XX_PROJECT_TEMPLATE_BATTERYSYSTEM_HPP
#define F1XX_PROJECT_TEMPLATE_BATTERYSYSTEM_HPP

#include <TaskManager.hpp>
#include <Hardware.hpp>
#include <BatteryPack.hpp>

struct BatterySystem : public Task {
    BatterySystem();
    void initialize() override;
    void run() override;

private:
    const uint8_t batteryAddress[ADDRESS_LENGTH] = {':', '0', '0', '0', '2', '0', '0', '0', '0', '1', '1', 'B', 'M', 'S', '3', '9', '~'};
    uint8_t receivedFrame[FRAME_LENGTH] = {0};
    BatteryPack pack;
    uint8_t valueToSend[6] = {0};

    void getData();
    void sendData();
    bool isFrameValid();
    void sendDataCan();

    void sendDataUart();
    void convertToString(uint16_t value);
};

#endif //F1XX_PROJECT_TEMPLATE_BATTERYSYSTEM_HPP
