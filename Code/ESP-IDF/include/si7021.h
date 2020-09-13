/**
 * @file     si7021.h
 * @author   Nicholas Polledri
 * @version  1.0
 * @date     13-09-2020
 * 
 * @brief    ESP-IDF driver for Si7021
 */

#ifndef SI7021_H
#define SI7021_H

#include <stdint.h>
#include <esp_err.h>
#include "typedefs.h"

esp_err_t si7021_setup(si7021_resolution_t resolution);
esp_err_t si7021_write_register(uint8_t settings);
esp_err_t si7021_read_register(uint8_t *output);
esp_err_t si7021_measure(float *temperature, float *humidity);
esp_err_t si7021_read_temperature(float *temperature);
esp_err_t si7021_read_temperature_after_humidity(float *temperature);
esp_err_t si7021_read_humidity(float *humidity);

#endif