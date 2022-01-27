#ifndef _VIS_TOOLS_H /* Start of _VIS_TOOLS_H */
#define _VIS_TOOLS_H

#include <Arduino.h>

#ifndef VIS_TOOLS_NUM_PARAMS
#define VIS_TOOLS_NUM_PARAMS 10
#endif

/** TODO: LUT with a struct with instruction and pointer to function for  */

template<unsigned int numParams>
struct VISCommandTmp { // Holds an instruction and its required parameters
	String instr; // Instruction
	String params[numParams]; // Instruction parameters
};
typedef VISCommandTmp<VIS_TOOLS_NUM_PARAMS> VISCommand;

// void VIS_StrToStrArray(String input, char delimiter, String* array, int arraySize);
void VIS_ToArray(String input, char delimiter, String* array, int arraySize);
VISCommand VIS_Decode(String cmd);

#endif /* End of _VIS_TOOLS_H */
