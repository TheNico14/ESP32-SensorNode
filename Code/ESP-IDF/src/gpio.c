/**
 * @file     gpio.c
 * @author   Nicholas Polledri
 * @version  1.0
 * @date     13-09-2020
 * 
 * @brief    GPIO functions
 */

// Include libraries
#include <driver/rtc_io.h>

#include "configuration.h"

#include "gpio.h"
#include "wifi.h"
#include "mqtt.h"

// Imported variables
extern RTC_DATA_ATTR uint8_t rtc_pir;
extern RTC_DATA_ATTR uint8_t rtc_pir_pending;

// Private function declarations
static void gpio_handler(void *args);

// Functions

/**
 * @brief    GPIO setup
 * 
 */
void gpio_setup(void)
{
    rtc_gpio_deinit(PIR_GPIO); // Re-enable GPIO after deep sleep

    gpio_config_t io_conf;

    io_conf.intr_type = GPIO_INTR_ANYEDGE;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = 1ULL << PIR_GPIO;
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;

    gpio_config(&io_conf); // Configure GPIO

    gpio_install_isr_service(0);
    gpio_isr_handler_add(PIR_GPIO, gpio_handler, NULL);
}

/**
 * @brief    GPIO event handler
 * 
 * @param    args: Pointer to event arguments
 */
static void gpio_handler(void *args)
{
    rtc_pir = gpio_get_level((gpio_num_t)PIR_GPIO); // Save the PIR value into RTC memory
    rtc_pir_pending = 1;
}

/**
 * @brief    PIR handler function.
 *           This function saves the actual PIR value into RTC memory,
 *           requests the Wi-Fi connection to be established and sends
 *           the new PIR value via MQTT.
 * 
 * @return   esp_err_t status
 */
esp_err_t handle_pir(void)
{
    if (!rtc_pir_pending)
        rtc_pir = gpio_get_level((gpio_num_t)PIR_GPIO); // Save the PIR value into RTC memory

    ESP_ERROR_CHECK(wifi_setup());     // Turn on Wi-Fi
    esp_err_t ret = wifi_event_wait(); // Wait for Wi-Fi connection

    if (ret == ESP_OK)
    {
        ESP_ERROR_CHECK(mqtt_setup());                         // Setup MQTT
        ESP_ERROR_CHECK_WITHOUT_ABORT(mqtt_send_pir(rtc_pir)); // Send PIR value
        rtc_pir_pending = 0;
        return (mqtt_event_wait()); // Wait for MQTT ack
    }
    else
        return ret;
}
