#!/usr/bin/python3

import paho.mqtt.client as mqtt
import time
import sys

action_m = 'motors/all'
action_l = 'linearActuator/a'
action_v = 'vibration/a'

client = mqtt.Client()
client.connect('127.0.0.1')

# Action to be tested: arg 1
action_test = ""
if sys.argv[1] == 'm':
    action_test = action_m
elif sys.argv[1] == 'l':
    action_test = action_l
elif sys.argv[1] == 'v':
    action_test = action_v

# Power: arg 2
power = int(sys.argv[2])
# Time: arg 3
time_run = float(sys.argv[3])
    
client.publish("deposition/{}".format(action_test), payload=power, qos=0, retain=False)
time.sleep(time_run)
client.publish("deposition/{}".format(action_test), payload=0, qos=0, retain=False)
