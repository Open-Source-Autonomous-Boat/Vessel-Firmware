void serialLogDump(){
	SerialLog.print("Date/Time: ");
	SerialLog.print(now.year(), DEC);
	SerialLog.print('/');
	SerialLog.print(now.month(), DEC);
	SerialLog.print('/');
	SerialLog.print(now.day(), DEC);
	SerialLog.print(" ");
	SerialLog.print(now.hour(), DEC);
	SerialLog.print(':');
	SerialLog.print(now.minute(), DEC);
	SerialLog.print(':');
	SerialLog.print(now.second(), DEC);
	if (gps.date.isValid()) {
		SerialLog.print("	Date: ");
		SerialLog.print(gps.date.year());
		SerialLog.print('/');
		SerialLog.print(gps.date.month());
		SerialLog.print('/');
		SerialLog.print(gps.date.day());
		// SerialLog.print(" ");
	} else {
		SerialLog.print("	Date: ");
		SerialLog.print("xx");
		SerialLog.print(':');
		SerialLog.print("xx");
		SerialLog.print(':');
		SerialLog.print("xx");
	}
	if (gps.time.isValid()) {
		SerialLog.print("	Time: ");
		SerialLog.print(gps.time.hour());
		SerialLog.print(':');
		SerialLog.print(gps.time.minute());
		SerialLog.print(':');
		SerialLog.print(gps.time.second());
	} else {
		SerialLog.print("	Time: ");
		SerialLog.print("xx");
		SerialLog.print(':');
		SerialLog.print("xx");
		SerialLog.print(':');
		SerialLog.print("xx");
	}
	SerialLog.print("	GPS Fix: ");
	SerialLog.print(GPSFix.value());
	SerialLog.print("	Lat/Long: ");
	SerialLog.print(CurrLat,6);
	SerialLog.print(", ");
	SerialLog.print(CurrLong,6);
	SerialLog.print("	Heading: ");
	SerialLog.print(Heading);
	SerialLog.print("	Bearing:");
	SerialLog.print(Bearing);
	SerialLog.println();
}