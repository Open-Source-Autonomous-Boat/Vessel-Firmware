#include <ArduinoJson.h>
#include <SPI.h>
#include <SdFat.h>
SdFs SD;

File missionProfile;
File missionLog;
File dataLog;

char fileCharArray[1024]; // Holds chars from SD read
StaticJsonDocument<1024> deserializedJson; // Nested array that holds the deserialized json contents from SD reads
StaticJsonDocument<1024> serializedJson; // Nested array that holds the serialized json contents for SD writes

extern struct Waypoint Waypoints[20];

bool loadMissionProfile(String path) {
	missionProfile = SD.open(path, FILE_READ);
	if (missionProfile) { // Check if the file opened
		int i;
		while(missionProfile.available()) { // read from the file until there's nothing else in it:
			fileCharArray[i] = missionProfile.read();
			i++;
		}
		missionProfile.close(); // Close file on SD card

		// Deserialize the JSON document
		DeserializationError error = deserializeJson(deserializedJson, fileCharArray);
		if (error) { // Test if parsing succeeds.
			Serial.print(F("deserializeJson() failed: "));
			Serial.println(error.f_str());
		}

		for(unsigned int i=0; i<sizeof deserializedJson["Waypoints"]; i++) {
			Waypoints[i] = Waypoint {.lat = deserializedJson["Waypoints"][i][0], .lon = deserializedJson["waypoints"][i][1]};
		}
			
		return true; 
	} else {
		Serial.println("error opening '" + path + "'");
		return false;
	}
}

void updateMissionLog(String path){
	// JsonArray datetime = serializedJson.createNestedArray("datetime");
	missionLog = SD.open(path, FILE_WRITE);
	missionLog.close();
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
