/**
 * @file     typedefs.h
 * @author   Nicholas Polledri
 * @version  1.0
 * @date     13-09-2020
 * 
 * @brief    Typedefs needed by other files
 */

#ifndef TYPEDEFS_H
#define TYPEDEFS_H

typedef enum measurement_type
{
    int_t = 0,
    float_t,
} measurement_type;

typedef enum
{
    SI7021_RES_HIGH2 = 0, // Temp resolution 14 bit, RH 12 bit
    SI7021_RES_HIGH,      // Temp resolution 12 bit, RH 8 bit
    SI7021_RES_HIGH1,     // Temp resolution 13 bit, RH 10 bit
    SI7021_RES_LOW        // Temp resolution 11 bit, RH 11 bit

} si7021_resolution_t;

typedef enum
{
    BH1750_MODE_ONE_TIME = 0,
    BH1750_MODE_CONTINIOUS
} bh1750_mode_t;

typedef enum
{
    BH1750_RES_LOW = 0, // Resolution 4 lx, measurement time 16 ms
    BH1750_RES_HIGH,    // Resolution 1 lx, measurement time 120 ms
    BH1750_RES_HIGH2    // Resolution 0.5 lx, measurement time 120 ms
} bh1750_resolution_t;

#endif