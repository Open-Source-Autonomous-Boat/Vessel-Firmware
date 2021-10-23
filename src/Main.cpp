// TODO: Add watchdog timer

#include "errors.h" // Contains checks to ensure everything is working

// ===RTC===
#include <RTClib.h>
#include "rtc.h"

// ===Nav & GPS===
#include <TinyGPS++.h>
#include "NavTools.h"			// Our custom library
#include "nav.h"
int Mode = 1;						// Vessel's mode of operation
// 0 = Standby? Hold?
// 1 = Waypoint Heading
// 2 = Waypoint Path
// 3 = Loiter
// 4 = Return Home
// 5 = Low Power
bool Fix;							// Set to true when the GPS has a fix
int Heading;						// The direction we are going
int Bearing;						// The dirrection we need to be going
int RelativeBearing;				// The difference between the direction we are going and the dirrection we need to be going
float CurrLat, CurrLong;		// The vessels current Lat and Long
float TargetLat, TargetLong;	// The vessels target Lat and Long
float WaypointRadius;
int TargetWaypoint;				// The index of the waypoint that the vessel is currently trying to reach
struct Waypoint {					// Struct to hold the waypoints
	float lat;
	float lon;
	float radius;
	// int action;
};
struct Waypoint Waypoints[20];

// ===SD Card===
#include "files.h"

// ===Rudder Servo===
#include <PWMServo.h>
PWMServo Rudder;
PWMServo Motor;
float RudderPos;
float RudderRange = 40;		// Max value for this is 127.5
float RudderTrim = 100;		// Tune this to the PWM value of the servo when straight
const int RudderMinPWM = 1000;		// Sets the min PWM pulse width
const int RudderMaxPWM = 2000;	// Sets the max PWM pulse width
float ThrottleMin = 15;		// Set later
float ThrottleMax = 15;		// Set later
float ThrottleOff = 0;		// PWM value 

#define SerialLog Serial1 // For bluetooth serial monitor
// #define SerialLog Serial // For USB serial monitor
#include "coms.h"

#include <Tasker.h>
Tasker tasker;

void setupDevices();
void serialLogDump();
void fetchSerialCommands();

void setup() {
	delay(1000); // If the run time is too fast it makes users feal like the program did nothing and causes distrust. This slows down the setup so that people trust it more.

	SerialLog.begin(115200); // Start serial
	SerialLog.println("SERIAL STARTED");
	SerialLog.println("===== STARTING UP VESSEL =====");
	setupDevices();
	SerialLog.println("LOADING MISSION PROFILE...");
	if(!loadMissionProfile("MissionProfiles/missionProfile1.json")){
		SerialLog.println("====== ERROR: COULD NOT LOAD MISSION PROFILE! ======");
	}
	TargetWaypoint = 0;
	TargetLat = Waypoints[TargetWaypoint].lat;
	TargetLong = Waypoints[TargetWaypoint].lon;
	WaypointRadius = Waypoints[TargetWaypoint].radius;

	// ===FOR TESTING ONLY===
	SerialLog.println("===== WAYPOINTS CURRENTLY LOADED =====");
	SerialLog.print(Waypoints[0].lat);
	SerialLog.print(", ");
	SerialLog.println(Waypoints[0].lon);
	// SerialLog.println(actions[Waypoints[0].action]);

	SerialLog.print(Waypoints[1].lat);
	SerialLog.print(", ");
	SerialLog.println(Waypoints[1].lon);
	// SerialLog.println(actions[Waypoints[1].action]);
	SerialLog.println("=========================================");
	// ======================
}

void loop() {
	tasker.loop();

	fetchSerialCommands();

	now = rtc.now();
	FetchGPS();		// Get the data from the GPS
	UpdateFix();	// Check if the GPS has a fix and write it to the 'fix' bool
	
	if(Fix && Mode == 1) { // If the GPS has a fix with satellites and we are in the right mode, start driving the boat
		Heading = GetHeading();															// Figure out what direction we are going
		Bearing = CalcBearing(CurrLat, CurrLong, TargetLat, TargetLong);	// Figure out what direction we need to be going
		RelativeBearing = Heading - Bearing;										// Figure out how much we need to turn to be going the right way
		RudderPos = ((255/127.5)*RelativeBearing)+RudderTrim;					// Figure out how much to steer the rudder to get us to turn the right way

		// Make sure we don't turn the rudder too much and pass our rudder min/max limits
		if(RudderPos >= (RudderTrim + RudderRange)){
			RudderPos = RudderTrim + RudderRange;
		}
		else if (RudderPos <= (RudderTrim - RudderRange)) {
			RudderPos = RudderTrim - RudderRange;
		}
		Rudder.write(RudderPos); // Steer the rudder
		Motor.write(15); // Start the motor

		if(CheckWaypointCompletion()){ // Check if we have reached the next waypoint
			TargetWaypoint++; // Advance to the next waypoint
		}

		// TODO: Add waypoint advancement
		// TODO: Add loiter mode
		// TODO: Loiter when last waypoint is reached
	}
	else {
		Motor.write(0); // Stop the motor
		Rudder.write(RudderTrim); // Center the rudder
	}

}	// End loop


void setupDevices(){
	SerialLog.println("STARTING GPS SERIAL...");
	gpsSerial.begin(9600);

	SerialLog.println("STARTING RTC...");
	if (!rtc.begin()) {
		SerialLog.println("====== ERROR: COULD NOT FIND RTC! ======");
	}
	
	SerialLog.println("STARTING LSM303DLHC...");
	if (!mag.begin()) {
		SerialLog.println("====== ERROR: COULD NOT FIND LSM303 MAGNETOMETER! ======");
	}
	if (!accl.begin()) {
		SerialLog.println("====== ERROR: COULD NOT FIND LSM303 ACCELEROMETER! ======");
	}

	SerialLog.println("SETTING UP RUDDER SERVO...");
	if(!Rudder.attach(22, RudderMinPWM, RudderMaxPWM)){
		SerialLog.println("====== ERROR: FAILED ATTACHING RUDDER SERVO! ======");
	}

	Serial.println("SETTING UP ESC...");
	if (!Motor.attach(23, 1000, 2000)) {
		SerialLog.println("====== ERROR: COULD NOT SET UP ESC! ======");
		SerialLog.println("ABORT ARMING ESC DUE TO FAILURE SETTING UP ESC");
	}
	else {
		SerialLog.println("ARMING ESC...");
		Motor.write(0);
	}


	SerialLog.println("STARTING SD CARD...");
	if (!SD.begin(SdioConfig(FIFO_SDIO))) {
		SerialLog.println("====== ERROR: COULD NOT INITIALIZE SD CARD! ======");
	}
}
