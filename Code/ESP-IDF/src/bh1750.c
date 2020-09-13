/**
 * @file     bh1750.c
 * @author   Nicholas Polledri
 * @version  1.0
 * @date     13-09-2020
 * 
 * @brief    ESP-IDF driver for BH1750
 */

// Include libraries
#include <driver/i2c.h>

#include "configuration.h"

#include "bh1750.h"

// Imported variables
extern i2c_port_t i2c_master_port;

// Private function declarations
esp_err_t bh1750_send_command(uint8_t opcode);

// Functions

/**
 * @brief    Setup BH1750 parameters
 * 
 * @param    mode: Measurement mode
 * @param    resolution: Resolution
 * @return   esp_err_t status
 */
esp_err_t bh1750_setup(bh1750_mode_t mode, bh1750_resolution_t resolution)
{
    uint8_t opcode;

    switch (mode)
    {
    case BH1750_MODE_ONE_TIME:
        opcode = BH1750_OPCODE_OT;
        break;

    default:
        opcode = BH1750_OPCODE_CONT;
        break;
    }

    switch (resolution)
    {
    case BH1750_RES_LOW:
        opcode |= BH1750_OPCODE_LOW;
        break;

    case BH1750_RES_HIGH:
        opcode |= BH1750_OPCODE_HIGH;
        break;

    default:
        opcode |= BH1750_OPCODE_HIGH2;
        break;
    }

    return bh1750_send_command(opcode);
}

/**
 * @brief    Send command to BH1750
 * 
 * @param    opcode: opcode to send
 * @return   esp_err_t
 */
esp_err_t bh1750_send_command(uint8_t opcode)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (BH1750_ADDR << 1) | I2C_MASTER_WRITE, I2C_MASTER_ACK);
    i2c_master_write_byte(cmd, opcode, I2C_MASTER_ACK);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(i2c_master_port, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);

    ESP_ERROR_CHECK(ret);

    return ret;
}

/**
 * @brief    Read light level from BH1750
 * 
 * @param    level: Pointer to light level variable
 * @return   esp_err_t 
 */
esp_err_t bh1750_read(uint16_t *level)
{
    uint8_t buf[2];
    uint16_t light;

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (BH1750_ADDR << 1) | I2C_MASTER_READ, I2C_MASTER_ACK);
    i2c_master_read(cmd, buf, 2, I2C_MASTER_ACK);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(i2c_master_port, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);

    ESP_ERROR_CHECK(ret);

    light = buf[0] << 8 | buf[1];
    *level = (light * 10) / 12;

    return ret;
}

/**
 * @brief    Send power down command to BH1750
 * 
 * @return   esp_err_t 
 */
esp_err_t bh1750_power_down()
{
    return bh1750_send_command(BH1750_OPCODE_POWER_DOWN);
}

/**
 * @brief    Send power on command to BH1750
 * 
 * @return   esp_err_t 
 */
esp_err_t bh1750_power_on()
{
    return bh1750_send_command(BH1750_OPCODE_POWER_ON);
}

/**
 * @brief    Set measurement sensitivity for BH1750
 * 
 * @param    time: Measurement sensitivity
 * @return   esp_err_t 
 */
esp_err_t bh1750_set_measurement_time(uint8_t time)
{
    bh1750_send_command(BH1750_OPCODE_MT_HI | (time >> 5));
    return bh1750_send_command(BH1750_OPCODE_MT_LO | (time & 0x1f));
}