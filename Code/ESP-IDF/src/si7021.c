/**
 * @file     si7021.c
 * @author   Nicholas Polledri
 * @version  1.0
 * @date     13-09-2020
 *
 * @brief    ESP-IDF driver for Si7021
 */

// Include libraries
#include <driver/i2c.h>

#include "configuration.h"

#include "si7021.h"

// Imported variables
extern i2c_port_t i2c_master_port;

// Private function declarations
esp_err_t si7021_send_command(uint8_t *command, size_t nbytes);
esp_err_t si7021_read_measurement(uint8_t measure_cmd, float *output, float (*fn)(uint16_t));
float si7021_code_to_rh_pct(const uint16_t code);
float si7021_code_to_celsius(const uint16_t code);
float si7021_code_to_fahrenheit(const uint16_t code);

// Functions

/**
 * @brief    Setup Si7021 parameters
 * 
 * @param    resolution: Sensor resolution
 * @return   esp_err_t status
 */
esp_err_t si7021_setup(si7021_resolution_t resolution)
{
    if (resolution < 0 || resolution > 3)
        return ESP_ERR_INVALID_ARG;
    return si7021_write_register(resolution << 4);
}

/**
 * @brief    Send command to Si7021
 * 
 * @param    command: Pointer to command to send
 * @param    nbytes: Number of bytes to send
 * @return   esp_err_t status
 */
esp_err_t si7021_send_command(uint8_t *command, size_t nbytes)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (SI7021_ADDR << 1) | I2C_MASTER_WRITE, I2C_MASTER_ACK);
    for (size_t i = 0; i < nbytes; i++)
        i2c_master_write_byte(cmd, command[i], I2C_MASTER_ACK);

    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(i2c_master_port, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);

    ESP_ERROR_CHECK(ret);

    return ret;
}

/**
 * @brief    Write register of Si7021
 * 
 * @param    settings: Settings byte to send
 * @return   esp_err_t status
 */
esp_err_t si7021_write_register(uint8_t settings)
{
    uint8_t data[2] = {SI7021_REG_WRITE, settings};
    return si7021_send_command(data, 2);
}

/**
 * @brief    Read register of Si7021
 * 
 * @param    output: Pointer to settings byte to read
 * @return   esp_err_t status
 */
esp_err_t si7021_read_register(uint8_t *output)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (SI7021_ADDR << 1) | I2C_MASTER_WRITE, I2C_MASTER_ACK);
    i2c_master_write_byte(cmd, SI7021_REG_READ, I2C_MASTER_ACK);
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (SI7021_ADDR << 1) | I2C_MASTER_READ, I2C_MASTER_ACK);
    i2c_master_read_byte(cmd, output, I2C_MASTER_NACK);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(i2c_master_port, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);

    ESP_ERROR_CHECK(ret);

    return ret;
}

/**
 * @brief    Temperature or humidity measurement reading
 * 
 * @param    measure_cmd: Measurement command
 * @param    output: Pointer to output variable
 * @param    fn: Function to convert measurement (temperature or humidity)
 * @return   esp_err_t status
 */
esp_err_t si7021_read_measurement(uint8_t measure_cmd, float *output, float (*fn)(uint16_t))
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (SI7021_ADDR << 1) | I2C_MASTER_WRITE, I2C_MASTER_ACK);
    i2c_master_write_byte(cmd, measure_cmd, I2C_MASTER_ACK);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(i2c_master_port, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);

    ESP_ERROR_CHECK(ret);

    vTaskDelay(MEASUREMENT_WAIT / portTICK_PERIOD_MS);

    uint8_t buf[2];
    uint16_t data;

    cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (SI7021_ADDR << 1) | I2C_MASTER_READ, I2C_MASTER_ACK);
    i2c_master_read(cmd, buf, 2, I2C_MASTER_LAST_NACK);
    i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(i2c_master_port, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);

    ESP_ERROR_CHECK(ret);

    data = buf[0] << 8 | buf[1];
    *output = fn(data);

    return ret;
}

/**
 * @brief    Temperature and humidity measurement
 * 
 * @param    temperature: Pointer to temperature variable
 * @param    humidity: Pointer to humidity variable
 * @return   esp_err_t status
 */
esp_err_t si7021_measure(float *temperature, float *humidity)
{
    si7021_read_humidity(humidity);
    return si7021_read_temperature_after_humidity(temperature);
}

/**
 * @brief    Temperature measurement
 * 
 * @param    temperature: Pointer to temperature variable
 * @return   esp_err_t status
 */
esp_err_t si7021_read_temperature(float *temperature)
{
#if TEMPERATURE_USE_FAHRENHEIT
    return si7021_read_measurement(SI7021_COMMAND_READ_TEMP, temperature, &si7021_code_to_fahrenheit);
#else
    return si7021_read_measurement(SI7021_COMMAND_READ_TEMP, temperature, &si7021_code_to_celsius);
#endif
}

/**
 * @brief    Temperature reading after humidity measurement
 * 
 * @param    temperature: Pointer to temperature variable
 * @return   esp_err_t status
 */
esp_err_t si7021_read_temperature_after_humidity(float *temperature)
{
    if (TEMPERATURE_USE_FAHRENHEIT)
        return si7021_read_measurement(SI7021_COMMAND_READ_TEMP_AFTER_RH, temperature, &si7021_code_to_fahrenheit);
    else
        return si7021_read_measurement(SI7021_COMMAND_READ_TEMP_AFTER_RH, temperature, &si7021_code_to_celsius);
}

/**
 * @brief    Humidity measurement
 * 
 * @param    humidity: Pointer to humidity variable
 * @return   esp_err_t status
 */
esp_err_t si7021_read_humidity(float *humidity)
{
    return si7021_read_measurement(SI7021_COMMAND_READ_TEMP, humidity, &si7021_code_to_rh_pct);
}

/**
 * @brief    Code conversion to percentual humidity
 * 
 * @param    code: raw humidity sensor reading
 * @return   float status
 */
float si7021_code_to_rh_pct(const uint16_t code)
{
    return (((125.0 * code) / 65536.0) - 6.0);
}

/**
 * @brief    Code conversion to celsius temperature
 * 
 * @param    code: raw temperature sensor reading
 * @return   float status
 */
float si7021_code_to_celsius(const uint16_t code)
{
    return (((175.72 * code) / 65536.0) - 46.85);
}

/**
 * @brief    Code conversion to fahrenheit temperature
 * 
 * @param    code: raw temperature sensor reading
 * @return   float status
 */
float si7021_code_to_fahrenheit(const uint16_t code)
{
    return (((175.72 * code) / 65536.0) - 46.85) * 1.8 + 32;
}