/*
 * hts221.h
 *
 *  Created on: Dec 27, 2021
 *      Author: kutas
 */

#ifndef HTS221_H_
#define HTS221_H_

#include "main.h"
#include "i2c.h"


#define 	HTS221_DEVICE_ADDRESS_0				0xBE

#define 	HTS221_WHO_AM_I_VALUE				0xBC
#define 	HTS221_WHO_AM_I_ADDRESS				0x0F

#define 	HTS221_ADDRESS_CTRL1				0x20

#define 	HTS221_ADDRESS_TEMP_L				0x2A
#define 	HTS221_ADDRESS_HUMI_L				0x28

uint8_t hts221_init(void);
uint8_t hts221_read_byte(uint8_t reg_addr);
void hts221_write_byte(uint8_t reg_addr, uint8_t value);
int8_t hts221_get_temp();
int8_t hts221_get_humi();

#endif /* HTS221_H_ */
