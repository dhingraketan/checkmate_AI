#ifndef _I2C_MODULE_H_
#define _I2C_MODULE_H_

#include <stdint.h>
#include <stdbool.h>

int init_i2c_bus(char* bus, int address);
void write_i2c_reg16(int i2c_file_desc, uint8_t reg_addr, uint16_t value);
uint16_t read_i2c_reg16(int i2c_file_desc, uint8_t reg_addr);
void clean_up_i2c(int i2c_file_desc);

#endif