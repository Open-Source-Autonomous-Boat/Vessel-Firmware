/**
 * @file files.h
 * @author Micahel MacDonald (@Michael2MacDonald)
 * @brief 
 */

#include "global.h" // Contains global variables and definitions
#include "errors.h" // Contains error handling functions, variables, and definitions
#include "nav.h" // Contains GPS and navigation information for logging

#define ARDUINOJSON_ENABLE_PROGMEM 0 // Prevent errors when trying to use PROGMEM
#include <ArduinoJson.h>
#include <Teensy4Time.h>
#include <SPI.h>
#include <SdFat.h>
SdFat32 SD;

// TODO: Check the amount of memory these take:
File32 missionProfile;
File32 missionLog;
File32 dataLog;

// This variable is initiated with 100KB in RAM2
DMAMEM StaticJsonDocument<100000> json; // Nested array to hold json contents when reading and writing to SD card

bool loadMissionProfile(short profileIndex) {
	String path = PROFILE_DIR + PROFILE_FILENAME + profileIndex + FILE_EXT;
	SerialDebug.println(path);
	missionProfile = SD.open(path, FILE_READ);
	
	if (!missionProfile) { // Check if the file opened
		SerialDebug.println("error opening '" + path + "'");
		return false; // Return false if there is an error
	}

	// Deserialize the JSON document
	DeserializationError error = deserializeJson(json, missionProfile);
	if (error) { // Test if parsing succeeds.
		SerialDebug.print(F("deserializeJson() failed: "));
		SerialDebug.println(error.c_str());
		return false; // Return false if there is an error
	}

	for(ushort i=0; i < sizeof(json["waypoints"]); i++) {
		// Waypoints[i] = Waypoint {.lat = json["waypoints"][i][0], .lon = json["waypoints"][i][1], .radius = json["waypoints"][i][2], .changeMode = json["waypoints"][i][3]};
		Waypoints[i] = Waypoint {.lat = json["waypoints"][i][0], .lon = json["waypoints"][i][1], .radius = json["waypoints"][i][2]};
	}
	json.clear();
	return true; 
}

// bool fetchMissionLog(ushort fileIndex) {
// 	String path = LOG_DIR + LOG_FILENAME + fileIndex + FILE_EXT;
// 	missionLog = SD.open(path, FILE_READ);
	
// 	if (!missionLog) { // Check if the file opened
// 		SerialDebug.println("error opening '" + path + "'");
// 		return false; // Return false if there is an error
// 	}

// 	// Deserialize the JSON document
// 	DeserializationError error = deserializeJson(json, missionLog);
// 	json.clear();
// 	if (error) { // Test if parsing succeeds.
// 		SerialDebug.print(F("deserializeJson() failed: "));
// 		SerialDebug.println(error.f_str());
// 		return false; // Return false if there is an error
// 	}

// 	return true; 
// }

void UpdateMissionLog(){
	short index = 0;
	while(true){ // Find the last log file. If non exist, create one
		if(!SD.exists(LOG_DIR+LOG_FILENAME+index+FILE_EXT)){ // Check if log file exists
			if(index == 0){
				missionLog = SD.open(LOG_DIR+LOG_FILENAME+0+FILE_EXT, FILE_WRITE);	// Create and open new log file
				missionLog.close();																	// Close log file
				break;
			} else {
				index--; // Move back to the last log file that existed
				break;
			}
		}
		index++; // Move to next log file
	}

	JsonObject object = json.createNestedObject();
	object["datetime"] = day() + '/' + month() + '/' + year() + ' ' + hour() + ':' + minute() + ':' + second();

	JsonObject jsonGPS = object.createNestedObject("gps");
	if (gps.date.isValid() && gps.time.isValid()) {
		String datetime = String(gps.date.day()) + '/' + String(gps.date.month()) + '/' + String(gps.date.year()) + ' ' + String(gps.time.hour()) + ':' + String(gps.time.minute()) + ':' + String(gps.time.second());
		jsonGPS["datetime"] = datetime;
	} else {
		jsonGPS["datetime"] = "xx/xx/xxxx xx:xx:xx";
	}
	jsonGPS["fix"] = GPSFix.value();
	jsonGPS["lat"] = CurrLat;
	jsonGPS["long"] = CurrLong;
	jsonGPS["heading"] = Heading;
	jsonGPS["bearing"] = Bearing;

	String objectString;
	serializeJson(json, objectString); // Serialize the new json document and save it to a string so we can read the length

	// Check the log file size and the size of log entry we want to add. If they are above 100KB then create a new file
	missionLog = SD.open(LOG_DIR+LOG_FILENAME+index+FILE_EXT, FILE_WRITE);
	if (missionLog.size() + objectString.length() >= 100000) {
		missionLog.close();
		index++; // Move to next log file
		missionLog = SD.open(LOG_DIR+LOG_FILENAME+index+FILE_EXT, FILE_WRITE);
	}

	// TODO: check if it is the first entry before adding comma and remove the extra [] around each entry
	missionLog.print(','); // Add comma before next entry
	serializeJson(json, missionLog);
	json.clear();
	missionLog.close();
	GetHeading();
	if (DEBUG_LOGDUMP) { // Activate in 'global.h'
		SerialDebug.print("Date/Time: ");
		SerialDebug.print(year(), DEC);
		SerialDebug.print('/');
		SerialDebug.print(month(), DEC);
		SerialDebug.print('/');
		SerialDebug.print(day(), DEC);
		SerialDebug.print(" ");
		SerialDebug.print(hour(), DEC);
		SerialDebug.print(':');
		SerialDebug.print(minute(), DEC);
		SerialDebug.print(':');
		SerialDebug.print(second(), DEC);
		if (gps.date.isValid()) {
			SerialDebug.print("	GPS Date: ");
			SerialDebug.print(gps.date.year());
			SerialDebug.print('/');
			SerialDebug.print(gps.date.month());
			SerialDebug.print('/');
			SerialDebug.print(gps.date.day());
			// SerialDebug.print(" ");
		} else {
			SerialDebug.print("	GPS Date: ");
			SerialDebug.print("xx");
			SerialDebug.print(':');
			SerialDebug.print("xx");
			SerialDebug.print(':');
			SerialDebug.print("xx");
		}
		if (gps.time.isValid()) {
			SerialDebug.print("	GPS Time: ");
			SerialDebug.print(gps.time.hour());
			SerialDebug.print(':');
			SerialDebug.print(gps.time.minute());
			SerialDebug.print(':');
			SerialDebug.print(gps.time.second());
		} else {
			SerialDebug.print("	GPS Time: ");
			SerialDebug.print("xx");
			SerialDebug.print(':');
			SerialDebug.print("xx");
			SerialDebug.print(':');
			SerialDebug.print("xx");
		}
		SerialDebug.print("	GPS Fix: ");
		SerialDebug.print(GPSFix.value());
		SerialDebug.print("	Lat/Long: ");
		SerialDebug.print(CurrLat,6);
		SerialDebug.print(", ");
		SerialDebug.print(CurrLong,6);
		SerialDebug.print("	Mode: ");
		SerialDebug.print(Mode);
		SerialDebug.print("	Heading: ");
		SerialDebug.print(Heading);
		SerialDebug.print("	Bearing: ");
		SerialDebug.print(Bearing);
		SerialDebug.print("	Relative Bearing: ");
		SerialDebug.print(RelativeBearing);
		SerialDebug.print("	Throttle PWM: ");
		SerialDebug.print(ThrottlePWM);
		SerialDebug.print("	Rudder PWM: ");
		SerialDebug.print(RudderPWM);
		SerialDebug.println();
	}
}
