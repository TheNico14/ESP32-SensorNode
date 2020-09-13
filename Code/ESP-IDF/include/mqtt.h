/**
 * @file     mqtt.h
 * @author   Nicholas Polledri
 * @version  1.0
 * @date     13-09-2020
 * 
 * @brief    MQTT functions
 */

#ifndef MQTT_H
#define MQTT_H

#include <stdint.h>
#include <esp_err.h>

esp_err_t mqtt_setup(void);
esp_err_t mqtt_event_wait(void);
esp_err_t mqtt_send_autodiscovery(void);
esp_err_t mqtt_send_light(uint16_t light);
esp_err_t mqtt_send_temperature(float temperature);
esp_err_t mqtt_send_humidity(float humidity);
esp_err_t mqtt_send_pir(uint8_t pir);

#endif
