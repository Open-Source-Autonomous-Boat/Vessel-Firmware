// TODO: Add check for if we have reached waypoint

// ===GPS===
#define gpsSerial Serial2 // Teensy 4.1 hardware serial port 2. This is the same as setting the serial pins to 7 and 8
// IMPORTANT: Our loop() takes too long causing the serial buffer to fill up before we can read it. Increasing the serial buffer size for Serial2 fixes this.
// To do this, go to 'C:\Program Files (x86)\Arduino\hardware\teensy\avr\cores\teensy4\HardwareSerial2.cpp'
// Then change '#define SERIAL2_RX_BUFFER_SIZE 64' to '#define SERIAL2_RX_BUFFER_SIZE 256' and save the file
#include <TinyGPS++.h>
TinyGPSPlus gps;
TinyGPSCustom GPSFix(gps, "GPGGA", 7);

extern float CurrLat, CurrLong;
extern bool Fix;

// ===LSM303===
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM303_U.h>

// Assign a unique ID to LSM303 sensor
Adafruit_LSM303_Accel_Unified accl = Adafruit_LSM303_Accel_Unified(00000);
Adafruit_LSM303_Mag_Unified mag = Adafruit_LSM303_Mag_Unified(00001);

//Calibration variables for LSM303. Callibration sketch here: https://learn.adafruit.com/lsm303-accelerometer-slash-compass-breakout/calibration
float magMinX = -69.73;
float magMaxX = 50.36;
float magMinY = -29.27;
float magMaxY = 94.91;
float magMinZ = -76.94;
float magMaxZ = 77.24;
float refMinX = -54.51;
float refMaxX = 49.02;
float refMinY = -54.08;
float refMaxY = 55.91;
float refMinZ = -57.62;
float refMaxZ = 55.57;


// This function runs two point calibration for the LSM303 magnetometer
float getCorrectedValue(float value, float rawMin, float rawMax, float refMin, float refMax){
	float rawRange = rawMax - rawMin;
	float refRange = refMax - refMin;
	float correctedValue = (((value-rawMin)*refRange)/rawRange)+refMin;
	return correctedValue;
}

float getHeading(){
	float pi = 3.14159;
	
	// Get new sensor events
	sensors_event_t MagEvent;
	mag.getEvent(&MagEvent);
	sensors_event_t AcclEvent;
	accl.getEvent(&AcclEvent);

	float correctedX = getCorrectedValue(MagEvent.magnetic.x, magMinX, magMaxX, refMinX, refMaxX);
	float correctedY = getCorrectedValue(MagEvent.magnetic.y, magMinY, magMaxY, refMinY, refMaxY);

	// Calculate the angle of the vector y,x
	float heading = (atan2(correctedY, correctedX) * 180) / pi;

	// Normalize to 0-360
	if (heading < 0) {
		heading = 360 + heading;
	}
	return heading;


	// Display the results (magnetic vector values are in micro-Tesla (uT))
	// Serial.print("X: "); Serial.print(MagEvent.magnetic.x); Serial.print("  ");
	// Serial.print("Y: "); Serial.print(MagEvent.magnetic.y); Serial.print("  ");
	// Serial.print("Z: "); Serial.print(MagEvent.magnetic.z); Serial.print("  ");Serial.println("uT");

	// Display the results (acceleration is measured in m/s^2) */
	// Serial.print("X: "); Serial.print(AcclEvent.acceleration.x); Serial.print("  ");
	// Serial.print("Y: "); Serial.print(AcclEvent.acceleration.y); Serial.print("  ");
	// Serial.print("Z: "); Serial.print(AcclEvent.acceleration.z); Serial.print("  ");Serial.println("m/s^2 ");
}

void FetchGPS() {
	// Fetch GPS data if it is available
	if (gpsSerial.available()) {	// Check if GPS is connected and online
		char c = gpsSerial.read();	// Read the GPS serial port
		if (gps.encode(c)) {			// Check for valid data
				CurrLat = gps.location.lat();
				CurrLong = gps.location.lng();

			// ToDo: Get speed, heading, etc from GPS.
			
			if (gps.date.isValid() && gps.time.isValid()) {
				UpdateClock(gps.date.year(), gps.date.month(), gps.date.day(), gps.time.hour(), gps.time.minute(), gps.time.second());
			}
		}
		// Serial.write(c); // Uncomment to see the GPS data flowing
	}

}

void UpdateFix(){
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
