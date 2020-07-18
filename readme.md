# ESP32 SensorNode

The ESP32 SensorNode is a ESP32 based multi-purpose sensor for use in home automation systems.
It connects via Wi-Fi to your home automation system and allows you to receive the measured parameters such as temperature, humidity, brightness and movement.

<img src="https://github.com/TheNico14/ESP32-SensorNode/blob/master/Documentation/Images/front.png?raw=true" width="250" height="250">
<img src="https://github.com/TheNico14/ESP32-SensorNode/blob/master/Documentation/Images/back.png?raw=true" width="250" height="250">

The board can be programmed by USB and it's compatible with both [ESPHome](https://esphome.io/) for use with [Home Assistant](https://www.home-assistant.io/) and typical Arduino code with [MQTT](https://en.wikipedia.org/wiki/MQTT) messages that should be compatible with a lot of home automation systems.

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

|Item|Label                     |Manufacturer Part |Value|Qty|Link                                                                                                                                           |
|----|--------------------------|------------------|-----|---|-----------------------------------------------------------------------------------------------------------------------------------------------|
|1   |C1,C2,C6                  |CL10A226MQ8NRNC   |22uF |3  |[LCSC](https://lcsc.com/product-detail/Multilayer-Ceramic-Capacitors-MLCC-SMD-SMT_SAMSUNG_CL10A226MQ8NRNC_22uF-226-20-6-3V_C59461.html)        |
|2   |C8,C11,C13                |CL10A475KQ8NNND   |4.7uF|3  |[LCSC](https://lcsc.com/product-detail/Others_Samsung-Electro-Mechanics_CL10A475KQ8NNND_Samsung-Electro-Mechanics-CL10A475KQ8NNND_C307470.html)|
|3   |C3,C4,C5,C7,C9,C10,C12,C14|0603B104K500CT    |100nF|8  |[LCSC](https://lcsc.com/product-detail/Multilayer-Ceramic-Capacitors-MLCC-SMD-SMT_Walsin-Tech-Corp-0603B104K500CT_C314282.html)                |
|4   |D1                        |BAT760-7          |     |1  |[LCSC](https://lcsc.com/product-detail/Schottky-Barrier-Diodes-SBD_DIODES_BAT760-7_BAT760-7_C124187.html)                                      |
|5   |D2,D3,D4                  |LESD5D5.0CT1G     |     |3  |[LCSC](https://lcsc.com/product-detail/New-Arrivals_Leshan-Radio-LESD5D5-0CT1G_C383211.html)                                                   |
|6   |EN,BOOT                   |K2-1107SP-A3SW-01 |     |2  |[LCSC](https://lcsc.com/product-detail/Others_Korean-Hroparts-Elec-K2-1107SP-A3SW-01_C283538.html)                                             |
|7   |LED                       |WS2812B-3535      |     |1  |[LCSC](https://lcsc.com/product-detail/Light-Emitting-Diodes-LED_3535-RGBIntegrated-Light_C114583.html)                                        |
|8   |PIR                       |AS312             |     |1  |[LCSC](https://lcsc.com/product-detail/Infrared-Sensors_Senba-Sensing-Tech-AS312_C90465.html)                                                  |
|9   |POWER                     |ORH-G36G          |     |1  |[LCSC](https://lcsc.com/product-detail/Light-Emitting-Diodes-LED_0603-green_C205443.html)                                                      |
|10  |Q1,Q2                     |SS8050-G          |     |2  |[LCSC](https://lcsc.com/product-detail/Transistors-NPN-PNP_Changjiang-Electronics-Tech-CJ-SS8050-G_C164886.html)                               |
|11  |R9,R10,R12,R13            |WR06X472 JTL      |4.7K |4  |[LCSC](https://lcsc.com/product-detail/Chip-Resistor-Surface-Mount_Walsin-Tech-Corp-Walsin-Tech-Corp-4-7KR-472-5_C384219.html)                 |
|12  |R2,R3,R7,R8,R14           |0603WAF1002T5E    |10K  |5  |[LCSC](https://lcsc.com/product-detail/Chip-Resistor-Surface-Mount_Uniroyal-Elec-0603WAF1002T5E_C25804.html)                                   |
|13  |R1,R4,R5,R6,R11           |0603WAJ0102T5E    |1K   |5  |[LCSC](https://lcsc.com/product-detail/Chip-Resistor-Surface-Mount_Uniroyal-Elec-0603WAJ0102T5E_C25585.html)                                   |
|14  |U1                        |ESP32-WROOM-32D   |     |1  |[LCSC](https://lcsc.com/product-detail/WIFI-Modules_ESP-WROOM-32_C95209.html)                                                                  |
|15  |U2                        |HT7833            |     |1  |[LCSC](https://lcsc.com/product-detail/Low-Dropout-Regulators-LDO_HT7833_C50936.html)                                                          |
|16  |U3                        |CP2102N-A01-GQFN24|     |1  |[LCSC](https://lcsc.com/product-detail/USB_SILICON-LABS_CP2102N-A01-GQFN24_CP2102N-A01-GQFN24_C80225.html)                                     |
|17  |U4                        |SI7021            |     |1  |[LCSC](https://lcsc.com/product-detail/Temperature-Humidity-Sensors_SILICON-LABS_SI7021-A20-GM1R_SILICON-LABS-SI7021-A20-GM1R_C132603.html)    |
|19  |U5                        |BH1750FVI-TR      |     |1  |[LCSC](https://lcsc.com/product-detail/Sensors_ROHM_BH1750FVI-TR_BH1750FVI-TR_C78960.html)                                                     |
|20  |USB                       |U-F-M5WD-W-1      |     |1  |[LCSC](https://lcsc.com/product-detail/Micro-USB-Connectors_Korean-Hroparts-Elec-U-F-M5WD-W-1_C145779.html)                                    |
|21  |                          |PIR COVER         |     |1  |[Aliexpress](https://www.aliexpress.com/item/32776166287.html)                                                                                       |

## Purchase an assembled unit

Since the production of this board requires relatively advanced soldering techniques and tools, I am considering offering boards already assembled on Tindie. I'll provide updates as soon as I'm ready to do so.
