/**
 * @file init.h
 * @author Micahel MacDonald (@Michael2MacDonald)
 * @brief Contains the initialization manager for all devices and modules
 */

#include "global.h" // Contains global variables and definitions
#include "errors.h" // Contains error handling functions, variables, and definitions

// ===SD Card===
#include "files.h"
// ===RTC===
#include <Teensy4Time.h>
// ===Nav & GPS===
#include "nav.h"

#include <PWMServo.h>
extern PWMServo Rudder;
extern PWMServo Motor;

/** TODO: 
 * - Create an array of startup functions
 * - Create a sub-array if a function depends on another function
 * 
 */


typedef auto (*LambdaFunc)()->int;
LambdaFunc Startup[][2] = { // PROGMEM or FLASHMEM ???

	{ // Start SD Card
		[]()->int{
			SerialDebug.println("STARTING SD CARD...");
			if (!SD.begin(SdioConfig(FIFO_SDIO))) {
				SerialDebug.println("====== ERROR: COULD NOT INITIALIZE SD CARD! ======");
				return -1; // Error
			} else {
				return 0; // Success
			}
		},
		[]()->int{ return 0; }
	}, // End SD Card

	{ // Start BNO085
		[]()->int{
			SerialDebug.println("STARTING BNO085...");
			if (!bno08x.begin_I2C()) {
				SerialDebug.println("====== ERROR: FAILED TO FIND BNO085! ======");
				return -1; // Error
			} else {
				return 0; // Success
			}
		},
		[]()->int{
			SerialDebug.println("SETTING UP BNO085 REPORTS...");
			if (!bno08x.enableReport(SH2_ROTATION_VECTOR, BNO08XInterval)) {
				SerialDebug.println("====== ERROR: COULD NOT ENABLE ROTATION VECTOR! ======");
				return -1; // Error
			} else {
				return 0; // Success
			}
		},
	}, // End BNO085

	{
		[]()->int{ return 0; },
		[]()->int{ return 0; }
	},
	{
		[]()->int{ return 0; },
		[]()->int{ return 0; }
	},
	{
		[]()->int{ return 0; },
		[]()->int{ return 0; }
	},
	{
		[]()->int{ return 0; },
		[]()->int{ return 0; }
	},
};

FLASHMEM void VesselStartup() {

	SerialDebug.println("STARTING SD CARD...");
	if (!SD.begin(SdioConfig(FIFO_SDIO))) {
		SerialDebug.println("====== ERROR: COULD NOT INITIALIZE SD CARD! ======");
	}

	SerialDebug.println("STARTING GPS SERIAL...");
	gpsSerial.begin(9600);

	// SerialDebug.println("STARTING RTC...");
	// if (!rtc.begin()) {
	// 	SerialDebug.println("====== ERROR: COULD NOT FIND RTC! ======");
	// }
	
	// SerialDebug.println("STARTING LSM303DLHC...");
	// if (!mag.begin()) {
	// 	SerialDebug.println("====== ERROR: COULD NOT FIND LSM303 MAGNETOMETER! ======");
	// }
	// if (!accl.begin()) {
	// 	SerialDebug.println("====== ERROR: COULD NOT FIND LSM303 ACCELEROMETER! ======");
	// }

	SerialDebug.println("STARTING BNO085...");
	if (!bno08x.begin_I2C()) {
		SerialDebug.println("====== ERROR: FAILED TO FIND BNO085! ======");
	} else {
		SerialDebug.println("SETTING UP BNO085 REPORTS...");
		if (! bno08x.enableReport(SH2_ROTATION_VECTOR, BNO08XInterval)) {
			SerialDebug.println("====== ERROR: COULD NOT ENABLE ROTATION VECTOR! ======");
		}
	}

	SerialDebug.println("SETTING UP RUDDER SERVO...");
	if (!Rudder.attach(22, RudderMinPW, RudderMaxPW)) {
		SerialDebug.println("====== ERROR: FAILED ATTACHING RUDDER SERVO! ======");
		HandleError(1);
	}
	Rudder.write(RudderTrim);

	Serial.println("SETTING UP ESC...");
	/** TODO: Connect power to esc */
	if (!Motor.attach(23, 1000, 2000)) {
		SerialDebug.println("====== ERROR: COULD NOT SET UP ESC! ======");
		SerialDebug.println("ABORT ARMING ESC DUE TO FAILURE SETTING UP ESC");
		HandleError(2);
		/** TODO: Disconnect power to esc before attempting setup again */
	} else {
		SerialDebug.println("ARMING ESC...");
		Motor.write(0);
	}
}