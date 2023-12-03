#include "SHT3X.h"



SHT3X::SHT3X(uint8_t address)
:
    address(address)
{
}
#define SHT3X_ERROR_WRITE  1
#define SHT3X_ERROR_READ  2
#define SHT3X_ERROR_CRC  3

#define SHT3X_READ_CLOCK_STRETCHING_ENABLED_MSB 0x2C
#define SHT3X_READ_CLOCK_STRETCHING_ENABLED_REPEATABILITY_LOW_LSB 0x10
#define SHT3X_READ_CLOCK_STRETCHING_ENABLED_REPEATABILITY_MEDIUM_LSB 0x0D
#define SHT3X_READ_CLOCK_STRETCHING_ENABLED_REPEATABILITY_HIGH_LSB 0x06
#define SHT3X_READ_CLOCK_STRETCHING_DISABLED_MSB 0x24
#define SHT3X_READ_CLOCK_STRETCHING_DISABLED_REPEATABILITY_LOW_LSB 0x16
#define SHT3X_READ_CLOCK_STRETCHING_DISABLED_REPEATABILITY_MEDIUM_LSB 0x0B
#define SHT3X_READ_CLOCK_STRETCHING_DISABLED_REPEATABILITY_HIGH_LSB 0x00
#define SHT3X_READ_PERIODIC_0_5_MSB 0x20
#define SHT3X_READ_PERIODIC_0_5_REPEATABILITY_HIGH_LSB 0x32
#define SHT3X_READ_PERIODIC_0_5_REPEATABILITY_MEDIUM_LSB 0x24
#define SHT3X_READ_PERIODIC_0_5_REPEATABILITY_LOW_LSB 0x2F
#define SHT3X_READ_PERIODIC_1_MSB 0x21
#define SHT3X_READ_PERIODIC_1_REPEATABILITY_HIGH_LSB 0x30
#define SHT3X_READ_PERIODIC_1_REPEATABILITY_MEDIUM_LSB 0x26
#define SHT3X_READ_PERIODIC_1_REPEATABILITY_LOW_LSB 0x2D
#define SHT3X_READ_PERIODIC_2_MSB 0x22
#define SHT3X_READ_PERIODIC_2_REPEATABILITY_HIGH_LSB 0x36
#define SHT3X_READ_PERIODIC_2_REPEATABILITY_MEDIUM_LSB 0x20
#define SHT3X_READ_PERIODIC_2_REPEATABILITY_LOW_LSB 0x2B
#define SHT3X_READ_PERIODIC_4_MSB 0x23
#define SHT3X_READ_PERIODIC_4_REPEATABILITY_HIGH_LSB 0x34
#define SHT3X_READ_PERIODIC_4_REPEATABILITY_MEDIUM_LSB 0x22
#define SHT3X_READ_PERIODIC_4_REPEATABILITY_LOW_LSB 0x29
#define SHT3X_READ_PERIODIC_10_MSB 0x27
#define SHT3X_READ_PERIODIC_10_REPEATABILITY_HIGH_LSB 0x37
#define SHT3X_READ_PERIODIC_10_REPEATABILITY_MEDIUM_LSB 0x21
#define SHT3X_READ_PERIODIC_10_REPEATABILITY_LOW_LSB 0x2A
#define SHT3X_READ_PERIODIC_MSB 0xE0
#define SHT3X_READ_PERIODIC_LSB 0x00
#define SHT3X_READ_PERIODIC_ART_MSB 0x2B
#define SHT3X_READ_PERIODIC_ART_LSB 0x32
#define SHT3X_READ_PERIODIC_BREAK_MSB 0x00
#define SHT3X_READ_PERIODIC_BREAK_LSB 0x06
#define SHT3X_RESET_MSB 0x30
#define SHT3X_RESET_LSB 0xA2
#define SHT3X_HEATER_MSB 0x30
#define SHT3X_HEATER_ENABLED_LSB 0x6D
#define SHT3X_HEATER_DISABLED_LSB 0x66
#define SHT3X_STATUS_MSB 0xF3
#define SHT3X_STATUS_LSB 0x2D
#define SHT3X_CLEAR_STATUS_MSB 0x30
#define SHT3X_CLEAR_STATUS_LSB 0x41

#define SHT3X_CRC_POLYNOMIAL 0x31
#define SHT3X_CRC_INIT 0xFF
#define SHT3X_CRC_FINAL_XOR 0x00


void SHT3X::begin()
{
    Wire.begin();
}

float SHT3X::temperature()
{
    float temp = temperatureReading;
    return temp * 175 / 65535 - 45;
}

float SHT3X::fahrenheit()
{
    return temperature() * 1.8 + 32;
}

float SHT3X::kelvin()
{
    return temperature() + 273.15;
}

float SHT3X::humidity()
{
    float temp = humidityReading;
    return temp * 100 / 65535;
}

bool SHT3X::checkCRC(uint8_t const *data, uint16_t length, uint8_t crc)
{
    return crcMethod.crc(data, 2) == crc;
}

bool SHT3X::send(uint8_t msb, uint8_t lsb)
{
	// Start I2C Transmission
	Wire.beginTransmission(address);
	// Send measurement command
	Wire.write(SHT3X_READ_CLOCK_STRETCHING_ENABLED_MSB);
	Wire.write(SHT3X_READ_CLOCK_STRETCHING_ENABLED_REPEATABILITY_HIGH_LSB);
	// Stop I2C transmission
	return Wire.endTransmission() == 0;
}

bool SHT3X::setHeater(bool on)
{
    if (on)
        return send(SHT3X_HEATER_MSB, SHT3X_HEATER_ENABLED_LSB);
    else
        return send(SHT3X_HEATER_MSB, SHT3X_HEATER_DISABLED_LSB);
}

bool SHT3X::sendReadCommandStretchingHigh()
{
    return send(SHT3X_READ_CLOCK_STRETCHING_ENABLED_MSB, SHT3X_READ_CLOCK_STRETCHING_ENABLED_REPEATABILITY_HIGH_LSB);
}

uint8_t SHT3X::read()
{
    if (!sendReadCommandStretchingHigh()) return SHT3X_ERROR_WRITE;

	uint8_t data[6];
	delay(500);

	// Request 6 bytes of data
	Wire.requestFrom(address, 6);

	// Read 6 bytes of data
	// cTemp msb, cTemp lsb, cTemp crc, humidity msb, humidity lsb, humidity crc
	for (int i = 0 ; i < 6 ; ++i)
		data[i] = Wire.read();
	
	delay(50);
	
	if (Wire.available() != 0) return SHT3X_ERROR_READ;

	// Convert the data
    if (!checkCRC(data, 2, data[2])) return SHT3X_ERROR_CRC;
    if (!checkCRC(data + 3, 2, data[5])) return SHT3X_ERROR_CRC;
    temperatureReading = data[0] * 256 + data[1];
    humidityReading = data[3] * 256 + data[4];

	return 0;
}
