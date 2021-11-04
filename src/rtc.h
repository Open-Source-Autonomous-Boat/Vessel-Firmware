#include "global.h"

#include <RTClib.h>
RTC_DS3231 rtc;
DateTime now;

// TODO: Implement DateTime structs

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
