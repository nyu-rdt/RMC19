#!/usr/bin/python3

# LINEAR ACTUATORS: 60 - 170 down  |  190 - 250 up
# DOOR: 120 open  |   250 close

# 'do' for door
# 'dr' for drum
# 'l' for linear actuators

import paho.mqtt.client as mqtt
from time import sleep

import sys

DRUM_TOPIC = 'motors/all'
LIN_ACTS_TOPIC = 'linearActuator/a'
DOOR_TOPIC = 'linearActuator/b'

client = mqtt.Client()
client.connect('127.0.0.1')

action_topic = ''
if sys.argv[1] == 'dr':
    action_topic = DRUM_TOPIC
elif sys.argv[1] == 'do':
    action_topic = DOOR_TOPIC
elif sys.argv[1] == 'l':
    action_topic = LIN_ACTS_TOPIC

power = sys.argv[2]
time = float(sys.argv[3])

client.publish("digging/{}".format(DOOR_TOPIC), payload=250, qos=0, retain=False)
client.publish("digging/{}".format(LIN_ACTS_TOPIC), payload=250, qos=0, retain=False)
sleep(1)
# client.publish("digging/{}".format(action_topic), payload=0, qos=0, retain=False)

client.publish("digging/{}".format(LIN_ACTS_TOPIC), payload=0, qos=0, retain=False)
"""
while True:
sleep(2)

client.publish("digging/{}".format(LIN_ACTS_TOPIC), payload=0, qos=0, retain=False)
    client.publish("digging/{}".format(LIN_ACTS_TOPIC), payload=100, qos=0, retain=False)
    sleep(0.1)
"""
