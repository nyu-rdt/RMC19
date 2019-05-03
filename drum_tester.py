#!/usr/bin/python3

import paho.mqtt.client as mqtt
from time import sleep
import sys

assert(len(sys.argv) >= 2)
motors = ['', 'motors/a', 'motors/b', 'linearActuator/a', 'linearActuator/b']

laspeed = int(sys.argv[1])

m1 = 'motors/a'
m2 = 'motors/b'
m3 = motors[3]
#power = int((int(sys.argv[2])+100)/200 * 255)
power = int(sys.argv[2])
client = mqtt.Client()
client.connect('127.0.0.1')

client.publish("digging/{}".format(m1), payload=230, qos=0, retain=False)
client.publish("digging/{}".format(m2), payload=230, qos=0, retain=False)
sleep(1)

while True:
	sleep(6)
	client.publish("digging/{}".format(m3), payload=65, qos=0, retain=False)
	sleep(0.5)
	client.publish("digging/{}".format(m3), payload=0, qos=0, retain=False)


# time.sleep(0.01)
print("motor: {} power: {}".format(m1, power))
print("motor: {} power: {}".format(m2, power))
print("motor: {} power: {}".format(m3, laspeed))

if (len(sys.argv) > 2):
    if (float(sys.argv[3]) > 0):
        time.sleep(float(sys.argv[3]))
        client.publish("digging/{}".format(m1), payload=0x00, qos=0, retain=False)
        client.publish("digging/{}".format(m2), payload=0x00, qos=0, retain=False)
        client.publish("digging/{}".format(m3), payload=0x00, qos=0, retain=False)
        print("stopping")
