/**
 * @file errors.h
 * @author Micahel MacDonald (@Michael2MacDonald)
 * @brief Contains error handling functions, variables, and definitions
 */

#ifndef ERRORS_H_
#define ERRORS_H_

#include "global.h"

/** TODO: Invalid Parameter error code for command functions */

enum ErrorSeverity {
	Unrecoverable, // This should NEVER happen
	CriticalError, // Critical Error (Restart Teensy???)
	Error,         // Error
	Warning,       // Log it and notify base
	Informational, // Not important; Log it
	Action,        // Manual action is required but the issue is not severe; log it and notify base
};
PROGMEM String ErrorSeverityMsg[] = {
	"===== UNRECOVERABLE ERROR OCCURRED!! ===== \nUNRECOVERABLE: ",
	"CRITICAL ERROR: ",
	"ERROR: ",
	"WARNING: ",
	"INFO: ",
	"ACTION: ",
};

struct ErrorCode {
	int id;
	ErrorSeverity severity;
	String logMsg;
};

PROGMEM ErrorCode ErrorCodes[MAX_NUM_ERROR] {
	ErrorCode {.id=0, .severity=Informational, .logMsg="Success, No Error Occurred"},
	ErrorCode {.id=1, .severity=Informational, .logMsg="Invalid PWM Pin Number (Rudder Servo)"},
	ErrorCode {.id=2, .severity=Informational, .logMsg="Invalid PWM Pin Number (Motor ESC)"},
};


void HandleError(int code) { // Call this function and pass an error code when an error occurs
	/** TODO: Use log function instead of using SerialDebug.print() */
	SerialDebug.print(ErrorSeverityMsg[ErrorCodes[code].severity]); // Start with the Error Severity message
	SerialDebug.println(ErrorCodes[code].logMsg); // Then print the unique Error message
}


#endif // End ERRORS_H_