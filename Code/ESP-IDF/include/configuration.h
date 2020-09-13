/**
 * @file     configuration.h
 * @author   Nicholas Polledri
 * @version  1.0
 * @date     13-09-2020
 * 
 * @brief    Configuration file for ESP32 SensorNode
 */

#ifndef CONFIGURATION_H
#define CONFIGURATION_H

// GENERAL
#define SLEEP_INTERVAL_SEC 5             // Time between measurements [sec]
#define SENSOR_UPDATE_INTERVAL_MAX 300   // Force an update after determined time [sec]
#define LIGHT_UPDATE_THRESHOLD 2         // Update if light changes more than this [lux]
#define TEMPERATURE_UPDATE_THRESHOLD 0.2 // Update if temperature changes more than this [°C/°F]
#define HUMIDITY_UPDATE_THRESHOLD 2      // Update if humidity changes more than this [%]

#define TEMPERATURE_USE_FAHRENHEIT 0 // Enable Fahrenheit measurements

// WI-FI
#define WIFI_SSID "wifi"
#define WIFI_PASSWORD "password"
#define WIFI_MAXIMUM_RETRY 5

#define WIFI_STATIC_IP 0 // Enable static IP
#define WIFI_IP_ADDRESS "192.168.1.100"
#define WIFI_DEFAULT_GATEWAY "192.168.1.1"
#define WIFI_NETMASK "255.255.255.0"

// MQTT
#define MQTT_BROKER "mqtt://192.168.1.10" // Broker IP (mqtt://X.X.X.X)
#define MQTT_USERNAME "user"              // MQTT username
#define MQTT_PASSWORD "password"          // MQTT password
#define MQTT_PORT 1883                    // MQTT port

#define MQTT_NODE_NAME "ESP32-SensorNode" // ESP32 SensorNode name on MQTT
#define MQTT_ENABLE_DISCOVERY 1           // Enable automatic Home Assistant sensor discovery

#define MQTT_LIGHT_TOPIC "light"             // Light topic
#define MQTT_TEMPERATURE_TOPIC "temperature" // Temperature topic
#define MQTT_HUMIDITY_TOPIC "humidity"       // Humidity topic
#define MQTT_PIR_TOPIC "motion"              // Motion topic

/* ADVANCED CONFIGURATION - From here don't edit if not sure ******************/

// MQTT
#define MQTT_SENSOR_DISCOVERY_TOPIC "homeassistant/sensor"
#define MQTT_BINARY_SENSOR_DISCOVERY_TOPIC "homeassistant/binary_sensor"
#define MQTT_TOPIC_MAX_LEN 80
#define MQTT_CONFIGURATION_PAYLOAD_MAX_LEN 200
#define MQTT_MEASUREMENT_MAX_LEN 10

// TIMEOUTS
#define WIFI_SETUP_TIMEOUT_MS 500 // Wi-Fi connection timeout
#define MQTT_SEND_TIMEOUT_MS 200  // MQTT message timeout

// EVENT BITS
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT BIT1
#define MQTT_PUBLISHED_BIT BIT2
#define MQTT_ERROR_BIT BIT3

// I2C
#define I2C_MASTER_SCL_IO 22      // I2C clock pin
#define I2C_MASTER_SDA_IO 21      // I2C data pin
#define I2C_MASTER_NUM 0          // I2C port
#define I2C_MASTER_FREQ_HZ 400000 // I2C bus frequency

// BH1750 - Light sensor
#define BH1750_ADDR 0x23                   // Sensor address
#define BH1750_MODE BH1750_MODE_CONTINIOUS // Sensor mode
#define BH1750_RESOLUTION BH1750_RES_HIGH  // Sensor resolution

#define BH1750_OPCODE_HIGH 0x0 // I2C Operation Codes
#define BH1750_OPCODE_HIGH2 0x1
#define BH1750_OPCODE_LOW 0x3
#define BH1750_OPCODE_CONT 0x10
#define BH1750_OPCODE_OT 0x20
#define BH1750_OPCODE_POWER_DOWN 0x00
#define BH1750_OPCODE_POWER_ON 0x01
#define BH1750_OPCODE_MT_HI 0x40
#define BH1750_OPCODE_MT_LO 0x60

// SI7021 - Temperature and humidity sensor
#define SI7021_ADDR 0x40                   // Temperature and humidity sensor I2C address
#define SI7021_RESOLUTION SI7021_RES_HIGH2 // Temperature and humidity sensor resolution

#define SI7021_COMMAND_READ_RH 0xF5 // I2C Operation Codes
#define SI7021_COMMAND_READ_TEMP 0xF3
#define SI7021_COMMAND_READ_TEMP_AFTER_RH 0xE0
#define SI7021_REG_WRITE 0xE6
#define SI7021_REG_READ 0xE7
#define SI7021_RESET 0xFE

#define MEASUREMENT_WAIT 25 // Measurement delay

// AS312 - PIR
#define PIR_GPIO 13 // PIR GPIO

#endif