# ESP32 SensorNode

ESP32 SensorNode is an ESP32 based multi-purpose sensor for use in home automation systems.
It connects via Wi-Fi to your home automation system and allows you to receive the measured parameters such as temperature, humidity, brightness, and movement.

<img src="https://github.com/TheNico14/ESP32-SensorNode/blob/master/Documentation/Images/front.png?raw=true" width="250" height="250">
<img src="https://github.com/TheNico14/ESP32-SensorNode/blob/master/Documentation/Images/back.png?raw=true" width="250" height="250">

The board can be programmed over USB and it's compatible with both [ESPHome](https://esphome.io/) (please read [below](https://github.com/TheNico14/ESP32-SensorNode#temperature-accuracy)) for use with [Home Assistant](https://www.home-assistant.io/) and typical Arduino code with [MQTT](https://en.wikipedia.org/wiki/MQTT) messages that should be compatible with a lot of home automation systems.

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

At the time of writing, ESPHome allows you to configure wifi power saving but does not yet support light sleep, which would further reduce consumption and consequently unnecessary heat.

For this reason, I'm working on a simple software able to spend most of the time in deep sleep and turn on periodically to check the sensors and use the Wi-Fi only when it's necessary to send new data via MQTT.
This software is still under development and will be uploaded as soon as it is finished and tested.

## ESPHome firmware

This guide requires ESPHome already installed on your machine, more info on installation [here](https://esphome.io/index.html).

Installation:

* In the ESPHome interface click on the + to add a new device.
* Choose a name for the device and click on continue.
* Select Generic ESP32 (WROVER Module) from the list and click continue.
* Enter your Wi-Fi network details and a password for OTA and API, then click continue.
* Click on submit.
* After a refresh of the page, the newly created device should be visible. Click on edit.
* You can now complete the configuration as shown in the example [here](https://github.com/TheNico14/ESP32-SensorNode/blob/master/Code/ESPHome/ESPHome_config).

Once the configuration is complete, you can upload the firmware onto the board. There are multiple ways to do this:

* Compile and download the firmware by clicking on the 3 dots of the device in ESPHome and selecting compile. You will get a .bin file that can be uploaded via the web interface or via USB.
* If you connect the board to the PC containing ESPHome you should be able to select the board from the ESPHome web interface and click upload.

## Manual firmware upload

Each board is shipped with a basic version of ESPHome that allows you to load firmware once connected to the hotspot created by the board. Once connected to the ESP32 SensorNode network, you can connect via browser to its web interface on 192.168.4.1 and upload the .bin firmware.

Another possibility is to use the [esphome-flasher](https://github.com/esphome/esphome-flasher/releases) tool to upload the firmware via USB.

## Purchase an assembled unit

Since the production of this board requires relatively advanced soldering techniques and tools, upon request of some interested people, I opened a shop in Tindie where I sell already assembled boards.

[Purchase an ESP32 SensorNode on Tindie](https://www.tindie.com/products/thenico/esp32-sensornode/).
