#!/usr/bin/python3

import paho.mqtt.client as mqtt
import time
import sys

assert(len(sys.argv) >= 2)
motors = ['', 'motors/a', 'motors/b', 'linearActuator/a', 'linearActuator/b']


m1 = 'linearActuator/a'
m2 = 'linearActuator/b'
#power = int((int(sys.argv[2])+100)/200 * 255)
power = int(sys.argv[2])
client = mqtt.Client()
client.connect('127.0.0.1')

client.publish("digging/{}".format(m1), payload=power, qos=0, retain=False)
#client.publish("digging/{}".format(m2), payload=power, qos=0, retain=False)
print("motor: {} power: {}".format(m1, power))
#print("motor: {} power: {}".format(m2, power))
if (len(sys.argv) >3):
    if (int(sys.argv[3]) == 1):
        time.sleep(0.5)
        client.publish("digging/{}".format(m1), payload=0x00, qos=0, retain=False)
        #client.publish("digging/{}".format(m2), payload=0x00, qos=0, retain=False)
        print("stopping")
