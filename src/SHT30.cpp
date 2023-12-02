#include "SHT30.h"



SHT30::SHT30(uint8_t address)
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


void SHT30::begin()
{
    Wire.begin();
}

float SHT30::temperature()
{
    float temp = temperatureReading;
    return temp * 175 / 65535 - 45;
}

float SHT30::fahrenheit()
{
    return temperature() * 1.8 + 32;
}

float SHT30::kelvin()
{
    return temperature() + 273.15;
}

float SHT30::humidity()
{
    float temp = humidityReading;
    return temp * 100 / 65535 - 45;
}

bool SHT30::checkCRC(uint8_t const *data, uint16_t length, uint8_t crc)
{
    return crcMethod.crc(data, 2) == crc;
}

uint8_t SHT30::read()
{
	uint8_t data[6];

	// Start I2C Transmission
	Wire.beginTransmission(address);
	// Send measurement command
	Wire.write(0x2C);
	Wire.write(0x06);
	// Stop I2C transmission
	if (Wire.endTransmission() != 0) return SHT3X_ERROR_WRITE;  

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
    temperatureReading = data[0] << 8 + data[1];
    humidityReading = data[3] << 8 + data[4];
    if (!checkCRC(data, 2, data[2])) return SHT3X_ERROR_CRC;
    if (!checkCRC(data + 3, 2, data[5])) return SHT3X_ERROR_CRC;

	return 0;
}
