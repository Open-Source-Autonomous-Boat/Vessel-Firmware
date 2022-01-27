/**
 * @file rtc.h
 * @author Micahel MacDonald (@Michael2MacDonald)
 * @brief Contains functions for formatting date and time into a pretty String
 */

#include <Arduino.h>

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

String formatDateTime(String _hour, String _minute, String _second, String _day, String _month, String _year){
	String _return;
	_return = formatDate(_hour, _minute, _second) + " " + formatTime(_day, _month, _year);
	return _return;
}