#ifndef __CONFIG_H__
#define __CONFIG_H__


#define DEBUG                       true
#define DEBUG2                      false
#define USE_WATCHDOG                false
#define USE_FREEMEM                 false
#define USE_LED                     true

// Serial parameters
const int BAUD_RATE               = 9600;


#if USE_WATCHDOG
//#define WDT_TIME                  WDTO_2S
#define WDT_TIME                    WDTO_8S
#endif


#if USE_LED
const byte LED_PIN                = 13;
#endif


// wiFly Serial pins
const byte WIFLY_SERIAL_RX        = 11;
const byte WIFLY_SERIAL_TX        = 12;


// Network configuration
#include "networkConfig.h"


// WiFly parameters
boolean wifly_connected           = false;
const int AFTER_ERROR_DELAY       = 1000;


// MQTT parameters
byte mqtt_server_addr[]           = { 192, 168, 1, 55 };    // Pi
char mqtt_client_id[]             = "irrigation";
const int MQTT_PORT               = 1883;
#define MQTT_MAX_PACKET_SIZE        168
#define MQTT_KEEPALIVE              300


// time parameters
// Offset hours from gps time (UTC)
const byte TZ_OFFSET_HOURS        = 9;  // Australian CST + 30 MINS (+10 for DST)
const byte TZ_OFFSET_MINUTES      = 30;
bool daylight_summer_time         = false;


#define USE_MASTER_RELAY            true

const byte MAX_ALARMS             = 10;
byte alarm_refs[MAX_ALARMS];
byte alarm_refs_cnt               = 0;

const char COMMAND_SEPARATOR      = ':';

// default times (minutes)
int relay_1_duration              = 30;    // front grass
int relay_2_duration              = 60;    // shrubs
int relay_3_duration              = 180;   // trees
int relay_4_duration              = 15;    // vegetables


const byte BUFFER_SIZE            = 32;
char char_buffer[BUFFER_SIZE];
char prog_buffer[BUFFER_SIZE];
char message[BUFFER_SIZE];

const char WIFLY_STATUS[]      PROGMEM = "relayduino/status/wifly";
const char TIME_STATUS[]       PROGMEM = "relayduino/status/time";
const char MEMORY_STATUS[]     PROGMEM = "relayduino/status/memory";
const char RELAY_ON_STATUS[]   PROGMEM = "relayduino/status/relay_on";
const char RELAY_OFF_STATUS[]  PROGMEM = "relayduino/status/relay_off";
const char ALARM_STATUS[]      PROGMEM = "relayduino/status/alarm";
const char ALARMS_STATUS[]     PROGMEM = "relayduino/status/alarms";
const char DURATIONS_STATUS[]  PROGMEM = "relayduino/status/durations";

PGM_P const STATUS_TOPICS[]    PROGMEM = {WIFLY_STATUS,     // idx = 0
                                          TIME_STATUS,      // idx = 1
                                          MEMORY_STATUS,    // idx = 2
                                          RELAY_ON_STATUS,  // idx = 3
                                          RELAY_OFF_STATUS, // idx = 4
                                          ALARM_STATUS,     // idx = 5
                                          ALARMS_STATUS,    // idx = 6
                                          DURATIONS_STATUS, // idx = 7
                                          };
                                          
const char DST_SET[]           PROGMEM = "all/control/dst";

const char TIME_REQUEST[]      PROGMEM = "relayduino/request/time";
const char STATE_REQUEST[]     PROGMEM = "relayduino/request/relay_state";
const char DURATION_REQUEST[]  PROGMEM = "relayduino/request/durations";

const char TIMER_STOP[]        PROGMEM = "relayduino/control/stop";
const char ALARMS_CONTROL[]    PROGMEM = "relayduino/control/alarms";
const char RELAY_CONTROL[]     PROGMEM = "relayduino/control/relay";
const char DURATION_CONTROL[]  PROGMEM = "relayduino/control/duration";

PGM_P const CONTROL_TOPICS[]   PROGMEM = {DST_SET,          // idx = 0
                                          TIME_REQUEST,     // idx = 1
                                          STATE_REQUEST,    // idx = 2
                                          DURATION_REQUEST, // idx = 3
                                          TIMER_STOP,       // idx = 4
                                          ALARMS_CONTROL,   // idx = 5
                                          RELAY_CONTROL,    // idx = 6
                                          DURATION_CONTROL, // idx = 7
                                          };      


#endif

