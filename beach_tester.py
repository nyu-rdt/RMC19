#!/usr/bin/python3

import paho.mqtt.client as mqtt
import time
import sys

assert(len(sys.argv) >= 2)
motors = ['motors/a', 'motors/b', 'linearActuator/a', 'linearActuator/b']


m = motors[int(sys.argv[1])]
power = int((int(sys.argv[2])+100)/200 * 255)
client = mqtt.Client()
client.connect('127.0.0.1')

client.publish("digging/{}".format(m), payload=power, qos=0, retain=False)
print("motor: {} power: {}".format(m, power))
if (int(sys.argv[3]) == 1):
    time.sleep(0.5)
    client.publish("digging/{}".format(m), payload=0x7F, qos=0, retain=False)
    print("stopping")
