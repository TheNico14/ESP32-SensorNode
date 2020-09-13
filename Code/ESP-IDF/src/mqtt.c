/**
 * @file     mqtt.c
 * @author   Nicholas Polledri
 * @version  1.0
 * @date     13-09-2020
 * 
 * @brief    
 */

// Include libraries
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "mqtt_client.h"

#include "configuration.h"

#include "mqtt.h"
#include "wifi.h"

// Global variables
esp_mqtt_client_handle_t client;
EventGroupHandle_t mqtt_event_group;
uint8_t mqtt_already_setup = 0;

char light_topic[MQTT_TOPIC_MAX_LEN];
char temperature_topic[MQTT_TOPIC_MAX_LEN];
char humidity_topic[MQTT_TOPIC_MAX_LEN];
char pir_topic[MQTT_TOPIC_MAX_LEN];

char light_configuration_topic[MQTT_TOPIC_MAX_LEN];
char temperature_configuration_topic[MQTT_TOPIC_MAX_LEN];
char humidity_configuration_topic[MQTT_TOPIC_MAX_LEN];
char pir_configuration_topic[MQTT_TOPIC_MAX_LEN];

char light_configuration_payload[MQTT_CONFIGURATION_PAYLOAD_MAX_LEN];
char temperature_configuration_payload[MQTT_CONFIGURATION_PAYLOAD_MAX_LEN];
char humidity_configuration_payload[MQTT_CONFIGURATION_PAYLOAD_MAX_LEN];
char pir_configuration_payload[MQTT_CONFIGURATION_PAYLOAD_MAX_LEN];

// Private function declarations
static void event_handler(void *args, esp_event_base_t event, int32_t event_id, void *event_data);

// Functions

/**
 * @brief    MQTT setup
 * 
 * @return   esp_err_t status
 */
esp_err_t mqtt_setup(void)
{
    if (mqtt_already_setup)
        return ESP_OK;

    mqtt_event_group = xEventGroupCreate(); // create MQTT event group

    snprintf(light_topic, MQTT_TOPIC_MAX_LEN, "%s/%s", MQTT_NODE_NAME, MQTT_LIGHT_TOPIC); // Create MQTT topic strings
    snprintf(temperature_topic, MQTT_TOPIC_MAX_LEN, "%s/%s", MQTT_NODE_NAME, MQTT_TEMPERATURE_TOPIC);
    snprintf(humidity_topic, MQTT_TOPIC_MAX_LEN, "%s/%s", MQTT_NODE_NAME, MQTT_HUMIDITY_TOPIC);
    snprintf(pir_topic, MQTT_TOPIC_MAX_LEN, "%s/%s", MQTT_NODE_NAME, MQTT_PIR_TOPIC);

#if MQTT_ENABLE_DISCOVERY

    snprintf(light_configuration_topic, MQTT_TOPIC_MAX_LEN, "%s/%s %s/config", MQTT_SENSOR_DISCOVERY_TOPIC, MQTT_NODE_NAME, MQTT_LIGHT_TOPIC);
    snprintf(temperature_configuration_topic, MQTT_TOPIC_MAX_LEN, "%s/%s %s/config", MQTT_SENSOR_DISCOVERY_TOPIC, MQTT_NODE_NAME, MQTT_TEMPERATURE_TOPIC);
    snprintf(humidity_configuration_topic, MQTT_TOPIC_MAX_LEN, "%s/%s %s/config", MQTT_SENSOR_DISCOVERY_TOPIC, MQTT_NODE_NAME, MQTT_HUMIDITY_TOPIC);
    snprintf(pir_configuration_topic, MQTT_TOPIC_MAX_LEN, "%s/%s %s/config", MQTT_BINARY_SENSOR_DISCOVERY_TOPIC, MQTT_NODE_NAME, MQTT_PIR_TOPIC);

    snprintf(light_configuration_payload, MQTT_CONFIGURATION_PAYLOAD_MAX_LEN, "{\"device_class\":\"illuminance\", \"name\": \"%s-light\", \"state_topic\": \"%s\", \"unit_of_measurement\": \"lx\"}", MQTT_NODE_NAME, light_topic);
#if TEMPERATURE_USE_FAHRENHEIT
    snprintf(temperature_configuration_payload, MQTT_CONFIGURATION_PAYLOAD_MAX_LEN, "{\"device_class\": \"temperature\", \"name\": \"%s-temperature\", \"state_topic\": \"%s\", \"unit_of_measurement\": \"°F\"}", MQTT_NODE_NAME, temperature_topic);
#else
    snprintf(temperature_configuration_payload, MQTT_CONFIGURATION_PAYLOAD_MAX_LEN, "{\"device_class\": \"temperature\", \"name\": \"%s-temperature\", \"state_topic\": \"%s\", \"unit_of_measurement\": \"°C\"}", MQTT_NODE_NAME, temperature_topic);
#endif
    snprintf(humidity_configuration_payload, MQTT_CONFIGURATION_PAYLOAD_MAX_LEN, "{\"device_class\": \"humidity\", \"name\": \"%s-humidity\", \"state_topic\": \"%s\", \"unit_of_measurement\": \"%%\"}", MQTT_NODE_NAME, humidity_topic);
    snprintf(pir_configuration_payload, MQTT_CONFIGURATION_PAYLOAD_MAX_LEN, "{\"device_class\": \"motion\", \"name\": \"%s-motion\", \"state_topic\": \"%s\"}", MQTT_NODE_NAME, pir_topic);

#endif

    esp_mqtt_client_config_t mqtt_cfg = {
        .uri = MQTT_BROKER,
        .username = MQTT_USERNAME,
        .password = MQTT_PASSWORD,
        .port = MQTT_PORT,
    };

    client = esp_mqtt_client_init(&mqtt_cfg); // Init MQTT client
    ESP_ERROR_CHECK(esp_mqtt_client_register_event(client,
                                                   ESP_EVENT_ANY_ID,
                                                   event_handler,
                                                   client)); // Register MQTT event handler
    mqtt_already_setup = 1;

    return esp_mqtt_client_start(client); // Start MQTT client
}

