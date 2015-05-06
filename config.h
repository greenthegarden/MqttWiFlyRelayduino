#ifndef MQTTWIFLYRELAYDUINO_CONFIG_H_
#define MQTTWIFLYRELAYDUINO_CONFIG_H_


#define DEBUG                       true
#define USE_FREEMEM                 true
#define USE_LED                     true


// Macros
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))


// Serial parameters
const int BAUD_RATE               = 9600;


#if USE_LED
const byte LED_PIN                = 13;
#endif


// Network configuration
#include "networkConfig.h"


// WiFly configuration
#include "wifly_config.h"


// MQTT configuration
#include "mqtt_config.h"


// Time configuration
#include "time_config.h"


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


#endif  /* MQTTWIFLYRELAYDUINO_CONFIG_H_ */

