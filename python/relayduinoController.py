#!/usr/bin/env python

import paho.mqtt.client as mqtt
#import paho.mqtt.publish as publish

import os, urlparse
import time

hostname = "localhost"

qos = 2
topics = []
topics.append(("relayduino/status/#", 0))
topics.append(("relayduino/request/#", 0))
topics.append(("relayduino/control/#", 0))

# MQTT Callbacks

def on_connect(client, userdata, flags, rc) :
	mqttc.subscribe(topics)

def on_disconnect(client, userdata, rc) :
	print("Disconnect returned:")
	print("client: {0}".format(str(client)))
	print("userdata: {0}".format(str(userdata)))
	print("result: {0}".format(str(rc)))

def on_message(client, userdata, message) :
#    print(time.clock())
	print(message.topic+" "+str(message.payload))
#    print("message received: topic is {0} with payload {1}".format(message.topic, message.payload))

def on_publish(client, userdata, mid) :
    print("mid: {0}".format(str(mid)))

def on_subscribe(client, userdata, mid, granted_qos) :
    print("Subscribed: "+str(mid)+" "+str(granted_qos))

def on_log(client, userdata, level, buf) :
    print("{0}".format(buf))

mqttc               = mqtt.Client()
mqttc.on_connect    = on_connect
mqttc.on_message    = on_message
#mqttc.on_publish    = on_publish
mqttc.on_subscribe  = on_subscribe
mqttc.on_disconnect = on_disconnect
# Uncomment to enable debug messages
#mqttc.on_log       = on_log

# Parse CLOUDMQTT_URL (or fallback to localhost)
url_str = os.environ.get('CLOUDMQTT_URL', 'mqtt://localhost:1883')
url = urlparse.urlparse(url_str)

# Connect
#mqttc.username_pw_set(url.username, url.password)
mqttc.connect(url.hostname, url.port)
mqttc.loop_start()

command_types_valid_inputs = ['m', 'w', 'c', 'e', 't', 'd', 'r', 's', 'o', 'l']
dst_command_valid_inputs   = ['0', '1']
alarms_command_valid_inputs   = ['0', '1']


print("Enter e for enable, t for timezone, d for time, r for relay, or s for relay state ")

# Loop continuously
while True :

	try :

		command_type = raw_input()

		if command_type in command_types_valid_inputs :

			if command_type is 'w' :
				mqttc.publish("relayduino/request/wifly");

			if command_type is 'm' :
				mqttc.publish("relayduino/request/memory");

			if command_type is 'e' :

				alarms_command = raw_input("Enter 0/1 to disable/enable alarms: ")

				if alarms_command in alarms_command_valid_inputs :
					mqttc.publish("relayduino/control/alarms", alarms_command)

			if command_type is 't' :

				dst_command = raw_input("Enter 0 to set standard time or 1 for summer time: ");

				if dst_command in dst_command_valid_inputs :
					mqttc.publish("all/control/dst", dst_command)

			elif command_type is 'c' :

         		relay, duration = raw_input("Enter relay and duration: ").split()

				message = relay + ":" + duration

				mqttc.publish("relayduino/control/duration", message)

			elif command_type is 'd' :
				mqttc.publish("relayduino/request/time")

			elif command_type is 's' :
				mqttc.publish("relayduino/request/relay_state")

			elif command_type is 'l' :
				mqttc.publish("relayduino/request/durations")

			elif command_type is 'o' :
				mqttc.publish("relayduino/control/stop");

			elif command_type is 'r' :
				relay, duration = raw_input("Enter relay and duration: ").split()

				#relay, duration = [int(relay), int(duration)]
				message = relay + ":" + duration

				mqttc.publish("relayduino/control/relay", message)

			else :
				print("Should not be here!!")

		else :
			print("Invalid command type entered")


	except KeyboardInterrupt :      #Triggered by pressing Ctrl+C

		running = False       #Stop thread1

		# Disconnect mqtt client
		mqttc.loop_stop()
		mqttc.disconnect()

		print("Bye")
		break         #Exit
