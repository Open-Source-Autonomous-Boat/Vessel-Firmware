# Setting Up Project For VSCode

**Note: The terms 'VisualTeensy' and 'Teensyduino' are used throughout this guide. Be careful not to get them confused.*

### Prerequisites:

- You have installed the Arduino IDE from [here](https://www.arduino.cc/en/software).
- You have installed Teensyduino from [here](https://www.pjrc.com/teensy/td_download.html).
- You have installed VSCode and the VSCode [`c/c++`](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools) extension from the `Extensions` tab in VSCode.

***Important!** After a fresh Arduino IDE installation you need to start the Arduino IDE at least once and open the library manager `(Sketch->Include Libraries->Manage Libraries)`. This will generate `preferences.txt` and `library_index.json` which are required by VisualTeensy. (If you used the Arduino IDE before, you are fine without that step)*

### Setup VisualTeensy
Follow the next steps or check out these resources by the maker of VisualTeensy, [luni64](https://github.com/luni64/):

[Quickstart Guide](https://github.com/luni64/VisualTeensy/wiki/Quick-Start-Guide), 
[Quick Setup Video](https://www.youtube.com/watch?v=UyrZ7ogx67Q), And
[VisualTeensy WIKI](https://github.com/luni64/VisualTeensy/wiki)

- Download the [latest release](https://github.com/luni64/VisualTeensy/releases/latest) of the VisualTeensy binaries and unzip in any convenient location.
- Run `VisualTeensy.exe` from the folder you just unzipped. On first startup, VisualTeensy will ask you for an Arduino installation folder. If you provide this information the startup code will fill in the default settings accordingly. Typically you will find the Arduino installation folder at `C:\Program Files (x86)\Arduino` or `C:\Program Files\Arduino` on windows.
- Open the project folder using `File->Open` and select the settings you need (i.e. board USB type etc...). The only thing I changed was turning on debug support. *Note: If you enable debug support you will have to install the [`Cortex-Debug`](https://marketplace.visualstudio.com/items?itemName=marus25.cortex-debug) VSCode extension.*
- Press `File->Save` then `Start` to generate the project.
- You can now close VisualTeensy if you like.

*Note: If you would like to change board settings at any point you can edit `.vsteensy\vsteensy.json`.*

### Setup The Project Folder

#### Setting up the file structure
- If you did the VisualTeensy setup on an existing project folder with code in it then you will have to move the code files to the `src` folder. You will also want to delete the `main.cpp` file VisualTeensy creates in `src`.
- If you created a new project folder then you can start writing your code in the `src` folder.
- Make sure to change any `.ino` files to `.cpp` because it will not recognize Arduino `.ino` files.
- Create a new folder named `lib`. Here you can add any libraries you need for your project.

#### Setting up include paths:
- Press `F1` and type `C/C++: Edit Configurations (UI)` (You can click on it when it pops up below the text bar).
- In the `Include path` setting change `C:/Program Files (x86)/Arduino/hardware/teensy/avr/cores/teensy4` to `cores/teensy4`. This will tell the project to use the Teensy core that is in the project folder rather than the one in the Arduino installation folder.
- Now add `lib/**` to a new line in the `Include path` setting. This will tell your project to look there for libraries.

**Note: You might run into dependency errors for libraries like `Wire` that you never had to download before. to your project `lib` folder. Normally the Arduino IDE and Teensyduino include some libraries like `Wire` by default. Your project will not have them so you can copy them to your `lib` folder or add them to the `Include path` setting. For windows use `C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries` for Teensy libraries and `C:\Program Files (x86)\Arduino\hardware\arduino\avr\libraries` for Arduino libraries.*

### Compiling & Uploading Code

- Open the project folder with vsCode (usually VisualTeensy does this automatically for you after generating the project).
- Compile with `Terminal->Run Build Task->Build` from the menu or use the shortcut `CTRL+SHIFT+B`.
- To upload firmware to the Teensy board you can use one of the installed uploaders. They show in the `Terminal->Run Build Task` menu.