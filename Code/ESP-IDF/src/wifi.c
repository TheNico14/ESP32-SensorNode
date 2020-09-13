/**
 * @file     wifi.c
 * @author   Nicholas Polledri
 * @version  1.0
 * @date     13-09-2020
 * 
 * @brief    
 */

// Include libraries
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "soc/rtc_cntl_reg.h"

#include "configuration.h"

#include "wifi.h"

// Global variables
EventGroupHandle_t wifi_event_group;
uint8_t retry_num = 0;
uint8_t wifi_already_setup = 0;

// Private function declarations
static void event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);
void wifi_init_sta(void);

// Functions

/**
 * @brief    Wi-Fi setup
 * 
 * @return   esp_err_t 
 */
esp_err_t wifi_setup(void)
{

    if (wifi_already_setup)
        return ESP_OK;

    WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); // Disable brownout detector

    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    wifi_init_sta();

    return ret;
}

/**
 * @brief    Wi-Fi event handler
 * 
 * @param    args: Pointer to event arguments
 * @param    event: Event
 * @param    event_id: Event id
 * @param    event_data: Pointer to event data
 */
static void event_handler(void *args, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    BaseType_t ret, higher_priority_task_woken = pdFALSE;

    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) // Station mode enabled event
        esp_wifi_connect();                                           // Connect to Wi-Fi

    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) // Disconnected event
    {
        if (retry_num < WIFI_MAXIMUM_RETRY) // If less than maximum connection attempts
        {
            esp_wifi_connect(); // Connect to Wi-Fi
            retry_num++;
        }
        else // If maximum connection attemps reached
        {
            ret = xEventGroupSetBitsFromISR(wifi_event_group,
                                            WIFI_FAIL_BIT,
                                            &higher_priority_task_woken); // Set event bits to signal Wi-Fi connection error
            if (ret == pdPASS)
                portYIELD_FROM_ISR(); // Request context switch
        }
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) // IP received event
    {
        retry_num = 0;
        ret = xEventGroupSetBitsFromISR(wifi_event_group,
                                        WIFI_CONNECTED_BIT,
                                        &higher_priority_task_woken); // Set event bits to signal Wi-Fi connection success
        if (ret == pdPASS)
            portYIELD_FROM_ISR(); // Request context switch
    }
}

/**
 * @brief    Wi-Fi init as station mode
 * 
 */
void wifi_init_sta(void)
{
    wifi_event_group = xEventGroupCreate(); // create Wi-Fi event group

    ESP_ERROR_CHECK(esp_event_loop_create_default()); // Create event loop

    tcpip_adapter_init(); // Init TCP-IP adapter

#if WIFI_STATIC_IP // If static IP enabled
    {
        tcpip_adapter_ip_info_t ip_info;

        tcpip_adapter_dhcpc_stop(TCPIP_ADAPTER_IF_STA); // Stop DHCP client

        ip4addr_aton(WIFI_IP_ADDRESS, &ip_info.ip); // Setup IP configuration
        ip4addr_aton(WIFI_DEFAULT_GATEWAY, &ip_info.gw);
        ip4addr_aton(WIFI_NETMASK, &ip_info.netmask);
        ESP_ERROR_CHECK(tcpip_adapter_set_ip_info(TCPIP_ADAPTER_IF_STA, &ip_info)); // Configure IP configuration
    }
#endif

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg)); // Wi-Fi init

    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID,
                                               &event_handler, NULL)); // Register Wi-Fi event handler
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP,
                                               &event_handler, NULL)); // Register IP event handler

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASSWORD,
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,
        },
    }; // Setup Wi-Fi config

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));                   // Set Wi-Fi station mode
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config)); // Configure Wi-Fi configuration
    wifi_already_setup = 1;

    ESP_ERROR_CHECK(esp_wifi_start()); // Start Wi-Fi
}

/**
 * @brief    Wait for Wi-Fi event
 * 
 * @return   esp_err_t status
 */
esp_err_t wifi_event_wait(void)
{
    EventBits_t bits = xEventGroupWaitBits(wifi_event_group,
                                           WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
                                           pdFALSE,
                                           pdFALSE,
                                           WIFI_SETUP_TIMEOUT_MS); // Wait for Wi-Fi event bits

    // Connected to Wi-Fi
    if (bits & WIFI_CONNECTED_BIT)
        return ESP_OK;

    // Error during Wi-Fi connection
    else if (bits & WIFI_FAIL_BIT)
    {
        printf("Failed to connect to AP %s with password: %s\n", WIFI_SSID, WIFI_PASSWORD);
        return ESP_FAIL;
    }

    // Timeout for Wi-Fi connection
    else
    {
        printf("Timeout connecting to AP %s\n", WIFI_SSID);
        return ESP_ERR_TIMEOUT;
    }
}