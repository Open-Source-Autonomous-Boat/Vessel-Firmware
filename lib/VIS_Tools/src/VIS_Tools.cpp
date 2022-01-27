#include "VIS_Tools.h" // Inlcude library header file
#include <Arduino.h>

// void VIS_StrToStrArray(String input, char delimiter, String* array, int arraySize) { // Splits a string into sub-strings using a delimiter
// 	int subStrIndex[] = {0, -1}; // index of previous delimiter, index of current delimiter
// 	int strLength = input.length()-1;
// 	int arrayIndex = 0;
// 	for (int i=0; i<=strLength && (arraySize-1)>=arrayIndex; i++) { // Loop through string
// 		if (input.charAt(i)==delimiter || i==strLength) { // Check if a delimiter is found
// 			subStrIndex[0] = subStrIndex[1]+1; // Position of previous delimiter
// 			// delimiters[1] = (i == strLength) ? i+1 : i; // Position of this delimiter (i+1 because substring ending index is exclusive)
// 			subStrIndex[1] = i; // Position of this delimiter (Do not include delimiter in substring)
// 			array[arrayIndex++] = input.substring(subStrIndex[0], subStrIndex[1]); // Save contents between the delimiters to substring
// 		}
// 	}
// 	return;
// }

// Splits a string into sub-strings using a delimiter; Ignores delimiters between quotes (Escaped)
void VIS_ToArray(String input, char delimiter, String* array, int arraySize) {
	int subStrIndex[] = {0, -1}; // index of previous delimiter, index of current delimiter
	int strLength = input.length()-1;
	int arrayIndex = 0;
	for (int i=0; i<=strLength && (arraySize-1)>=arrayIndex; i++) { // Loop through string
		bool escaped = false;
		if (input.charAt(i)=='"') {
			escaped = !escaped;
		} else if (!escaped) { // If not escaped then just act normal
			if (input.charAt(i)==delimiter || i==strLength) { // Check if a delimiter is found
				subStrIndex[0] = subStrIndex[1]+1; // Position of previous delimiter
				// delimiters[1] = (i == strLength) ? i+1 : i; // Position of this delimiter (i+1 because substring ending index is exclusive)
				subStrIndex[1] = i; // Position of this delimiter (Do not include delimiter in substring)
				array[arrayIndex] = input.substring(subStrIndex[0], subStrIndex[1]); // Save contents between the delimiters to substring
				arrayIndex++;
			}
		} else { // If escaped then ignore delimiters
			if (i==strLength) { // Check if a delimiter is found
				subStrIndex[0] = subStrIndex[1]+1; // Position of previous delimiter
				// delimiters[1] = (i == strLength) ? i+1 : i; // Position of this delimiter (i+1 because substring ending index is exclusive)
				subStrIndex[1] = i; // Position of this delimiter (Do not include delimiter in substring)
				array[arrayIndex] = input.substring(subStrIndex[0], subStrIndex[1]); // Save contents between the delimiters to substring
				arrayIndex++;
			}
		}
		
	}
	return;
}

VISCommand VIS_Decode(String cmd) {
	VISCommand result; // Holds our result
	// Copy the instuction (ex: W1) from the string 'cmd' to the 'result.instr' struct string variable
	result.instr = cmd.substring( 0, cmd.indexOf(' ') );
	// Remove the instuction from the string 'cmd', leaving only the parameters (This is to get the string 'cmd' ready to split the params into an array
	cmd = cmd.substring( 0, cmd.indexOf(' ')+1 );
	// Split the parameters into an array and save them in the 'result.params' struct string array
	VIS_ToArray(cmd, ',', result.params, sizeof(result.params));
	// Now return the 'result' struct with the instruction in 'result.instr' and the params in 'result.params'
	return result;
}

