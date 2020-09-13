/**
 * @file     i2c.h
 * @author   Nicholas Polledri
 * @version  1.0
 * @date     13-09-2020
 * 
 * @brief    I2C functions
 */

#ifndef I2C_H
#define I2C_H

#include <stdint.h>
#include <esp_err.h>

esp_err_t i2c_setup(void);

#endif
