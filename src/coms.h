#include "global.h"

// void fetchSerialCommands(){
// 	SerialDebug.println("Run");
// 	int index = 0;
// 	char c[1000];
// 	while (SerialDebug.available()) {
// 		SerialDebug.println("Index: " + index);
// 		c[index] = SerialDebug.read();
// 		index++;
// 		SerialDebug.println("Serial Read: " + c[index]);
// 	}
// 	String s = String(c);
// 	SerialDebug.println("String: " + s);
// 	if (s == "Mode: 1") {
// 		SerialDebug.println("Changing Mode");
// 		Mode = 1;
// 	}
// 	SerialDebug.println("Mode: " + Mode);
	
// }

// TODO: Reduce buffer size
DMAMEM char serialEncodeBuffer[10000] = {'\0'};

bool encodeCommands(char c){
	bool isValid = false;
	static int index = 0;
	switch (c) {
		case '$': // Use '$' to indicate the start of a message when sending commands through serial to the vessel
			break;
		case '&': // Use '&' to indicate the end of a message when sending commands through serial to the vessel
			serialEncodeBuffer[index] = '\0';
			index = 0;
			isValid = true;
			break;
		default:
			serialEncodeBuffer[index] = c;
			break;
	}
	index++;
	return isValid;
}

void fetchSerialCommands(){
	char c = '0'; // Set to '0' to prevent "may be used uninitialized" warning
	if (SerialDebug.available()) {
		c = SerialDebug.read();
	}
	if (c == '1') {
		SerialDebug.println("Changing Mode");
		Mode = 1;
	}
	
}