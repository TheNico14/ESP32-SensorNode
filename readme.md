# ESP32 SensorNode

ESP32 SensorNode is an ESP32 based multi-purpose sensor for use in home automation systems.
It connects via Wi-Fi to your home automation system and allows you to receive the measured parameters such as temperature, humidity, brightness, and movement.

<img src="https://github.com/TheNico14/ESP32-SensorNode/blob/master/Documentation/Images/front.png?raw=true" width="250" height="250">
<img src="https://github.com/TheNico14/ESP32-SensorNode/blob/master/Documentation/Images/back.png?raw=true" width="250" height="250">

The board can be programmed over USB and it's compatible with both [ESPHome](https://esphome.io/) for use with [Home Assistant](https://www.home-assistant.io/) and typical Arduino code with [MQTT](https://en.wikipedia.org/wiki/MQTT) messages that should be compatible with a lot of home automation systems.

The board dimensions are 5.08 x 4.57 cm (2 x 1.8 inches).

## Sensors

* Temperature / Humidity: SI7021\
    ± 0.4 °C (max), –10 to 85 °C\
    ± 3% RH (max), 0 – 80% RH
* Ambient Light: BH1750\
    1 - 65535 lx
* Motion sensor: AS312

## Features

* Programmable via USB
* Green status LED and digital RGB LED (WS2812B)
* Boot and Reset buttons

## Temperature accuracy

Due to the high consumption of the ESP32 with Wi-Fi enabled, the board tends to heat up over time and the measured temperature can increase by about 1°C.

The sensor has been isolated as much as possible from the rest of the system but it would be preferable to implement software that can limit consumption in order to obtain precise measurements.

<img src="https://github.com/TheNico14/ESP32-SensorNode/blob/master/Documentation/Images/front_thermal.png?raw=true" width="250" height="250">
<img src="https://github.com/TheNico14/ESP32-SensorNode/blob/master/Documentation/Images/back_thermal.png?raw=true" width="250" height="250">

At the time of writing, Esphome allows you to configure wifi power saving but does not yet support light sleep, which would further reduce consumption and consequently unnecessary heat.

For this reason, I'm working on a simple software able to spend most of the time in deep sleep and turn on periodically to check the sensors and use the Wi-Fi only when it's necessary to send new data via MQTT.
This software is still under development and will be uploaded as soon as it is finished and tested.

## Bill of Materials

|Item|Label                     |Manufacturer Part |Value|Qty|
|----|--------------------------|------------------|-----|---|
|1   |C1,C2,C6                  |CL10A226MQ8NRNC   |22uF |3  |
|2   |C11,C13                   |CL10A475KQ8NNND   |4.7uF|2  |
|3   |C3,C4,C5,C7,C9,C10,C12,C14|0603B104K500CT    |100nF|8  |
|4   |D1                        |BAT760-7          |     |1  |
|5   |D2,D3,D4                  |LESD5D5.0CT1G     |     |3  |
|6   |EN,BOOT                   |K2-1107SP-A3SW-01 |     |2  |
|7   |LED                       |WS2812B-3535      |     |1  |
|8   |PIR                       |AS312             |     |1  |
|9   |Q1,Q2                     |SS8050-G          |     |2  |
|10  |R1,R4,R5,R6,R11           |0603WAJ0102T5E    |1K   |5  |
|11  |R2,R3,R7,R8               |0603WAF1002T5E    |10K  |4  |
|12  |R9,R10,R12                |WR06X472 JTL      |4.7K |3  |
|13  |STATUS                    |ORH-G36G          |     |1  |
|14  |U1                        |ESP32-WROOM-32D   |     |1  |
|15  |U2                        |HT7833            |     |1  |
|16  |U3                        |CP2102N-A01-GQFN24|     |1  |
|17  |U4                        |SI7021            |     |1  |
|19  |U5                        |BH1750FVI-TR      |     |1  |
|20  |USB                       |U-F-M5WD-W-1      |     |1  |
|21  |                          |PIR COVER         |     |1  |

## Purchase an assembled unit

Since the production of this board requires relatively advanced soldering techniques and tools, I am considering offering boards already assembled on Tindie. I'll provide updates as soon as I'm ready to do so.
