/**
 * @file     wifi.h
 * @author   Nicholas Polledri
 * @version  1.0
 * @date     13-09-2020
 * 
 * @brief    Wi-Fi functions
 */

#ifndef WIFI_H
#define WIFI_H

#include <stdint.h>
#include <esp_err.h>

esp_err_t wifi_setup(void);
esp_err_t wifi_event_wait(void);

#endif