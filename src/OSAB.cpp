/* TODO:
 * Add error handling (What do you do when the library can't connect to the lsm303?)
 * Add watchdog timer
 */

#include "errors.h" // Contains checks to ensure everything is working

// ===RTC===
#include <RTClib.h>
#include "rtc.h"

// ===Nav & GPS===
#include <TinyGPS++.h>
#include "NavTools.h"			// Our custom library
#include "navigation.h"
bool Fix;							// Set to true when the GPS has a fix
int Heading;						// The direction we are going
int Bearing;						// The dirrection we need to be going
int RelativeBearing;				// The difference between the direction we are going and the dirrection we need to be going
float CurrLat, CurrLong;		// The vessels current Lat and Long
float TargetLat, TargetLong;	// The vessels target Lat and Long
int TargetWaypoint;				// The number of the waypoint that the vessel is currently trying to reach
struct Waypoint {					// Struct to hold the waypoints
	float lat;
	float lon;
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
float RudderRange = 40;		// Max value is 127.5
float RudderTrim = 100;		// PWM value of servo when straight
int RudderMinPWM = 500;		// Don't drive the servo under this value
int RudderMaxPWM = 2500;	// Don't drive the servo over this value

void setupDevices();
void serialLogDump();

void setup() {
	delay(1000); // If the run time is too fast it makes users feal like the program did nothing and causes distrust. This slows down the setup so that people trust it more.

	Serial.begin(115200); // Start serial
	Serial.println("SERIAL STARTED");
	Serial.println("===== STARTING UP VESSEL =====");
	setupDevices();
	Serial.println("LOADING MISSION PROFILE...");
	if(!loadMissionProfile("MissionProfiles/missionProfile1.json")){
		Serial.println("====== ERROR: COULD NOT LOAD MISSION PROFILE! ======");
	}
	TargetWaypoint = 0;
	TargetLat = Waypoints[TargetWaypoint].lat;
	TargetLong = Waypoints[TargetWaypoint].lon;


	// ===Check Mission File Integrity===
	// ToDo: Code goes here
	// ==================================

	// ToDo: Check communication link integrity
	// ==========================

	// ===Initiate Satellite Communication===
	// ToDo: Code goes here
	// ToDo: Send startup message
	// ===========================

	// ===Compile Mission File===
	// ToDo: Code goes here
	// ==========================



	// ===FOR TESTING ONLY===
	Serial.println("===== WAYPOINTS CURRENTLY LOADED =====");
	Serial.print(Waypoints[0].lat);
	Serial.print(", ");
	Serial.println(Waypoints[0].lon);
	// Serial.println(actions[Waypoints[0].action]);

	Serial.print(Waypoints[1].lat);
	Serial.print(", ");
	Serial.println(Waypoints[1].lon);
	// Serial.println(actions[Waypoints[1].action]);
	Serial.println("=========================================");
	// ======================
}

void loop() {
	now = rtc.now();
	FetchGPS();		// Get the data from the GPS
	UpdateFix();	// Check if the GPS has a fix and write it to the 'fix' bool
	
	if(Fix) { // If the GPS has a fix with satellites, start driving the boat
		Heading = getHeading();															// Figure out what direction we are going
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
		Motor.write(102); // Start the motor
	}
	else {
		Motor.write(100); // Stop the motor
		Rudder.write(RudderTrim); // Center the rudder
	}

	serialLogDump();

}	// End loop


void setupDevices(){
	Serial.println("STARTING GPS SERIAL...");
	gpsSerial.begin(9600);

	Serial.println("STARTING RTC...");
	if (!rtc.begin()) {
		Serial.println("====== ERROR: COULD NOT FIND RTC! ======");
	}
	
	Serial.println("STARTING LSM303DLHC...");
	if (!mag.begin()) {
		Serial.println("====== ERROR: COULD NOT FIND LSM303 MAGNETOMETER! ======");
	}
	if (!accl.begin()) {
		Serial.println("====== ERROR: COULD NOT FIND LSM303 ACCELEROMETER! ======");
	}

	Serial.println("SETTING UP RUDDER SERVO...");
	if(!Rudder.attach(22, RudderMinPWM, RudderMaxPWM)){
		Serial.println("====== ERROR: FAILED ATTACHING RUDDER SERVO! ======");
	}

	Serial.println("SETTING UP ESC...");
	if (!Motor.attach(23)) {
		Serial.println("====== ERROR: COULD NOT SET UP ESC! ======");
		Serial.println("ABORT ARMING ESC DUE TO FAILURE SETTING UP ESC");
	}
	else {
		Serial.println("ARMING ESC...");
		Motor.write(90); // You must set throttle to neutral to arm ESC
	}


	Serial.println("STARTING SD CARD...");
	if (!SD.begin(SdioConfig(FIFO_SDIO))) {
		Serial.println("====== ERROR: COULD NOT INITIALIZE SD CARD! ======");
	}
}

void serialLogDump(){
	Serial.print("Date/Time: ");
	Serial.print(now.year(), DEC);
	Serial.print('/');
	Serial.print(now.month(), DEC);
	Serial.print('/');
	Serial.print(now.day(), DEC);
	Serial.print(" ");
	Serial.print(now.hour(), DEC);
	Serial.print(':');
	Serial.print(now.minute(), DEC);
	Serial.print(':');
	Serial.print(now.second(), DEC);
	if (gps.date.isValid()) {
		Serial.print("	Date: ");
		Serial.print(gps.date.year());
		Serial.print('/');
		Serial.print(gps.date.month());
		Serial.print('/');
		Serial.print(gps.date.day());
		// Serial.print(" ");
	} else {
		Serial.print("	Date: ");
		Serial.print("xx");
		Serial.print(':');
		Serial.print("xx");
		Serial.print(':');
		Serial.print("xx");
	}
	if (gps.time.isValid()) {
		Serial.print("	Time: ");
		Serial.print(gps.time.hour());
		Serial.print(':');
		Serial.print(gps.time.minute());
		Serial.print(':');
		Serial.print(gps.time.second());
	} else {
		Serial.print("	Time: ");
		Serial.print("xx");
		Serial.print(':');
		Serial.print("xx");
		Serial.print(':');
		Serial.print("xx");
	}
	Serial.print("	GPS Fix: ");
	Serial.print(GPSFix.value());
	Serial.print("	Lat/Long: ");
	Serial.print(CurrLat,6);
	Serial.print(", ");
	Serial.print(CurrLong,6);
	Serial.print("	Heading: ");
	Serial.print(Heading);
	Serial.print("	Bearing:");
	Serial.print(Bearing);
	Serial.println();
}
