//
// Created by Damian Bigajczyk on 10.08.2021.
//

#ifndef F1XX_PROJECT_TEMPLATE_BATTERYPACK_HPP
#define F1XX_PROJECT_TEMPLATE_BATTERYPACK_HPP
#include "cstdint"
#define ADDRESS_LENGTH 17
#define FRAME_LENGTH 166
#define NUM_OF_CELLS 14

enum class Status: uint8_t {
    normal = 0,
    charging = 1,
    discharging = 2
};
enum class Cell: uint8_t {
    cell_1 = 0,
    cell_2 = 1,
    cell_3 = 2,
    cell_4 = 3,
    cell_5 = 4,
    cell_6 = 5,
    cell_7 = 6,
    cell_8 = 7,
    cell_9 = 8,
    cell_10 = 9,
    cell_11 = 10,
    cell_12 = 11,
    cell_13 = 12,
    cell_14 = 13
};

class BatteryPack {
public:

    BatteryPack(const uint8_t addr[]);

    void setAddress(const uint8_t addr[]);
    void setFrame(uint8_t data[]);

    uint8_t* getPointerToAddress();
    uint8_t* getPointerToFrame();

    Status getState();
    uint16_t getCellVol(Cell n);
    uint16_t getBattVol();
    uint16_t getChargingCurrent();
    uint16_t getDischargingCurrent();
    uint16_t getChargeLevelPercentage();
    uint16_t getChargeLevelAh();
    uint16_t getCapacity();

private:

    uint8_t address[ADDRESS_LENGTH];
    uint8_t frame[FRAME_LENGTH];
    Status state;
    uint16_t battVoltage;
    uint16_t cellVoltage[NUM_OF_CELLS];
    uint16_t chargingCurrent;
    uint16_t dischargingCurrent;
    uint16_t chargeLevelPercentage;
    uint16_t chargeLevelAh;
    uint16_t capacity;

    void setCellVol(uint8_t cell);
    void setBattVol();
    void setChargingCurrent();
    void setDischargingCurrent();
    void setState();
    void setChargeLevelPercentage();
    void setChargeLevelAh();
    void setCapacity();

    uint16_t convertHexToDec(uint8_t start, uint8_t end);
};
#endif //F1XX_PROJECT_TEMPLATE_BATTERYPACK_HPP

/*
 * Inny adres - ale otrzymujemy inną ramkę 232 bajty
 * uint8_t b_addr_2[17] = {':', '0', '0', '0', '1', '0', '0', '0', '0', '1', '1', 'B', 'M', 'S', '3', 'A', '~'};
 * Opis ramki danych
 *
 * frame[0] ':' - znak poczatku danych
 * frame[165] '~' - znak konca ramki
 *
 * frame[1:11] - ?
 * frame[11:25] - zeros
 * frame[25:29] - temp?
 * frame[29:31] - ilosc cel
 * frame[31:35] - cell_1
 * frame[35:39] - cell_2
 * frame[39:43] - cell_3
 * frame[43:47] - cell_4
 * frame[47:51] - cell_5
 * frame[51:55] - cell_6
 * frame[55:59] - cell_7
 * frame[59:63] - cell_8
 * frame[63:67] - cell_9
 * frame[67:71] - cell_10
 * frame[71:75] - cell_11
 * frame[75:79] - cell_12
 * frame[79:83] - cell_13
 * frame[83:87] - cell_14
 * frame[87:91] - prąd ładowania
 * frame[91:95] - prąd rozładowywania
 * frame[95:97] - ?
 * frame[97:99] - ?
 * frame[99:101] - ?
 * frame[101:103] - ?
 * frame[103:105] - ?
 * frame[105:107] - ?
 * frame[107:111] - zeros
 * frame[111:115] - state: 0-normal, 1-charging, 2-discharging
 * frame[115:123] - zeros
 * frame[123:125] - ?
 * frame[125:133] - zeros
 * frame[133:141] - zeros
 * frame[141:149] - ?
 * frame[149:153] - ?
 * frame[153:155] - stopień naładowania [%]
 * frame[155:157] - zeros
 * frame[157:159] - stopień naładowania [Ah]
 * frame[159:161] - zeros
 * frame[161:163] - ?
 * frame[163:164] - fabryczna pojemność [Ah]
 * frame[164:165] - liczba kontrolna
 */