/**
 * IMPORTANT: Our the functions in loop() take too long to execute causing the serial buffer to fill up before we can read it. Increasing the serial buffer size for Serial2 fixes this.
 * To do this, first go to the 'HardwareSerial2.cpp' file in your teensy core
 * Then change '#define SERIAL2_RX_BUFFER_SIZE 64' to '#define SERIAL2_RX_BUFFER_SIZE 256' and save the file
 * This file can usually be found here on windows: 'C:\Program Files (x86)\Arduino\hardware\teensy\avr\cores\teensy4\HardwareSerial2.cpp'
 */

#include "global.h"

// ===GPS===
#include <TinyGPS++.h>
TinyGPSPlus gps;
TinyGPSCustom GPSFix(gps, "GPGGA", 7);

// ===LSM303===
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM303_U.h>

// Assign a unique ID to LSM303 sensor
Adafruit_LSM303_Accel_Unified accl = Adafruit_LSM303_Accel_Unified(00000);
Adafruit_LSM303_Mag_Unified mag = Adafruit_LSM303_Mag_Unified(00001);

//Calibration variables for LSM303. Callibration sketch here: https://learn.adafruit.com/lsm303-accelerometer-slash-compass-breakout/calibration
const float magMinX = -69.73;
const float magMaxX = 50.36;
const float magMinY = -29.27;
const float magMaxY = 94.91;
const float magMinZ = -76.94;
const float magMaxZ = 77.24;
const float refMinX = -54.51;
const float refMaxX = 49.02;
const float refMinY = -54.08;
const float refMaxY = 55.91;
const float refMinZ = -57.62;
const float refMaxZ = 55.57;

// This function runs two point calibration for the LSM303 magnetometer
FASTRUN float getCorrectedValue(float value, float rawMin, float rawMax, float refMin, float refMax){
	float rawRange = rawMax - rawMin;
	float refRange = refMax - refMin;
	float correctedValue = (((value-rawMin)*refRange)/rawRange)+refMin;
	return correctedValue;
}

FASTRUN float GetHeading(){
	// Get new sensor events
	sensors_event_t MagEvent;
	mag.getEvent(&MagEvent);
	sensors_event_t AcclEvent;
	accl.getEvent(&AcclEvent);

	float correctedX = getCorrectedValue(MagEvent.magnetic.x, magMinX, magMaxX, refMinX, refMaxX);
	float correctedY = getCorrectedValue(MagEvent.magnetic.y, magMinY, magMaxY, refMinY, refMaxY);

	// Calculate the angle of the vector y,x
	float heading = (atan2(correctedY, correctedX) * 180) / M_PI;

	// Normalize to 0-360
	if (heading < 0) {
		heading = 360 + heading;
	}
	return heading;

	if(DEBUG_RAW_IMU){ // Activate in 'global.h'
		// Display the results (magnetic vector values are in micro-Tesla (uT))
		SerialDebug.print("X: "); SerialDebug.print(MagEvent.magnetic.x); SerialDebug.print("  ");
		SerialDebug.print("Y: "); SerialDebug.print(MagEvent.magnetic.y); SerialDebug.print("  ");
		SerialDebug.print("Z: "); SerialDebug.print(MagEvent.magnetic.z); SerialDebug.print("  ");SerialDebug.println("uT");

		// Display the results (acceleration is measured in m/s^2) */
		SerialDebug.print("X: "); SerialDebug.print(AcclEvent.acceleration.x); SerialDebug.print("  ");
		SerialDebug.print("Y: "); SerialDebug.print(AcclEvent.acceleration.y); SerialDebug.print("  ");
		SerialDebug.print("Z: "); SerialDebug.print(AcclEvent.acceleration.z); SerialDebug.print("  ");SerialDebug.println("m/s^2 ");
	}
	
}

bool WaypointComplete(){ // Returns true if vessel has reached the target waypoint
	float dist = CalcDistance(CurrLat, CurrLong, Waypoints[Target].lat, Waypoints[Target].lon);
	if(dist <= Waypoints[Target].radius){
		return true;
	} else {
		return false;
	}
}

void FetchGPS() {
	// Fetch GPS data if it is available
	if (gpsSerial.available()) {	// Check if GPS is connected and online
		char c = gpsSerial.read();	// Read the GPS serial port
		if (gps.encode(c)) {			// Check for valid data
				CurrLat = gps.location.lat();
				CurrLong = gps.location.lng();

			// TODO: Get speed and course over ground (track) from GPS
			
			if (gps.date.isValid() && gps.time.isValid() && Fix) {
				UpdateClock(gps.date.year(), gps.date.month(), gps.date.day(), gps.time.hour(), gps.time.minute(), gps.time.second());
			}
		}
		if (DEBUG_RAW_GPS){ // Activate in 'global.h'
			SerialDebug.write(c); // Print the raw GPS sentences
		}
	}

}

void UpdateFix(){ // Sets 'Fix' to true if GPS fix is above 0
	if(GPSFix.isUpdated()) {
		String f = GPSFix.value();
		switch (f.toInt()) {
			case 0:
				Fix = false;
			break;
			case 1:
				Fix = true;
			break;
			case 2:
				Fix = true;
			break;
			case 4:
				Fix = true;
			break;
			case 5:
				Fix = true;
			break;
			case 6:
				Fix = true;
			break;
			case 7:
				Fix = true;
			break;
			case 8:
				Fix = true;
			break;
			case 9:
				Fix = true;
			break;
		}
	}
}

/**======================
 *    Navigation Modes
 *========================**/

void ModeHold(){
	ThrottlePWM = ThrottleOff; // Stop the motor
	RudderPWM = RudderTrim; // Center the rudder
}

void ModeWaypointHeading() {
	Bearing = CalcBearing(CurrLat, CurrLong, Waypoints[Target].lat, Waypoints[Target].lon); // Find the direction we need to be going
	RelativeBearing = Heading - Bearing; // Figure out the angle between the vessel and the target waypoint
	// Figure out how much to turn the rudder by using the RelativeBearing as an input to a linear equation where y=RudderPWM
	RudderPWM = ((255/127.5)*RelativeBearing)+RudderTrim;
}

void ModeWaypointPath() {
	// TODO: Write code
}

void ModeHeading() {
	// TODO: Write code
}

void ModeLoiter() {
	// TODO: Write code
}

void ModeAbort() {
	float dist = CalcDistance(CurrLat, CurrLong, Waypoints[Target].lat, Waypoints[Target].lon);
	if(dist <= Waypoints[Target].radius){
		// TODO: Add loiter code
	} else {
		Bearing = CalcBearing(CurrLat, CurrLong, Abort.lat, Abort.lon); // Find the direction we need to be going
		RelativeBearing = Heading - Bearing; // Figure out the angle between the vessel and the target waypoint
		// Figure out how much to turn the rudder by using the RelativeBearing as an input to a linear equation where y=RudderPWM
		RudderPWM = ((255/127.5)*RelativeBearing)+RudderTrim;
	}
}

void ModeReturnHome() {
	Bearing = CalcBearing(CurrLat, CurrLong, Waypoints[Target].lat, Waypoints[Target].lon); // Find the direction we need to be going
	RelativeBearing = Heading - Bearing; // Figure out the angle between the vessel and the target waypoint
	// Figure out how much to turn the rudder by using the RelativeBearing as an input to a linear equation where y=RudderPWM
	RudderPWM = ((255/127.5)*RelativeBearing)+RudderTrim;
}

// void LowPower() {
//	// TODO: Write code
// }
