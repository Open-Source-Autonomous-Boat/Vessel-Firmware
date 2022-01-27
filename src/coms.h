/**
 * @file coms.h
 * @author Micahel MacDonald (@Michael2MacDonald)
 * @brief Contains communication and remote command functions
 */

/** TODO:
 * - Add satellite comunications
 * - Add Lora comunications
 */

#include "global.h" // Contains global variables and definitions
#include "errors.h" // Contains error handling functions, variables, and definitions
#include "control.h" // Contains RunVISCommand()

#include <ASCIIPacket.h> // Our custom lib for sending packet messages over serial using ASCII control characters

// #define VIS_TOOLS_NUM_PARAMS 12 // Set the array size for command params to 12 (Default is 10) in VIS_Tools.h
#include <VIS_Tools.h> // Our Custom Lib

ASCIIPacket SerialPackets(&SerialDebug);

void processPacket() { // Reads and proccesses the incoming packets
	/** TODO: Create array size option (var/define) */
	String tmp[100]; // Temparary array for holding manipulating the strings

	VIS_ToArray(PacketText, ';', tmp, sizeof(tmp)); // Split into commands

	for (uint i = 0; i<sizeof(tmp) && tmp[i]!=""; i++) { // For every command
		Commands[i] = VIS_Decode(tmp[i]);
	}

	// processCommands(); // Run the commands in 'Commands'
	for (uint i=0; i<sizeof(Commands)&&Commands[i].instr!=NULL; i++) { // For each command
		RunVISCommand(Commands[i]); // Run the command
	}
}

void fetchSerialCommands(){
	// char c = '0'; // Set to '0' to prevent "may be used uninitialized" warning
	// if (SerialDebug.available()) {
	// 	c = SerialDebug.read();
	// }
	// if (c == '1') {
	// 	SerialDebug.println("Changing Mode");
	// 	Mode = 1;
	// }

	for (int i=0; i<SerialDebug.available(); i++) { // For each char in serial buffer
		SerialPackets.update(); // Run update() to fetch char from Stream
		// After each update() check if packet has been received
		if (SerialPackets.available()) {
			SerialPackets.get(&PacketHeader, &PacketText);
			/** TODO: Process message commands/data */
			processPacket();
		}
	}
}

// Receives and processes incoming communications
void ReceiveCommunications() {
	// Check Serial port
	fetchSerialCommands();
	// Check Lora coms
	/** TODO: Check Lora coms */
	// Check Sat coms
	/** TODO: Check Sat coms */
}