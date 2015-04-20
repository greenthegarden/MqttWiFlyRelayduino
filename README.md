# MqttWiFlyRelayduino
Control and monitor a Relayduino via MQTT and WiFly module

Code to control and monitor a [Relayduino] (https://oceancontrols.com.au/KTA-223.html) using [MQTT] (http://mqtt.org) through a [RN-XV WiFly Module] (https://www.sparkfun.com/products/10822).

The following libraries are required which are not included with the Arduino IDE:

* WiFly-MQTT from https://github.com/lagoudiana/Wifly-MQTT/tree/master/Arduino-wifly%20MQTT
* Time from https://www.pjrc.com/teensy/td_libs_Time.html
* TimeAlarms from https://www.pjrc.com/teensy/td_libs_TimeAlarms.html
* MemoryFree from http://playground.arduino.cc/code/AvailableMemory

I use the Relayduino as an irrigation controller which control master valve and four valves. The system operators automatically via alarms (based on TimeAlarms). In addition, the relays can controlled via the following MQTT topics:

all/control/dst: if 
                                          time_request,     // idx = 1
                                          state_request,    // idx = 2
                                          duration_request, // idx = 3
                                          timer_stop,       // idx = 4
                                          alarms_control,   // idx = 5
                                          relay_control,    // idx = 6
                                          duration_control
