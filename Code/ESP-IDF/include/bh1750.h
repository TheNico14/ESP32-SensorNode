/**
 * @file     bh1750.h
 * @author   Nicholas Polledri
 * @version  1.0
 * @date     13-09-2020
 * 
 * @brief    ESP-IDF driver for BH1750
 */

#ifndef BH1750_H
#define BH1750_H

#include <stdint.h>
#include <esp_err.h>
#include "typedefs.h"

esp_err_t bh1750_setup(bh1750_mode_t mode, bh1750_resolution_t resolution);
esp_err_t bh1750_read(uint16_t *level);
esp_err_t bh1750_power_down();
esp_err_t bh1750_power_on();
esp_err_t bh1750_set_measurement_time(uint8_t time);

#endif