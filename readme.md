# ESP32 SensorNode

ESP32 SensorNode is an ESP32 based multi-purpose sensor for use in home automation systems.
It connects via Wi-Fi to your home automation system and allows you to receive the measured parameters such as temperature, humidity, brightness, and movement.

<img src="https://github.com/TheNico14/ESP32-SensorNode/blob/master/Documentation/Images/front.png?raw=true" width="250" height="250">
<img src="https://github.com/TheNico14/ESP32-SensorNode/blob/master/Documentation/Images/back.png?raw=true" width="250" height="250">

The board can be programmed over USB and it's compatible with both [ESPHome](https://esphome.io/) (please read [below](https://github.com/TheNico14/ESP32-SensorNode#temperature-accuracy)) and the custom [ESP-IDF](https://github.com/espressif/esp-idf) software for use with [Home Assistant](https://www.home-assistant.io/). The custom ESP-IDF software utilizes [MQTT](https://en.wikipedia.org/wiki/MQTT) and it  should be compatible with other home automation systems.

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

At the time of writing, ESPHome allows you to configure wifi power saving but does not yet support light sleep, which would further reduce consumption and consequently unnecessary heat. It should be possible to use deep sleep with ESPHome but it has not been extensively tested.

An alternative custom software based on ESP-IDF has been developed for this board to minimize the problems caused by the high power consumption of the ESP32 microcontroller and its influence on measurements.
This software uses deep sleep and parsimonious use of Wi-Fi to minimize power consumption and the resulting increase of the board's temperature.

## Installation

The software installation guide can be found in the [project wiki](https://github.com/TheNico14/ESP32-SensorNode/wiki).

## Purchase an assembled unit

Since the production of this board requires relatively advanced soldering techniques and tools, upon request of some interested people, I opened a shop in Tindie where I sell already assembled boards.

[Purchase an ESP32 SensorNode on Tindie](https://www.tindie.com/products/thenico/esp32-sensornode/).
