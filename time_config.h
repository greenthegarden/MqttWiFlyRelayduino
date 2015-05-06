#ifndef MQTTWIFLYRELAYDUINO_TIME_CONFIG_H_
#define MQTTWIFLYRELAYDUINO_TIME_CONFIG_H_


#include <Time.h>

// time parameters
// Offset hours from gps time (UTC)
const byte TZ_OFFSET_HOURS        = 9;  // Australian CST + 30 MINS (+10 for DST)
const byte TZ_OFFSET_MINUTES      = 30;
bool daylight_summer_time         = false;


#endif  /* MQTTWIFLYRELAYDUINO_TIME_CONFIG_H_ */

