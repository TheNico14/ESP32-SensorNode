/**
 * @file     i2c.c
 * @author   Nicholas Polledri
 * @version  1.0
 * @date     13-09-2020
 * 
 * @brief    I2C functions
 */

// Include libraries
#include "driver/i2c.h"

#include "configuration.h"

#include "i2c.h"

// Global variables
i2c_port_t i2c_master_port = 0;

// Functions

/**
 * @brief    I2C setup
 * 
 * @return   esp_err_t status
 */
esp_err_t i2c_setup(void)
{
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = I2C_MASTER_SDA_IO;
    conf.sda_pullup_en = GPIO_PULLUP_DISABLE;
    conf.scl_io_num = I2C_MASTER_SCL_IO;
    conf.scl_pullup_en = GPIO_PULLUP_DISABLE;
    conf.master.clk_speed = I2C_MASTER_FREQ_HZ;

    i2c_master_port = I2C_MASTER_NUM;
    i2c_param_config(i2c_master_port, &conf); // I2C configuration

    return i2c_driver_install(i2c_master_port, conf.mode, 0, 0, 0); // I2C driver start
}
