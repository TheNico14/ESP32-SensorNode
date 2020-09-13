/**
 * @file     main.c
 * @author   Nicholas Polledri
 * @version  1.0
 * @date     13-09-2020
 * 
 * @brief    Main code of ESP32 SensorNode
 */

// Include libraries
#include <stdlib.h>
#include <stdint.h>
#include <esp_err.h>
#include <sys/time.h>
#include <esp_task_wdt.h>
#include <esp_sleep.h>
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

#include "typedefs.h"
#include "configuration.h"

#include "gpio.h"
#include "i2c.h"
#include "bh1750.h"
#include "si7021.h"
#include "wifi.h"
#include "mqtt.h"

// Global variables
struct timeval timestamp;

// RTC variables
RTC_DATA_ATTR float rtc_temperature;
RTC_DATA_ATTR float rtc_humidity;
RTC_DATA_ATTR uint16_t rtc_light;
RTC_DATA_ATTR uint8_t rtc_pir;
RTC_DATA_ATTR uint8_t rtc_pir_pending;
RTC_DATA_ATTR struct timeval rtc_temperature_timestamp;
RTC_DATA_ATTR struct timeval rtc_humidity_timestamp;
RTC_DATA_ATTR struct timeval rtc_light_timestamp;
RTC_DATA_ATTR uint8_t rtc_temperature_valid;
RTC_DATA_ATTR uint8_t rtc_humidity_valid;
RTC_DATA_ATTR uint8_t rtc_light_valid;

// Private function declarations
esp_err_t setup(void);
esp_err_t check_measurements(void);
uint8_t handle_measurement(measurement_type type, void *measurement, uint8_t *rtc_measurement_valid, void *rtc_measurement, float update_threshold, struct timeval timestamp, struct timeval *rtc_timestamp);
void start_deep_sleep(void);

// Functions

/**
 * @brief    Main function
 * 
 */
void app_main(void)
{
    gettimeofday(&timestamp, NULL); // Get current timestamp

    esp_sleep_wakeup_cause_t wakeup_cause = esp_sleep_get_wakeup_cause();

    switch (wakeup_cause)
    {

    // Actions to execute after a routine wakeup
    case ESP_SLEEP_WAKEUP_TIMER:
        i2c_setup();
        check_measurements();
        break;

    // Actions to execute after a PIR interrupt wakeup
    case ESP_SLEEP_WAKEUP_EXT0:
        gpio_setup();
        handle_pir();
        break;

    // Actions to execute after every other wakeup cause
    default:
        setup();
        mqtt_send_autodiscovery();
        break;
    }

    if (rtc_pir_pending)
        handle_pir();

    start_deep_sleep();
}

/**
 * @brief    Sensors setup needed to initialize sensors after first boot
 * 
 * @return   esp_err_t status
 */
esp_err_t setup(void)
{
    rtc_light_valid = 0;
    rtc_temperature_valid = 0;
    rtc_humidity_valid = 0;

    ESP_ERROR_CHECK(i2c_setup());

    bh1750_mode_t bh1750_mode = BH1750_MODE;
    bh1750_resolution_t bh1750_resolution = BH1750_RESOLUTION;
    ESP_ERROR_CHECK(bh1750_setup(bh1750_mode, bh1750_resolution));

    si7021_resolution_t si7021_resolution = SI7021_RESOLUTION;
    ESP_ERROR_CHECK(si7021_setup(si7021_resolution));

    return ESP_OK;
}

/**
 * @brief    Sensor reading and measurements handler function.
 *           This function reads the value of the light, temperature and humidity
 *           measurements, then checks if the new measurements need to be sent
 *           and if so, requests the Wi-Fi connection to be established and sends
 *           the required measurements via MQTT.
 * 
 * @return   esp_err_t status
 */
