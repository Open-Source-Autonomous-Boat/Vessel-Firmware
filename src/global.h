/**
 * @file global.h
 * @author Micahel MacDonald (@Michael2MacDonald)
 * @brief 
 * 
 * 
 * 
 */

#ifndef GLOBAL_H_
#define GLOBAL_H_

#include <Arduino.h>
#include <VIS_Tools.h> // Our Custom Lib

/** NOTES: 
 * - Because the Teensy 4.x boards are 32 bit we use the 'short' data type so that int's will always be 16 bits
 * - All GPS coordanates use floats
 */

/**==============================================
 *                Global Definitions
 * ==============================================**/

#define ENABLED 1
#define DISABLED 0
#define ENABLE ENABLED
#define DISABLE DISABLED
#define uint unsigned int     // abbreviation for unsigned int
#define ushort unsigned short // abbreviation for unsigned short

// Array Sizes Used For Array Initialization (Use these to set the max number of objects each array can hold)
#define MAX_NUM_WAYPOINTS 100     // Number of Mission Waypoints
#define MAX_NUM_CMD 100           // Number of commands (Total number of commands in VIS)
#define MAX_NUM_QUEUE_CMD 100     // Number of commands in queue
#define MAX_NUM_CMD_PRESETS 20    // Number of saved commands for waypoints to use (Used for array creation)
#define MAX_NUM_CMD_PARAM 6       // Max Number of parameters a command can have
#define MAX_NUM_ERROR 50          // Number of error codes

#define MODE_HOLD 0 // Hold
#define MODE_WP_BEARING 1 // Waypoint Bearing
#define MODE_WP_COURSE 2 // Waypoint Course
#define MODE_HEADING 3 // Heading
#define MODE_LIOTER 4 // Lioter
#define MODE_HOME 5 // Return Home
#define MODE_ABORT 6 // Abort
#define MODE_LP 7 // Low Power

// Currently the serial ports we are using have not been decided so some of the comments might be off
#define gpsSerial Serial5       // Hardware serial port 2. This is the same as setting the serial pins to 7 and 8 on the Teensy 4.1
#define USBSerial Serial        // Serial port for USB
#define BluetoothSerial Serial1 // Serial port for ESP32 (Serial over Bluetooth via ESP32)
// Uncomment to use USB serial port:
#define SerialDebug USBSerial   // Serial port to use for serial outputs, debug messages, and error messages
#define SerialCMD USBSerial     // Serial port to use for serial outputs, debug messages, and error messages
// Uncomment to use Bluetooth serial port:
// #define SerialDebug BluetoothSerial // Serial port to use for serial outputs, debug messages, and error messages
// #define SerialCMD BluetoothSerial   // Serial port to use for serial outputs, debug messages, and error messages

// Turn On/Off Debug Messages
// TODO: Maybe implement this in the future https://andrewsleigh.com/fab-slider/coding-better-debugging/
#define DEBUG_LOADED_WAYPOINTS 1    // Output the loaded waypoints on startup
#define DEBUG_LOGDUMP 1             // Output formatted navigation and system variables like time, mode, and location
#define DEBUG_RAW_GPS 0             // Output the raw GPS sentences
#define DEBUG_RAW_IMU 0             // Output raw IMU readings


/**==============================================
 *                Global Variables
 * ==============================================**/


bool TestingMode = true;  // Enables testing functions 
bool FirstStartup = true; // First time the vessel has started (In case of reboots during missions)


/**======================
 *    Communication
 * ======================**/

String PacketHeader; // Holds Incoming Packet Header
String PacketText; // Holds Incoming Packet Text


/**======================
 *       Control
 * ======================**/

/** TODO:
 * Name the entire command to Command ("W1,7,43.627,122.891,40;")
 * Name only the command 'id' (W1,D2,etc) to Instruction
 */


// struct Command { // Holds commands and their parameters to be executed
// 	String cmd; // Command name
// 	String params[MAX_CMD_PARAM]; // Command parameters
// };
// Command Commands[NUM_QUEUE_CMD]; /** TODO: Create array size option (var/define) */
// Command CommandPresets[NUM_CMD_PRESETS];

VISCommand Commands[MAX_NUM_QUEUE_CMD];
VISCommand CommandPresets[MAX_NUM_CMD_PRESETS];

