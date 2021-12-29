/*
 * lps25hb.c
 *
 *  Created on: Dec 22, 2021
 *      Author: kutas
 */
#include "lps25hb.h"

uint8_t address_press = LPS25HB_DEVICE_ADDRESS_0;

uint8_t lps25hb_read_byte(uint8_t reg_addr)
{
	uint8_t data = 0;
	return *(i2c_master_read(&data, 1, reg_addr, address_press, 0));
}


void lps25hb_write_byte(uint8_t reg_addr, uint8_t value)
{
	i2c_master_write(value, reg_addr, address_press, 0);
}


void lps25hb_readArray(uint8_t * data, uint8_t reg, uint8_t length)
{
	i2c_master_read(data, length, reg, address_press, 1);
}


float lps25hb_get_temp()
{
    uint8_t temp[2];
    lps25hb_readArray(temp, LPS25HB_ADDRESS_TEMP_L, 2);
    int16_t temperature = ((temp[1] << 8) | temp[0]);

    return (float)(42.5 + (temperature/480));
}

float lps25hb_get_press()
{
	uint8_t data[3];

	lps25hb_readArray(data, LPS25HB_ADDRESS_PRESS_XL, 3);

	uint32_t press = ((data[2] << 16) | (data[1] << 8) | data[0]);

	return (float)(press/4096);
}


uint8_t lps25hb_init(void)
{

	uint8_t status = 1;

	//LIS3MDL_ACC_ON;

	LL_mDelay(100);

	uint8_t val = lps25hb_read_byte(LPS25HB_WHO_AM_I_ADDRESS);

	if(val == LPS25HB_WHO_AM_I_VALUE)
	{
		status = 1;
	}
	else			//if the device is not found on one address, try another one
	{
		address_press = LPS25HB_DEVICE_ADDRESS_1;
		val = lps25hb_read_byte(LPS25HB_WHO_AM_I_ADDRESS);
		if(val == LPS25HB_WHO_AM_I_VALUE)
		{
			status = 1;
		}
		else
		{
			status = 0;
			return status;
		}
	}

	//acc device init

	uint8_t ctrl1 = 9 << 4;
	lps25hb_write_byte(LPS25HB_ADDRESS_CTRL1, ctrl1);

	return status;
}
