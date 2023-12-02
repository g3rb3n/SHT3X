
#ifndef __SHT30_H
#define __SHT30_H

#include "CRCSlow.h"

#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

#include "Wire.h"

class SHT30
{
public:
	SHT30(uint8_t address = 0x45);
    void begin();
	uint8_t read();
    float temperature();
    float fahrenheit();
    float kelvin();
    float humidity();

private:
	uint8_t address;
	uint16_t temperatureReading;
	uint16_t humidityReading;
    ootb::CRCSlow crcMethod = ootb::CRCSlow(0x31, 0xFF, 0x00);

    bool checkCRC(uint8_t const *data, uint16_t length, uint8_t crc);
};


#endif
