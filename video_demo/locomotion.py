#/usr/bin/python3

import paho.mqtt.client as mqtt
import time
import sys

assert(len(sys.argv) >= 3)

turn = ['direction/forward', 'turn/left', 'turn/right'
power = int(sys.argv[2])
direction = turn[int(sys.argv[1])]

client = mqtt.Client()
client.connect('127.0.0.1')

client.publish('locomotion/{]'.format(direction), payload=power, qos=0, retain=False)
print("Direction: {}, Power {}".format(direction, power))
