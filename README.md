# OSAB Vessel Firmware

This is the firmware that runs on the main teensy 4.1 board.

### Table Of Contents

- [Dependencies](#dependencies)
- [Folder Structure](#folder-structure)
- [How To Upload](#how-to-upload)
	- [Using the Arduino IDE and Teensyduino](#using-the-arduino-ide-and-teensyduino)
	- [Using VSCode, VisualTeensy, and Teensyduino](#using-vscode-visualteensy-and-teensyduino)
- [Contributors](#contributors)

## Dependencies

**Note:** any library forked by [PaulStoffrege](https://github.com/PaulStoffregen) is required to be the forked version for Teensy compatibility reasons.

- [adafruit/Adafruit_LSM303DLHC](https://github.com/adafruit/Adafruit_LSM303DLHC) **Might be replaced by `adafruit/Adafruit_LSM303_Accel` and `adafruit/Adafruit_LSM303DLH_Mag`*
- [adafruit/Adafruit_Sensor](https://github.com/adafruit/Adafruit_Sensor)
- [bblanchon/ArduinoJson](https://github.com/bblanchon/ArduinoJson)
- [Open-Source-Autonomous-Boat/NavTools](https://github.com/Open-Source-Autonomous-Boat/NavTools)
- [PaulStoffregen/PWMServo](https://github.com/PaulStoffregen/PWMServo) **Included in Teensyduino*
- [adafruit/RTClib](https://github.com/adafruit/RTClib) **Will be replaced by `PaulStoffregen/Time` soon.*
- [PaulStoffregen/SdFat-beta](https://github.com/PaulStoffregen/SdFat-beta) **Note:** You have to rename the library folder to `SdFat`.
- [PaulStoffregen/SPI](https://github.com/PaulStoffregen/SPI) **Included in Teensyduino*
- [joysfera/arduino-tasker](https://github.com/joysfera/arduino-tasker)
- [mikalhart/TinyGPSPlus](https://github.com/mikalhart/TinyGPSPlus)
- [PaulStoffregen/Wire](https://github.com/PaulStoffregen/Wire) **Included in Teensyduino*

## Folder Structure

`/cores/teensy4/` Folder contains modified teensy4 core for increased Serial2 read buffer.

`/lib/` Folder contains all required libraries and their dependencies. 

`/src/` Folder contains the source code.

## How To Upload

#### Using the Arduino IDE and Teensyduino
1. Download & install the [Arduino IDE](https://www.arduino.cc/en/software).
2. Download & install [Teensyduino](https://www.pjrc.com/teensy/td_download.html).
3. Go to your arduino installation folder and open `hardware/teensy/avr/cores/teensy4/HardwareSerial2.cpp`. Find and change `#define SERIAL2_RX_BUFFER_SIZE 64` to `#define SERIAL2_RX_BUFFER_SIZE 256`.
4. Download and install the following required libraries that are not already included in the Arduino IDE or Teensyduino:
	- [mikalhart/TinyGPSPlus](https://github.com/mikalhart/TinyGPSPlus) **Can not be downloaded using the Arduino IDE library manager*
	- [Open-Source-Autonomous-Boat/NavTools](https://github.com/Open-Source-Autonomous-Boat/NavTools) **Can not be downloaded using the Arduino IDE library manager*
	- [adafruit/Adafruit_LSM303DLHC](https://github.com/adafruit/Adafruit_LSM303DLHC)
	- [adafruit/Adafruit_Sensor](https://github.com/adafruit/Adafruit_Sensor) (Adafruit_Unified_Sensor)
	- [bblanchon/ArduinoJson](https://github.com/bblanchon/ArduinoJson)
	- [adafruit/RTClib](https://github.com/adafruit/RTClib)
5. Download the code in the `src` folder of this repository and put it in a folder named `OSAB` or anything you want as long is it follows Arduino's file naming rules. Rename `Main.cpp` to what you named the folder and add the `.ino` file extension.
6. In the Arduino IDE, open the `OSAB` folder containing the code. Connect your Teensy 4.1 to your computer and select the correct port and board type in the Arduino IDE.
7. Upload the code to the Teensy 4.1 by selecting upload in the Arduino IDE.

### Using VSCode, VisualTeensy, and Teensyduino

Check out [`Setting Up This Project For VSCode`](/VSCode-Setup.md) for more information.

# Contributors
- [Michael2MacDonald](https://github.com/Michael2MacDonald)