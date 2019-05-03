#!/usr/bin/python3

import paho.mqtt.client as mqtt
import time
import sys

assert(len(sys.argv) >= 2)
motors = ['', 'motors/a', 'motors/b', 'linearActuator/a', 'linearActuator/b']
sensors = ['', 'motors/a', 'motors/b', 'linearActuator/a', 'linearActuator/b']

laspeed = int(sys.argv[1])

m1 = 'motors/a'
m2 = 'motors/b'
s1 = sensors[1]
m3 = motors[3]
#power = int((int(sys.argv[2])+100)/200 * 255)
power = int(sys.argv[2])
client = mqtt.Client()
client.connect('127.0.0.1')

client.publish("digging/{}".format(s1), payload=1, qos=0, retain=False)
#client.publish("digging/{}".format(m2), payload=power, qos=0, retain=False)
#client.publish("digging/{}".format(m3), payload=laspeed, qos=0, retain=False)
# time.sleep(0.01)


