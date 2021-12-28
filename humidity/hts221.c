/*
 * hts221.c
 *
 *  Created on: Dec 27, 2021
 *      Author: kutas
 */


#include "hts221.h"

uint8_t addres = HTS221_DEVICE_ADDRESS_0;

uint8_t hts221_read_byte(uint8_t reg_addr)
{
	uint8_t data = 0;
	return *(i2c_master_read(&data, 1, reg_addr, addres, 0));
}


void hts221_write_byte(uint8_t reg_addr, uint8_t value)
{
	i2c_master_write(value, reg_addr, addres, 0);
}


void hts221_readArray(uint8_t * data, uint8_t reg, uint8_t length)
{
	i2c_master_read(data, length, reg, addres, 1);
}


int8_t hts221_get_temp()
{
    uint8_t temp[2];
    hts221_readArray(temp, HTS221_ADDRESS_TEMP_L, 2);
    uint16_t tmp = ((temp[1] << 8) | temp[0]);
    int8_t temperature = (10*(tmp - 300))/(200) + 10;

    return temperature;
}

int8_t hts221_get_humi()
{
    uint8_t temp[2];

    hts221_readArray(temp, HTS221_ADDRESS_HUMI_L, 2);
    uint16_t tmp = ((temp[1] << 8) | temp[0]);
    int8_t humi = (20*(tmp - 4000))/(2000) + 20;
    return humi;
}


uint8_t hts221_init(void)
{

	uint8_t status = 1;

	LL_mDelay(100);

	uint8_t val = hts221_read_byte(HTS221_WHO_AM_I_ADDRESS);

	if(val == HTS221_WHO_AM_I_VALUE)
	{
		status = 1;
	}
	else			//if the device is not found on one address, try another one
	{
		status = 0;
		return status;
	}

	uint8_t ctrl1 = (8 << 4) | 1;
	hts221_write_byte(HTS221_ADDRESS_CTRL1, ctrl1);

	return status;
}