/**
 * @brief    MQTT event handler
 * 
 * @param    args: Pointer to event arguments
 * @param    event: Event
 * @param    event_id: Event id
 * @param    event_data: Pointer to event data
 */
static void event_handler(void *args, esp_event_base_t event, int32_t event_id, void *event_data)
{
    BaseType_t ret, higher_priority_task_woken = pdFALSE;

    if (event_id == MQTT_EVENT_PUBLISHED || event_id == MQTT_EVENT_ERROR)
    {
        if (event_id == MQTT_EVENT_PUBLISHED)
            ret = xEventGroupSetBitsFromISR(mqtt_event_group,
                                            MQTT_PUBLISHED_BIT,
                                            &higher_priority_task_woken); // Set event bits to signal MQTT published event

        else
            ret = xEventGroupSetBitsFromISR(mqtt_event_group,
                                            MQTT_ERROR_BIT,
                                            &higher_priority_task_woken); // Set event bits to signal MQTT error event

        if (ret == pdPASS)
            portYIELD_FROM_ISR(); // Request context switch
    }
}

/**
 * @brief    Wait for MQTT event
 * 
 * @return   esp_err_t status
 */
esp_err_t mqtt_event_wait(void)
{
    EventBits_t bits = xEventGroupWaitBits(mqtt_event_group,
                                           MQTT_PUBLISHED_BIT | MQTT_ERROR_BIT,
                                           pdTRUE,
                                           pdFALSE,
                                           MQTT_SEND_TIMEOUT_MS); // Wait for MQTT event bits

    // Received a MQTT ack
    if (bits & MQTT_PUBLISHED_BIT)
        return ESP_OK;

    // Error during publishing MQTT message
    else if (bits & MQTT_ERROR_BIT)
    {
        printf("Failed to send message\n");
        return ESP_FAIL;
    }

    // Timeout for publishing MQTT message
    else
    {
        printf("Timeout sending message\n");
        return ESP_ERR_TIMEOUT;
    }
}

