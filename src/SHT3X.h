
#ifndef __SHT3X_H
#define __SHT3X_H

#include "CRCSlow.h"

#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

#include "Wire.h"

class SHT3X
{
public:
	SHT3X(uint8_t address = 0x45);
    void begin();
	uint8_t read();
    float temperature();
    float fahrenheit();
    float kelvin();
    float humidity();

    bool setHeater(bool on);

private:
	uint8_t address;
	uint16_t temperatureReading;
	uint16_t humidityReading;
    ootb::CRCSlow crcMethod = ootb::CRCSlow(0x31, 0xFF, 0x00);

    bool sendReadCommandStretchingHigh();
    bool send(uint8_t msb, uint8_t lsb);

    bool checkCRC(uint8_t const *data, uint16_t length, uint8_t crc);
};


#endif