struct VISLookupTable { // Lookup table for executing commands
	String instr;             // Instruction name
	int (*funcPtr)(VISCommand); // Pointer to instruction function
} VISLut[MAX_NUM_CMD];
// VISLookupTable VISLut[MAX_CMD_PARAM];



/**======================
 *    Navigation & GPS
 * ======================**/

struct euler_t { // Holds Euler Yaw, Pitch, And Roll For BNO085
	float yaw;
	float pitch;
	float roll;
} ypr;

struct Waypoint {							// Struct to hold a waypoint
	float lat;
	float lon;
	ushort radius;
	// short changeMode; // Replaced by cmd2
	short cmd1; // Index of function to be executed when waypoint is set as target
	short cmd2; // Index of function to be executed when waypoint is reached
};

Waypoint Waypoints[MAX_NUM_WAYPOINTS]; // Initialize struct array to hold mission waypoints
// PROGMEM const Waypoint Abort = {.lat = 45.212018, .lon = -123.972962, .radius = 20, .changeMode = 3};
// PROGMEM const Waypoint Home = {.lat = 45.212018, .lon = -123.972962, .radius = 20, .changeMode = 3};
PROGMEM const Waypoint Abort = {.lat = 45.212018, .lon = -123.972962, .radius = 20};
PROGMEM const Waypoint Home = {.lat = 45.212018, .lon = -123.972962, .radius = 20};

short Mode = 1;							// Vessel's mode of operation. Initialize as 0
/** MODE:
 * 0 = Hold: Do nothing (Drift)
 * 1 = Waypoint Bearing: Tries to keep its heading pointed toward the target waypoint
 * 2 = Waypoint Course: Tries to follow the line between the previous waypoint and the target waypoint
 * 3 = Heading: Tries to keep its heading equal to a given angle relative to north (TODO: Check if we are using true north or magnetic north)
 * 4 = Loiter: Holds (does nothing) while within the radius of the target waypoint. If it drifts out of the radius it rotates towards the waypoint or directly away from it (whichever results in less rotation) and drives/floats forwards/backwards (at 0.5 m/s * the distance to the edge of the target radius)
 * 5 = Return Home
 * 6 = Abort
 * 7 = Low Power
 */
bool Fix = false;								// Set to true when the GPS fix is >0
/** TODO: Check if we use true north or magnetic north in our calculations */
// All angles are relative to north unless otherwise stated
short Course;								// The angle between the previous waypoint and the target waypoint
short Bearing;								// The angle between the vessel and the target waypoint
short RelativeBearing;					// The angle between the heading and the bearing
short Heading;								// The angle the vessel is facing
short Track;								// The angle the vessel actually travels over the ground

short Target;								// Target Waypoint: The index of the waypoint that the vessel is currently trying to reach
float CurrLat, CurrLong;				// The vessels current Lat and Long
short WaypointRadius;					// How close the vessel has to get to the target waypoint to register as reaching it (In meters)

/**======================
 *    Motor And Rudder
 * ======================**/

short RudderPWM;							// Holds the current servo PWM value
short RudderTrim = 100;					// Tune this to the PWM value of the servo when straight
short RudderRange = 40;					// Max/Min PWM rudder value == RudderTrim +/- RudderRange
const short RudderMinPW = 1000;		// Sets the min PWM pulse width
const short RudderMaxPW = 2000;		// Sets the max PWM pulse width
short RudderPos;							// Holds the current servo position in real life
short RudderMinPos = 1000;				// Minimum allowed rudder position
short RudderMaxPos = 2000;				// Maximum allowed rudder position

short ThrottlePWM;						// Holds the current throttle PWM value
short ThrottleMin = 15;					// TODO: Set/implement later
short ThrottleMax = 15;					// TODO: Set/implement later
short ThrottleOff = 0;					// PWM value for no throttle
short MotorRPM;							// Holds the current motor RPM value
short MaxMotorRPM;						// Maximum allowed motor RPM

/**======================
 *    Date And Time
 * ======================**/

// TODO: Implement DateTime structs
// Raw date/time
struct datetime { // Struct to hold date/time vars (Lowercase to avoid conflicts with classes from other libs)
	short Year; // This must contain a 4 digit year
	byte Month;
	byte Day;
	byte Hour;
	byte Minute;
	byte Second;
	byte Hundredths; // Do we need this?
	String Date; // Formated date string
	String Time; // Formated time string
};
datetime Datetime; // Initialize struct to hold RTC date/time
datetime DatetimeGPS; // Initialize struct to hold RTC date/time