esp_err_t check_measurements(void)
{
    uint8_t light_needs_update = 0,
            temperature_needs_update = 0,
            humidity_needs_update = 0;

    // BH1750 measurement
    uint16_t light;
    ESP_ERROR_CHECK(bh1750_read(&light)); // Sensor reading
    light_needs_update = handle_measurement(int_t,
                                            &light,
                                            &rtc_light_valid,
                                            &rtc_light,
                                            LIGHT_UPDATE_THRESHOLD,
                                            timestamp,
                                            &rtc_light_timestamp);

    // Si7021 measurement
    float temperature, humidity;
    ESP_ERROR_CHECK(si7021_measure(&temperature, &humidity)); // Sensor reading
    temperature_needs_update = handle_measurement(float_t,
                                                  &temperature,
                                                  &rtc_temperature_valid,
                                                  &rtc_temperature,
                                                  TEMPERATURE_UPDATE_THRESHOLD,
                                                  timestamp,
                                                  &rtc_temperature_timestamp);

    humidity_needs_update = handle_measurement(float_t,
                                               &humidity,
                                               &rtc_humidity_valid,
                                               &rtc_humidity,
                                               HUMIDITY_UPDATE_THRESHOLD,
                                               timestamp,
                                               &rtc_humidity_timestamp);

    // Check if an update is needed
    if (light_needs_update || temperature_needs_update || humidity_needs_update)
    {
        ESP_ERROR_CHECK(wifi_setup());     // Turn on Wi-Fi
        esp_err_t ret = wifi_event_wait(); // Wait for Wi-Fi connection

        if (ret == ESP_OK) // If Wi-Fi connection is established
        {
            ESP_ERROR_CHECK(mqtt_setup()); // Setup MQTT

            // Light update check
            if (light_needs_update)
            {
                ESP_ERROR_CHECK_WITHOUT_ABORT(mqtt_send_light(light)); // Send light value
                if (mqtt_event_wait() != ESP_OK)                       // Wait for MQTT ack
                    ret = ESP_FAIL;
            }

            // Temperature update check
            if (temperature_needs_update)
            {
                ESP_ERROR_CHECK_WITHOUT_ABORT(mqtt_send_temperature(temperature)); // Send temperature value
                if (mqtt_event_wait() != ESP_OK)                                   // Wait for MQTT ack
                    ret = ESP_FAIL;
            }

            // Humidity update check
            if (humidity_needs_update)
            {
                ESP_ERROR_CHECK_WITHOUT_ABORT(mqtt_send_humidity(humidity)); // Send humidity value
                if (mqtt_event_wait() != ESP_OK)                             // Wait for MQTT ack
                    ret = ESP_FAIL;
            }
        }
        return ret;
    }
    return ESP_OK;
}

/**
 * @brief    Checks if new measurement needs to be sent.
 * 
 * @param    type: int_t or float_t 
 * @param    measurement: pointer to new measurement
 * @param    rtc_measurement_valid: pointer to valid rtc measurement flag
 * @param    rtc_measurement: pointer to rtc measurement
 * @param    update_threshold: threshold to cross for sending a measurement
 * @param    timestamp: actual timestamp
 * @param    rtc_timestamp: pointer to rtc measurement timestamp
 * @return   uint8_t: 0-1 needs update, -1 error
 */
uint8_t handle_measurement(measurement_type type, void *measurement, uint8_t *rtc_measurement_valid, void *rtc_measurement, float update_threshold, struct timeval timestamp, struct timeval *rtc_timestamp)
{
    float delta, value, rtc_value;

    if (type == int_t) // If measurement type is integer
    {
        value = *((int16_t *)measurement); // Cast values with integer type
        rtc_value = *((int16_t *)rtc_measurement);
    }
    else if (type == float_t) // If measurement type is float
    {
        value = *((float *)measurement); // Cast values with float type
        rtc_value = *((float *)rtc_measurement);
    }
    else           // If measurement type is not correct
        return -1; // Return error value

    if (*rtc_measurement_valid == 1) // If RTC measurement is valid
    {
        delta = abs(value - rtc_value);

        if (delta > update_threshold || (timestamp.tv_sec - rtc_timestamp->tv_sec) >= SENSOR_UPDATE_INTERVAL_MAX) // If delta is bigger than thresold or last value is expired
        {
            if (type == int_t)
                *((int16_t *)rtc_measurement) = *((int16_t *)measurement); // Save measurement as integer
            else if (type == float_t)
                *((float *)rtc_measurement) = *((float *)measurement); // Save measurement as float

            *rtc_measurement_valid = 1;
            *rtc_timestamp = timestamp;
            return 1;
        }
        return 0;
    }
    else // If RTC measurement is not valid
    {
        if (type == int_t)
            *((int16_t *)rtc_measurement) = *((int16_t *)measurement); // Save measurement as integer
        else if (type == float_t)
            *((float *)rtc_measurement) = *((float *)measurement); // Save measurement as float

        *rtc_measurement_valid = 1;

        return 1;
    }
}

/**
 * @brief    Deep sleep preparation and activation
 * 
 */
void start_deep_sleep(void)
{
    esp_sleep_enable_timer_wakeup(SLEEP_INTERVAL_SEC * 1000000);  // Enable wakeup after time interval
    esp_sleep_enable_ext0_wakeup((gpio_num_t)PIR_GPIO, !rtc_pir); // Enable wakeup after PIR interrupt

    fflush(stdout); // Empty the stdout stream

    esp_deep_sleep_start(); // Start deep sleep
}