#include "hal/port_extender.h"
#include "hal/i2c_module.h"
#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>


#define I2C_BUS "/dev/i2c-1"

// MCP23017 Addresses
#define MCP23017_1 0x20
#define MCP23017_2 0x21
#define MCP23017_3 0x22
#define MCP23017_4 0x23

// MCP23017 Registers
#define IODIRA   0x00  // I/O Direction Register for Port A
#define IODIRB   0x01  // I/O Direction Register for Port B
#define GPPUA    0x0C  // Pull-up Enable Register for Port A
#define GPPUB    0x0D  // Pull-up Enable Register for Port B
#define GPIOA    0x12  // Bank A
#define GPIOB    0x13  // Bank B

// MCP23017 Addresses Array
static int mcp_addresses[4] = { MCP23017_1, MCP23017_2, MCP23017_3, MCP23017_4 };
static bool is_initialized = false;

void portExtender_init_and_config() {
    if (is_initialized) {
        return;
    }

    for (int i = 0; i < 1; i++) {
        int i2c_fd = init_i2c_bus(I2C_BUS, mcp_addresses[i]);

        // Set all PORT A & PORT B pins as input
        write_i2c_reg16(i2c_fd, IODIRA, 0xFF);
        usleep(5000);

        write_i2c_reg16(i2c_fd, IODIRB, 0xFF);

        usleep(5000);
        // Enable pull-up resistors on all PORT A & PORT B
        write_i2c_reg16(i2c_fd, GPPUA, 0xFF);

        usleep(5000);
        write_i2c_reg16(i2c_fd, GPPUB, 0xFF);


        clean_up_i2c(i2c_fd);

    }

    is_initialized = true;
}

void portExtender_readAllPins(int portNumber, uint8_t portData[16]) {
    if (!is_initialized) {
        return;
    }

    if (portNumber < 1 || portNumber > 4) {
        return;
    }

    int i2c_fd = init_i2c_bus(I2C_BUS, mcp_addresses[portNumber - 1]);

    uint16_t portA = read_i2c_reg16(i2c_fd, GPIOA);
    uint16_t portB = read_i2c_reg16(i2c_fd, GPIOB);

    clean_up_i2c(i2c_fd);

    // Extract individual pin states (bit-wise) and store in portData array
    for (int i = 0; i < 8; i++) {
        portData[i] = (portA & (1 << i)) ? 0 : 1;
        portData[i + 8] = (portB & (1 << i)) ? 0 : 1;
    }
}

void portExtender_cleanup() {
    is_initialized = false;
}