/**
 * @file Main.cpp
 * @author Micahel MacDonald (@Michael2MacDonald)
 * @brief The main file and entry point for the vessel firmware, containing setup() and loop()
 * 
 * Copyright (C) 2021 By Michael MacDonald
 * This code is licensed under (INSERT_LICENSE) license (see src/LICENSE.txt for details)
 * 
 */

/** TODO: Add watchdog timer */

#include "global.h" // Contains global variables and definitions
#include "errors.h" // Contains error handling functions, variables, and definitions

#include "init.h" // Contains the initialization manager for all devices and modules
#include "control.h" // Contains VIS command management and execution functions
#include "nav.h" // Contains GPS, BNO085, and navigation functions 
#include "coms.h" // Contains communication and remote command functions
// #include "Flag.h" // Future support for bit flags to save space in non-volitile memory

// ===Rudder Servo===
#include <PWMServo.h>
PWMServo Rudder;
PWMServo Motor;

// ===Scheduling===
#include <Tasker.h> // To be replaced soon by an interupt driven, priority thread manager
Tasker tasker;


void deviceSetup(); // Declare deviceSetup() function before it is used in setup()
void run(); // Declare run() function before it is used in loop()

FLASHMEM void setup() {
	delay(1000); // Delays startup for when turning on or uploading new code to help with debugging

	tasker.setInterval(UpdateMissionLog, MISSION_LOG_UPDATE_INTREVAL); // Update mission log periodically

	SerialDebug.begin(115200); // Start serial for debugging
	SerialDebug.println("SERIAL STARTED");
	SerialDebug.println();
	SerialDebug.println("===== CRASH REPORT =====");
	SerialDebug.println(CrashReport);
	SerialDebug.println("========================");
	SerialDebug.println();
	SerialDebug.println("===== STARTING UP VESSEL =====");

	VesselStartup(); // Setup code for all devices/modules

	/** TODO: Move to init.h */
	SerialDebug.println("LOADING MISSION PROFILE...");
	if (!loadMissionProfile(MISSION_ONE)) {
		SerialDebug.println("====== ERROR: COULD NOT LOAD MISSION PROFILE! ======");
		SerialDebug.println("FALLBACK: LOADING DEFAULT MISSION PROFILE...");
		if (!loadMissionProfile(MISSION_DEFAULT)) {
			SerialDebug.println("====== ERROR: COULD NOT LOAD DEFAULT MISSION PROFILE! ======");
			// TODO: Add error handling
		}
	}
	
	/** TODO: Move to init.h */
	/** TODO: Get current waypoint from non-volatile memory */
	Target = 1;
	/** TODO: Figure out how the vessel will know when to change modes other than arriving at waypoints */
	Mode = 1;

	/** TODO: Move to init.h */
	if (DEBUG_LOADED_WAYPOINTS) { // List waypoints loaded from the mission profile
		SerialDebug.println("===== WAYPOINTS CURRENTLY LOADED =====");
		for (unsigned int i=0; i<sizeof(Waypoints)/sizeof(Waypoints[0]); i++) {
			if (Waypoints[i].lon == _NULL) {break;} // If the waypoint is null then we have reached the end of the waypoints
			SerialDebug.print(Waypoints[i].lat);
			SerialDebug.print(", ");
			SerialDebug.println(Waypoints[i].lon);
		}
		SerialDebug.println("=========================================");
	}

} // End setup()

FASTRUN void loop() {

	if (Mode==7) {
		// TODO: Write code
	} else {
		run();
	}

} // End loop()


FASTRUN void run() {
	tasker.loop(); // To be replaced

	ReceiveCommunications(); // Receives and processes incoming communications

	FetchGPS();  // Get the data from the GPS
	UpdateFix(); // Check if the GPS has a fix and write it to 'Fix'

	// Moved out of if(Fix){} so that the data will be logged and available even if there is not a fix
	Heading = GetHeading(); // Find the direction the vessel is facing
	/** TODO: Calculate Bearing?/Track? so that the data will be logged and available even if there is not a fix */
	
	if (Fix) { // If the GPS has a fix with satellites, start driving the boat

		switch (Mode) {
		case MODE_HOLD:
			ModeHold(); // Hold: Do nothing
			break;
		case MODE_WP_BEARING:
			ModeWaypointHeading(); // Waypoint Bearing
			break;
		case MODE_WP_COURSE:
			ModeWaypointPath(); // Waypoint Course
			break;
		case MODE_HEADING:
			ModeHeading(); // Heading
			break;
		case MODE_LIOTER:
			ModeLoiter(); // Loiter
			break;
		case MODE_HOME:
			ModeReturnHome(); // Return Home
			break;
		case MODE_ABORT:
			ModeAbort(); // Abort
			break;
		default: // If for some reason the input is invalid
			/** TODO: Error */
			ModeHold(); // Do nothing
			break;
		}

		// Make sure we don't turn the rudder too much and pass our rudder min/max limits
		if (RudderPWM >= (RudderTrim + RudderRange)) {
			RudderPWM = RudderTrim + RudderRange;
		} else if (RudderPWM <= (RudderTrim - RudderRange)) {
			RudderPWM = RudderTrim - RudderRange;
		}
		
		/** TODO: Come up with a throttle value system */
		ThrottlePWM = 15; // Set throttle to 15

		if (WaypointComplete()) { // Check if we have reached the next waypoint
			if (Waypoints[Target].cmd2 != -1) {
				RunVISCommand(CommandPresets[Waypoints[Target].cmd1]);
			}
			if ( (unsigned)Target<(sizeof(Waypoints)-1) ) {
				Target++; // Advance to the next waypoint
				if (Waypoints[Target].cmd1 != -1) {
					RunVISCommand(CommandPresets[Waypoints[Target].cmd1]);
				}
			} else {
				/** TODO: What do we do when the end of the waypoints ar reached? */
				Mode = MODE_HOLD;
			}
			// if (Waypoints[Target].changeMode != _NULL){ // If it is NULL just keep the current mode
			// 	Mode = Waypoints[Target].changeMode;
			// }
		}
	} else { // Hold
		ModeHold();
	} // End if(Fix)

	// Only write to the servo if we have a new PWM value to give it
	static short curRudderPos = RudderTrim; // Set the current Rudder PWM to center on startup
	if (RudderPWM != curRudderPos) {
		SerialDebug.println("====UPDATED RUDDER====");
		Rudder.write(RudderPWM);	// If the throttle has changed then update the ESC
		curRudderPos = RudderPWM;	// Update the current Rudder PWM
	}
	// Only write to the ESC if we have a new PWM (throttle) value to give it
	static short curThrottle = ThrottleOff; // Set the current throttle to 0 on startup
	if (ThrottlePWM != curThrottle) {
		Motor.write(ThrottlePWM);	// If the throttle has changed then update the ESC
		curThrottle = ThrottlePWM;	// Update the current throttle
	}

} // End run()