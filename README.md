# MqttWiFlyRelayduino
Control and monitor a Relayduino via MQTT and WiFly module

Repository contains both Arduino code which is used to operate a [Relayduino] (https://oceancontrols.com.au/KTA-223.html) using [MQTT] (http://mqtt.org) through a [RN-XV WiFly Module] (https://www.sparkfun.com/products/10822) and Python code which is run on a Raspberry Pi to control and monitor the Relayduino.

I use the Relayduino as an irrigation controller which controls a master valve and four valves each controlling an irrigation zone within our garden.

The following libraries are required which are not included with the Arduino IDE to support the Arduino code:

* WiFly-MQTT from https://github.com/lagoudiana/Wifly-MQTT/tree/master/Arduino-wifly%20MQTT
* Time from https://www.pjrc.com/teensy/td_libs_Time.html
* TimeAlarms from https://www.pjrc.com/teensy/td_libs_TimeAlarms.html
* MemoryFree from http://playground.arduino.cc/code/AvailableMemory

The [Mosquitto MQTT Broker] (http://mosquitto.org/) is run on a Raspberry Pi and the Python code implemented using a the [Paho Python Client] (https://www.eclipse.org/paho/clients/python/).
