/**
 * @file     gpio.h
 * @author   Nicholas Polledri
 * @version  1.0
 * @date     13-09-2020
 * 
 * @brief    GPIO functions
 */

#ifndef GPIO_H
#define GPIO_H

#include <stdint.h>
#include <esp_err.h>

void gpio_setup(void);
esp_err_t handle_pir(void);

#endif