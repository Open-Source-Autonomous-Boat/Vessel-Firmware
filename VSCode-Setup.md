# Setting Up This Project For VSCode

**Note:** The terms 'VisualTeensy' and 'Teensyduino' are used throughout this guide. Be careful not to get them confused.

**Note:** This guide is only for Windows 10

**Note:** This is a guide for setting up VSCode with Teensyduino and VisualTeensy. This is just an example of one way to use VSCode for teensy development and it might not be right for everyone.*

In this guide we will install VSCode, the Arduino IDE, and Teensyduino. Then we will setup VSCode and our project folder. Then we will configure and install VisualTeensy into our project folder. We will have to manually alter the VisualTeensy install to get it to work as we need.

#### Table Of Contents

- [Getting Started](#getting-started)
- [Setting Up VSCode And The Project Folder](#setting-up-vscode-and-the-project-folder)
- [Setting Up VisualTeensy](#setting-up-visualteensy)
- [Altering The VisualTeensy Install](#altering-the-visualteensy-install)
- [Compiling And Uploading Code](#compiling-and-uploading-code)


### Getting Started

1. Download and install [VSCode](https://code.visualstudio.com/download).
2. Download and install the [Arduino IDE](https://www.arduino.cc/en/software).
3. Download and install [Teensyduino](https://www.pjrc.com/teensy/td_download.html).

**Note:** After a fresh Arduino installation you need to start the Arduino IDE at least once and open the library manager `(Sketch->Include Libraries->Manage Libraries)`. This will generate `preferences.txt` and `library_index.json` which are required by VisualTeensy. If you have opened the IDE and library manager before, you can skip this step.

### Setting Up VSCode And The Project Folder

1. Using the `Extentions` tab found on the left hand menu bar of VSCode, find and install the [C/C++ extension](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools).
2. Clone this repository into a location of your choice or create a new project folder that includes a `src` folder for code and a `lib` folder for libraries.
	- Add your code and libraries to their respective folders if you created a new folder (This can be done later).

### Setting Up VisualTeensy

If you need help you can check out the VisualTeensy [Quickstart Guide](https://github.com/luni64/VisualTeensy/wiki/Quick-Start-Guide) or the [VisualTeensy WIKI](https://github.com/luni64/VisualTeensy/wiki).

1. Download the [latest release](https://github.com/luni64/VisualTeensy/releases/latest) of VisualTeensy and unzip it in any convenient location.
2. Run `VisualTeensy.exe` from the folder you just unzipped.
	- On first startup, VisualTeensy will ask you for an Arduino installation folder. Fill this in so VisualTeensy will fill in the default settings automatically. The Arduino installation folder should be located at `C:\Program Files (x86)\Arduino` on windows.
3. Open the project folder in VisualTeensy using `File->Open`
4. Set the options.
	- In the first tab (`project`), select `Board Type: Teensy 4.1`. The other options should be already set.
	- Select the `Expert Setup` option. Make sure that the `Teensyduino core` option is set to `Copy local core into project` and that the path is `C:\Program Files (x86)\Arduino\hardware\teensy\avr` or something similar.
	- Select the `Settings` tab. Make sure that the only path set under the `Uploaders` section is the `PJRC Uploader`. It should be set to something like this `C:\Program Files (x86)\Arduino\hardware\tools`.
	- Due to how VisualTeensy handles `installed, shared Libraries` we will be manually setting up the libraries.
4. Press `File->Save` then `Start` to generate the project.
	- VSCode will automatically open with the project folder when VisualTeensy is done.
5. You can now close VisualTeensy if you like.

### Altering The VisualTeensy Install

1. Press `F1` and type `C/C++: Edit Configurations (UI)` (You can click on it when it pops up below the text bar).
	- In the `Include path` setting change `C:/Program Files (x86)/Arduino/hardware/teensy/avr/cores/teensy4` to `cores/teensy4`. This will tell VSCode to look for the Teensy core that is in the project folder rather than the one in the Arduino installation folder.
	- Now add `lib/**` to a new line in the `Include path` setting. This will tell your project to look there for libraries.
	- **Note:** The `Include path` setting does not affect anything to do with what code/libraries are used when compiling. The `makefile` determines that. `Include path` just tells VSCode what folders/code to look at when checking for errors in your code.
2. Open the `makefile` file and list all of your libraries after `LIBS_LOCAL :=`.
	- Make sure to use the name of the library's folder, not the name of the library (`TinyGPSPlus`, not `TinyGPS++`).
	- Separate each library with spaces.
	- It should look something like this: `LIBS_LOCAL := Adafruit_LSM303DLHC Adafruit_Unified_Sensor ArduinoJson NavTools PWMServo RTClib SdFat SPI TinyGPSPlus Wire`.
3. Open `.vsteensy/vsteensy.json` and add the same list of libraries to `projectLibraries` that you added to the `makefile`.
	- It should look something like this: `"projectLibraries": ["Adafruit_LSM303DLHC","Adafruit_Unified_Sensor","ArduinoJson","NavTools","PWMServo","RTClib","SdFat","SPI","TinyGPSPlus","Wire"]`.

**Note:** You might run into dependency errors for libraries like `Wire`. Normally the Arduino IDE and Teensyduino include some libraries like `Wire` by default. Your project will not have them so you can copy them to your `lib` folder and add them to the `makefile`, and `vsteensy.json` files.

### Compiling And Uploading Code

1. Open the project folder with VSCode (usually VisualTeensy does this automatically for you after generating the project).
2. Compile with `Terminal->Run Build Task->Build` from the menu or use the shortcut `CTRL+SHIFT+B` and selecting `Build`.
3. To upload firmware to the Teensy board you can use one of the installed uploaders. They show up in the `Terminal->Run Build Task` menu.