// TODO: Add watchdog timer
#define ARDUINOJSON_ENABLE_PROGMEM 0 // Prevent errors when trying to use PROGMEM
#include "global.h" // Contains global variables and definitions
#include "errors.h" // Contains checks to ensure everything is working and error handling for if it is not working

// ===RTC===
// #include <RTClib.h>
#include <Teensy4Time.h>
#include "rtc.h"

// ===Nav & GPS===
#include <TinyGPS++.h>
#include "NavTools.h" // Our custom library
#include "nav.h"

// ===SD Card===
#include "files.h"

// ===Rudder Servo===
#include <PWMServo.h>
PWMServo Rudder;
PWMServo Motor;

// ===Comunication===
// TODO: Add satellite comunications
#include "coms.h"
#include <string.h>

// ===Scheduling===
#include <Tasker.h>
Tasker tasker;

void deviceSetup(); // Declare deviceSetup() function before it is used in setup()
void run(); // Declare run() function before it is used in loop()

FLASHMEM void setup() {
	delay(1000); // Delays startup for when turning on or uploading new code to help will debugging

	tasker.setInterval(updateMissionLog, 1000); // Update mission log every 1 second

	SerialDebug.begin(115200); // Start serial for debugging
	SerialDebug.println("SERIAL STARTED");
	SerialDebug.println("===== CRASH REPORT =====");
	SerialDebug.println(CrashReport);
	SerialDebug.println("========================");
	SerialDebug.println("===== STARTING UP VESSEL =====");

	deviceSetup(); // Setup code for all devices/modules

	SerialDebug.println("LOADING MISSION PROFILE...");
	if (!loadMissionProfile(MISSION_ONE)) {
		SerialDebug.println("====== ERROR: COULD NOT LOAD MISSION PROFILE! ======");
		SerialDebug.println("FALLBACK: LOADING DEFAULT MISSION PROFILE...");
		if (!loadMissionProfile(MISSION_DEFAULT)) {
			SerialDebug.println("====== ERROR: COULD NOT LOAD DEFAULT MISSION PROFILE! ======");
			// TODO: Add error handling
		}
	}
	
	// TODO: Get current waypoint from non volatile memory
	Target = 1;
	// TODO: figure out how the vessel will know when to change modes other than arriving at waypoints
	Mode = 1;

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

	// NVRAM_START; // Start the Teensy's 16 bytes of NVRAM

	// NVRAM_WRITE(0, 4, 4, 15);
	// SerialDebug.println(NVRAM_READ(0, 4, 4), HEX);
	
	// SerialDebug.println();
}

FASTRUN void loop() {

	if (Mode==7) {
		// TODO: Write code
	} else {
		run();
	}

}	// End loop


FLASHMEM void deviceSetup() {
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
	
	SerialDebug.println("STARTING LSM303DLHC...");
	if (!mag.begin()) {
		SerialDebug.println("====== ERROR: COULD NOT FIND LSM303 MAGNETOMETER! ======");
	}
	if (!accl.begin()) {
		SerialDebug.println("====== ERROR: COULD NOT FIND LSM303 ACCELEROMETER! ======");
	}

	SerialDebug.println("SETTING UP RUDDER SERVO...");
	if (!Rudder.attach(22, RudderMinPW, RudderMaxPW)) {
		SerialDebug.println("====== ERROR: FAILED ATTACHING RUDDER SERVO! ======");
	}
	Rudder.write(RudderTrim);

	Serial.println("SETTING UP ESC...");
	if (!Motor.attach(23, 1000, 2000)) {
		SerialDebug.println("====== ERROR: COULD NOT SET UP ESC! ======");
		SerialDebug.println("ABORT ARMING ESC DUE TO FAILURE SETTING UP ESC");
	} else {
		SerialDebug.println("ARMING ESC...");
		Motor.write(0);
	}
}

FASTRUN void run() {
	tasker.loop();

	fetchSerialCommands();

	// now = rtc.now();
	FetchGPS();		// Get the data from the GPS
	UpdateFix();	// Check if the GPS has a fix and write it to the 'fix' bool
	
	if (Fix) { // If the GPS has a fix with satellites, start driving the boat

		Heading = GetHeading(); // Find the direction the vessel is facing

		switch (Mode) {
		case 0: // Hold
			ModeHold(); // Do nothing
			break;
		case 1: // Waypoint Bearing
			ModeWaypointHeading();
			break;
		case 2: // Waypoint Course
			ModeWaypointPath();
			break;
		case 3: // Heading
			ModeHeading();
			break;
		case 4: // Loiter
			ModeLoiter();
			break;
		case 5: // Return Home
			ModeReturnHome();
			break;
		case 6: // Abort
			ModeAbort();
			break;
		default: // If for some reason the input is invalid
			// TODO: Error
			ModeHold(); // Do nothing
			break;
		}

		// Make sure we don't turn the rudder too much and pass our rudder min/max limits
		if (RudderPWM >= (RudderTrim + RudderRange)) {
			RudderPWM = RudderTrim + RudderRange;
		}
		else if (RudderPWM <= (RudderTrim - RudderRange)) {
			RudderPWM = RudderTrim - RudderRange;
		}
		
		ThrottlePWM = 15; // Set throttle to

		if (WaypointComplete()) { // Check if we have reached the next waypoint
			Target++; // Advance to the next waypoint
			if (Waypoints[Target].changeMode != _NULL){ // If it is NULL just keep the current mode
				Mode = Waypoints[Target].changeMode;
			}
		}
	}
	else { // Hold
		ModeHold();
	}

	// Only write to the servo if we have a new PWM value to give it
	static short curRudderPos = RudderTrim; // Set the current Rudder PWM to center on startup
	if (RudderPWM != curRudderPos) {
		Rudder.write(RudderPWM);	// If the throttle has changed then update the ESC
		curRudderPos = RudderPWM;	// Update the current Rudder PWM
	}

	// Only write to the ESC if we have a new PWM (throttle) value to give it
	static short curThrottle = ThrottleOff; // Set the current throttle to 0 on startup
	if (ThrottlePWM != curThrottle) {
		Motor.write(ThrottlePWM);	// If the throttle has changed then update the ESC
		curThrottle = ThrottlePWM;	// Update the current throttle
	}
}