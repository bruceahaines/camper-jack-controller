# camper-jack-controller
 
An ESP8266/ESP32 based IoT controller for electric HappiJac(TM) camper jacks.

Features of this project include:
- An IoT Wi-Fi access point/web server that you can connect to from any Wi-Fi device with a web browser.
- A web page written in HTML/JS that uses AJAX and Websockets to communicate with the ESP device.
- Bootstrap CSS framework
- A node.js server used for testing to speed up html development cycles
- Uses the SPIFFS file system to store web server files - no hard-coded HTML!
- no more .ino files! This project uses the PlatformIO IDE as an extension of VSCode and is an attempt to enable clean, well-organized code using Arduino libraries.

The mcu interfaces with the electric jacks via an existing HappiJac(TM) control board that I "reverse-engineered". I pulled out the existing Zilog mcu and wired 8 GPIO pins from the ESP dev board directly to an existing relay driver IC. Each jack is controlled via a dual relay that can reverse polarity to the jack motor.

Inspiration for this project came from a camper with a HappiJac(TM) system that was missing its remote control :)
