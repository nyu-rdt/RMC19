#!/usr/bin/python3

import paho.mqtt.client as mqtt
import time
import sys

action_f = 'direction/forward'
action_l = 'turn/left'
action_r = 'turn/right'

client = mqtt.Client()
client.connect('127.0.0.1')

# Action to be tested: arg 1
action_test = ""
if sys.argv[1] == 'f':
    action_test = action_f
elif sys.argv[1] == 'l':
    action_test = action_l
elif sys.argv[1] == 'r':
    action_test = action_r

# Power: arg 2
power = int(sys.argv[2])
# Time: arg 3
time_run = float(sys.argv[3])
    
client.publish("locomotion/{}".format(action_test), payload=power, qos=0, retain=False)
time.sleep(time_run)
client.publish("locomotion/{}".format(action_test), payload=0, qos=0, retain=False)
