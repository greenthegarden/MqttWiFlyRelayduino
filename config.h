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
const byte ledPin                 = 13;
#endif


// wiFly Serial pins
const byte wiflySerialRx          = 11;
const byte wiflySerialTx          = 12;


// Network configuration
#include "networkConfig.h"


// WiFly parameters
boolean wifly_connected           = false;
const int AFTER_ERROR_DELAY       = 1000;


// MQTT parameters
byte mqtt_server_addr[]           = { 192, 168, 1, 55 };    // Pi
const int mqtt_port               = 1883;
char mqtt_client_id[]             = "irrigation";
#define MQTT_MAX_PACKET_SIZE        168
#define MQTT_KEEPALIVE              300


// time parameters
// Offset hours from gps time (UTC)
const byte TZ_OFFSET_HOURS        = 9;  // Australian CST + 30 MINS (+10 for DST)
const byte TZ_OFFSET_MINUTES      = 30;
bool DST                          = false;


#define USE_MASTER_RELAY            true

const byte MAX_ALARMS  = 10;
byte alarm_refs[MAX_ALARMS];
byte alarm_refs_cnt               = 0;

const char COMMAND_SEPARATOR      = ':';

// default times (minutes)
int R1_DURATION             = 30;    // front grass
int R2_DURATION             = 60;    // shrubs
int R3_DURATION             = 180;   // trees
int R4_DURATION             = 15;    // vegetables


const byte BUFFER_SIZE            = 32;
char char_buffer[BUFFER_SIZE];
char prog_buffer[BUFFER_SIZE];
char message[BUFFER_SIZE];

//topics
const char wifly_status[]      PROGMEM = "relayduino/status/wifly";
const char time_status[]       PROGMEM = "relayduino/status/time";
const char memory_status[]     PROGMEM = "relayduino/status/memory";
const char relay_on_status[]   PROGMEM = "relayduino/status/relay_on";
const char relay_off_status[]  PROGMEM = "relayduino/status/relay_off";
const char alarm_status[]      PROGMEM = "relayduino/status/alarm";
const char alarms_status[]     PROGMEM = "relayduino/status/alarms";
const char durations_status[]  PROGMEM = "relayduino/status/durations";

const char time_request[]      PROGMEM = "relayduino/request/time";
const char state_request[]     PROGMEM = "relayduino/request/relay_state";
const char duration_request[]  PROGMEM = "relayduino/request/durations";

const char timer_stop[]        PROGMEM = "relayduino/control/stop";
const char alarms_control[]    PROGMEM = "relayduino/control/alarms";
const char relay_control[]     PROGMEM = "relayduino/control/relay";
const char duration_control[]  PROGMEM = "relayduino/control/duration";

const char dst_set[]           PROGMEM = "all/control/dst";

// tables to refer to strings
PGM_P const status_topics[]    PROGMEM = {wifly_status,     // idx = 0
                                          time_status,      // idx = 1
                                          memory_status,    // idx = 2
                                          relay_on_status,  // idx = 3
                                          relay_off_status, // idx = 4
                                          alarm_status,     // idx = 5
                                          alarms_status,    // idx = 6
                                          durations_status, // idx = 7
                                          };

PGM_P const control_topics[]   PROGMEM = {dst_set,          // idx = 0
                                          time_request,     // idx = 1
                                          state_request,    // idx = 2
                                          duration_request, // idx = 3
                                          timer_stop,       // idx = 4
                                          alarms_control,   // idx = 5
                                          relay_control,    // idx = 6
                                          duration_control, // idx = 7
                                          };      


#endif

