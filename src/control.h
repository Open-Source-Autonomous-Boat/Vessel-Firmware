/**
 * @file control.h
 * @author Micahel MacDonald (@Michael2MacDonald)
 * @brief Contains VIS command management and execution functions
 */

/** TODO: Rename to Command Control System (CCS)????? */

#ifndef _CONTROL_H_
#define _CONTROL_H_

#include "global.h" // Contains global variables and definitions
#include "errors.h" // Contains error handling functions, variables, and definitions

int SetNavMode(VISCommand cmd) {
	if ( cmd.params[0].toInt() == (0||1||2||3||4||5||6||7) ) {
		Mode = cmd.params[0].toInt();
		return 0; // No error
	}
	/** TODO: Invalid Parameter error code */
	return -1; // Error: Invalid Parameter
}

int SetWaypoint(VISCommand cmd) {
	// Make sure parameters are valid (lat&lon are not empty and index is empty or in a valid range)
	if ( cmd.params[0]!="" && cmd.params[1]!="" && ( (0>=cmd.params[2].toInt() && cmd.params[2].toInt()>MAX_NUM_WAYPOINTS) || cmd.params[2]!="" ) ) {
		if (cmd.params[2]!="") { // If no Waypoint index is given then add it to the end
			int last = -1;
			for (uint i = 0; i<sizeof(Waypoints); i++) { // Find the last waypoint
				if (Waypoints[i].lat) { last=i; break; }
			}
			if (last != -1) { // Make sure the array is not full
				Waypoints[last].lat = cmd.params[0].toFloat(); // Set Lat
				Waypoints[last].lon = cmd.params[1].toFloat(); // Set Lon
			} else { return -1; } // Error: Array Overflow
		} else { // Set waypoint
			Waypoints[cmd.params[2].toInt()].lat = cmd.params[0].toFloat(); // Set Lat
			Waypoints[cmd.params[2].toInt()].lon = cmd.params[1].toFloat(); // Set Lon
		}
	}

	return -1; // Error: Invalid Parameter
}

int RemoveWaypoint(VISCommand cmd) {
	uint index = cmd.params[0].toInt();
	// Make sure parameters are valid (index is in a valid range)
	if ( 0>=index && index<MAX_NUM_WAYPOINTS ) {
		uint last = 0;
		// Remove waypoint by shifting the following waypoints down one index
		for (uint i = index; i<MAX_NUM_WAYPOINTS-1 && Waypoints[1].lat; i++) {
			Waypoints[i] = Waypoints[i+1];
			last = i;
		}
		// Clear the last waypoint after shifting it down so we don't have a duplicate waypoint
		Waypoints[last+1] = (Waypoint){.lat=0, .lon=0, .radius=0, .cmd1=0, .cmd2=0};
	}
	return -1; // Error: Invalid Parameter
}

int SetCommandPreset(VISCommand cmd) {
	int index = cmd.params[0].toInt();
	// Make sure parameters are valid (index is in a valid range and there are not too many parameters)
	if ( 0>=index && index>MAX_NUM_CMD_PRESETS) {
		uint p = 1;
		for (uint i=1; i<(sizeof(CommandPresets)-(index+1)) && cmd.params[p]!=NULL; i++) { // For the number of remaining 'CommandPresets' slots; While cmd.params[p] is not null
			CommandPresets[index++] = VIS_Decode(cmd.params[p]);
			p++;
		}
	}
	return -1; // Error: Invalid Parameter
}


void VISLutSetup() {
	uint index = 0;
	VISLut[index++] = {.instr = "C1", .funcPtr = SetNavMode};
	VISLut[index++] = {.instr = "W1", .funcPtr = SetWaypoint};
	VISLut[index++] = {.instr = "W2", .funcPtr = RemoveWaypoint};
	VISLut[index++] = {.instr = "D1", .funcPtr = SetCommandPreset};
	// VISLut[index++] = {.instr = "", .funcPtr = };
	// VISLut[index++] = {.instr = "", .funcPtr = };
}

void RunVISCommand(VISCommand cmd) {
	for (uint i=0; i<sizeof(VISLut); i++) { // Search through the VIS LUT for the instruction function pointer
		if (VISLut[i].instr = cmd.instr) { // If you find the instruction run the function
			VISLut[i].funcPtr(cmd);
		}
	}
}

#endif // End _CONTROL_H_