/**
 * @brief    Send Home Assistant autodiscovery config
 * 
 * @return   esp_err_t 
 */
esp_err_t mqtt_send_autodiscovery(void)
{
    ESP_ERROR_CHECK(wifi_setup());     // Turn on Wi-Fi
    esp_err_t ret = wifi_event_wait(); // Wait for Wi-Fi connection

    if (ret == ESP_OK)
    {
        ESP_ERROR_CHECK(mqtt_setup()); // Setup MQTT

        if (esp_mqtt_client_publish(client, light_configuration_topic, light_configuration_payload, 0, 1, 0) != -1)
            ret = ESP_FAIL;
        if (mqtt_event_wait() != ESP_OK) // Wait for MQTT ack
            ret = ESP_FAIL;

        if (esp_mqtt_client_publish(client, temperature_configuration_topic, temperature_configuration_payload, 0, 1, 0) != -1)
            ret = ESP_FAIL;
        if (mqtt_event_wait() != ESP_OK) // Wait for MQTT ack
            ret = ESP_FAIL;

        if (esp_mqtt_client_publish(client, humidity_configuration_topic, humidity_configuration_payload, 0, 1, 0) != -1)
            ret = ESP_FAIL;
        if (mqtt_event_wait() != ESP_OK) // Wait for MQTT ack
            ret = ESP_FAIL;

        if (esp_mqtt_client_publish(client, pir_configuration_topic, pir_configuration_payload, 0, 1, 0) != -1)
            ret = ESP_FAIL;
        if (mqtt_event_wait() != ESP_OK) // Wait for MQTT ack
            ret = ESP_FAIL;

        return ret;
    }
    else
        return ret;
}

/**
 * @brief    Send light measurement
 * 
 * @param    light: Light value
 * @return   esp_err_t status
 */
esp_err_t mqtt_send_light(uint16_t light)
{
    char temp[MQTT_MEASUREMENT_MAX_LEN];
    snprintf(temp, MQTT_MEASUREMENT_MAX_LEN, "%hu", light);

    if (esp_mqtt_client_publish(client, light_topic, temp, 0, 1, 0) != -1)
        return ESP_OK;
    else
        return ESP_FAIL;
}

/**
 * @brief    Send temperature measurement
 * 
 * @param    temperature: Temperature value
 * @return   esp_err_t status
 */
esp_err_t mqtt_send_temperature(float temperature)
{
    char temp[MQTT_MEASUREMENT_MAX_LEN];
    snprintf(temp, MQTT_MEASUREMENT_MAX_LEN, "%.2f", temperature);

    if (esp_mqtt_client_publish(client, temperature_topic, temp, 0, 1, 0) != -1)
        return ESP_OK;
    else
        return ESP_FAIL;
}

/**
 * @brief    Send humidity measurement
 * 
 * @param    humidity: Humidity value
 * @return   esp_err_t status
 */
esp_err_t mqtt_send_humidity(float humidity)
{
    char temp[MQTT_MEASUREMENT_MAX_LEN];
    snprintf(temp, MQTT_MEASUREMENT_MAX_LEN, "%.2f", humidity);

    if (esp_mqtt_client_publish(client, humidity_topic, temp, 0, 1, 0) != -1)
        return ESP_OK;
    else
        return ESP_FAIL;
}

/**
 * @brief    Send pir value
 * 
 * @param    pir: Pir value
 * @return   esp_err_t status
 */
esp_err_t mqtt_send_pir(uint8_t pir)
{
    char temp[MQTT_MEASUREMENT_MAX_LEN];
    if (pir)
        snprintf(temp, MQTT_MEASUREMENT_MAX_LEN, "on");
    else
        snprintf(temp, MQTT_MEASUREMENT_MAX_LEN, "off");

    if (esp_mqtt_client_publish(client, pir_topic, temp, 0, 1, 0) != -1)
        return ESP_OK;
    else
        return ESP_FAIL;
}