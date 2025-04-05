// Description: Implementation of the joystick controller functions.

#include "joystick_controller.h"
#include "i2c_module.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <stdint.h>
#include <stdbool.h>

#define I2CDRV_LINUX_BUS "/dev/i2c-1"
#define I2C_DEVICE_ADDRESS 0x48

#define REG_CONFIGURATION 0x01
#define REG_DATA 0x00

#define TLA2024_CHANNEL_CONF_0 0x83D2
#define TLA2024_CHANNEL_CONF_1 0x83C2

static int i2c_file_desc = 0;

void joystick_init(){

    i2c_file_desc = init_i2c_bus(I2CDRV_LINUX_BUS, I2C_DEVICE_ADDRESS);
}

uint16_t joystick_get_x(){

    write_i2c_reg16(i2c_file_desc, REG_CONFIGURATION, TLA2024_CHANNEL_CONF_0);

    usleep(1000);

    uint16_t raw_read = read_i2c_reg16(i2c_file_desc, REG_DATA);
    uint16_t value = ((raw_read & 0xFF) << 8) | ((raw_read & 0xFF00) >> 8);
    value = value >> 4;

    return value;

}


uint16_t joystick_get_y(){

    write_i2c_reg16(i2c_file_desc, REG_CONFIGURATION, TLA2024_CHANNEL_CONF_1);

    usleep(1000);

    uint16_t raw_read = read_i2c_reg16(i2c_file_desc, REG_DATA);
    uint16_t value = ((raw_read & 0xFF) << 8) | ((raw_read & 0xFF00) >> 8);
    value = value >> 4;

    return value;

}

J_DIRECTION joystick_get_direction(){

    uint16_t x_value = joystick_get_x();
    uint16_t y_value = joystick_get_y();


    if (x_value > 1200){
        return RIGHT;
    } else if (x_value < 450){
        return LEFT;
    } else if (y_value > 1200){
        return DOWN;
    } else if (y_value < 400){
        return UP;
    } else {
        return NO_MOVEMENT;
    }
}

void joystick_cleanup(){

    clean_up_i2c(i2c_file_desc);
}