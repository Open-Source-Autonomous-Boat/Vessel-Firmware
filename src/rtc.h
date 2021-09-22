#include <RTClib.h>
RTC_DS3231 rtc;
DateTime now;

//===Global Date/Time Variables===
// These store the most recent date/time from the RTC module
// Note: You can only access these variables from this file and the main .ino file

// Raw date/time
int Year; // This must contain a 4 digit year
byte Month, Day, Hour, Minute, Second, Hundredths;

// Formated date/time sentences (Strings)
String Datetime;
String Date;
String Time;


// Inputs the day, month, and year and outputs a String formated 'dd:mm:yyyy'
String formatTime(String _day, String _month, String _year) {
	String _return;
	_return = _day + "/" + _month + "/" + _year ;
	return _return;
}

// Inputs the hours, minutes, and seconds and outputs a String formated 'hh:mm:ss'
String formatDate(String _hour, String _minute, String _second){
	String _return;
	_return = _hour + ":" + _minute + ":" + _second;
	return _return;
}

void UpdateClock(int _year, byte _month, byte _day, byte _hour, byte _minute, byte _second ) {
	rtc.adjust(DateTime(_year, _month, _day, _hour, _minute, _second));
}