/**==============================================
 *            Global Conf Variables
 * ==============================================**/

/**======================
 *    Files and SD Card
 * ======================**/

int MISSION_LOG_UPDATE_INTREVAL = 1000; /** TODO: Make EXTMEM */ // Every 1 Second

// struct MissionLogEntry {
// 	float lat;
// 	float lon;
// 	short radius;
// 	short changeMode;
// 	// int action;
// };
// MissionLogEntry MissionLog[100] = {'\0'};	// Initialize struct array to hold mission log and fill with NULL

// struct DataLogEntry {
// 	float lat;
// 	float lon;
// 	short radius;
// 	short changeMode;
// 	// int action;
// };
// DataLogEntry DataLog[100] = {'\0'};	// Initialize struct array to hold data log and fill with NULL

/**
 * Filepaths should be formatted like this: PROFILE_DIR + PROFILE_FILENAME + File Index + FILE_EXT
 * The result should look something like this: '/MissionProfiles/missionProfile6.json'
 */
String FILE_EXT = ".json"; // File extention

String PROFILE_DIR = "MissionProfiles/";	// Root directory for mission profile files
String LOG_DIR = "MissionLogs/";					// Root directory for mission log files
String DLOG_DIR = "DataLogs/";							// Root directory for data log files

String PROFILE_FILENAME = "MissionProfile";	// Root path for mission profile files
String LOG_FILENAME = "MissionLog";					// Root path for mission log files
String DLOG_FILENAME = "DataLog";							// Root path for data log files

// TODO: Hardcode Default and Abort mission profiles??
// Make costant?
short MISSION_ABORT_HOME = 0;	// Index of 'Abort Mission' mission profile (Abort to mission launch location)
short MISSION_ABORT_NEAREST = 1;	// Index of 'Abort Mission' mission profile (Abort to nearest abort location)
short MISSION_ABORT_ONE = 2;	// Index of 'Abort Mission' mission profile (Abort to abort location one)
short MISSION_DEFAULT = 3;	// Index of 'Default' mission profile (Used when user mission profile can't be found. Usually set to the shortest route to mission end location)
short MISSION_ONE = 4;		// Index of first user mission profile


// Go to Pacific City, Oregon and loiter in a 40 meter bubble off the coast right across from the path to our house.
// I chose this spot because there are less people there, we are familiar with it, and it is easy to reach
// Make costant?
// const String MISSION_ABORT_HOME_PROFILE = "{\"waypoints\":[\"45.212018\",\"-123.972962\",\"20\",\"3\"]}";


/**==============================================
 *                Registry Editers
 * ==============================================**/

/**
 * WARNING: This is very advanced stuff. If you don't know what you are doing then don't mess with this.
 *          While it would be very hard to damage any physical components, you can still easily make a
 *          a mistake and cause fatal system bugs that are very hard to pinpoint and can ruin a mission.
 *          All it takes is one typo or mistake to write a random value to a random memory address that
 *          the CPU uses for a critical function and you can cause bad or unexpected results.
 */

#define RESET() ((*(volatile uint32_t *)0xE000ED0C)=0x05FA0004) // Run 'RESET();' to restart the teensy

// The built-in NVRAM is for the onboard RTC but can be used to store variables
// #define NVRAM_START (SNVS_LPCR |= (1 << 24)) // Enable NVRAM - documented in SDK
// volatile uint32_t *NVRAM = (volatile uint32_t *)0x400D4100; // Create array of 4 32bit blocks at the address of the built-in nvram
// #define MASK(length, offset) (((1<<length)-1)<<offset) // Bitmask
// #define I_MASK(length, offset) 0xFFFFFFFF-MASK(length, offset) // Inverted bitmask
// #define NVRAM_READ(block, length, offset) ( (NVRAM[block]&MASK(length, offset)) >> offset )
// #define NVRAM_WRITE(block, length, offset, value) ( NVRAM[block] = ( (NVRAM[block]&(I_MASK(length, offset))) | (value << offset)) )



#endif // End GLOBAL_H_