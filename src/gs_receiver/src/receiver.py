#!/usr/bin/env python
import socket
import os
import time
import json
import struct
import rospy

from rdt_mqtt.msg import PackedMessage

SERVER_ADDR = '0.0.0.0'
SERVER_PORT = 10010

# CLIENT_ADDR = '10.100.32.215'
# CLIENT_ADDR = '127.0.0.1'
CLIENT_ADDR = '192.168.1.104'
# CLIENT_ADDR = '192.168.1.104'
CLIENT_PORT = 10010
# CLIENT_PORT = 1883

# lazy hex conversion. Ascii values are consecutive so just do (int) char-'0'
HEX = "0123456789:;<=>?"

conn = None


def init_socket():
    global conn
    conn = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    conn.bind((SERVER_ADDR, SERVER_PORT))
    conn.setblocking(0)


def pack_data(data):
    s = ''
    for b in data:
        s += struct.pack('B', b)
    return s

# takes a value from


def byte_to_hex(byte):
    h = byte // 16
    l = byte - (16 * h)
    return HEX[h] + HEX[l]


# motorNames=["BackLeft","BackRight","FrontLeft","FrontRight"]
""" Hopefully we can be more organized with the motor naming next semester
        F
2 FL+-------+FR 3
    |       |
L   | ROBOT |   R
    |       |
0 BL+-------+BR 1
        B
"""
motorNames = ["a", "b", "c", "d"]

init_socket()
while not rospy.is_shutdown():
    pub = rospy.Publisher('bridge', PackedMessage, queue_size=20)
    rospy.init_node('gs_recv', anonymous=True)

    received_data = bytearray(2500)
    try:
        received_len, addr = conn.recvfrom_into(received_data, 2048)
    except Exception as e:
        received_len = 0

    if received_len:
        data = ""
        hbSum = 0
        # first 3 recieved bytes are for the heartbeat
        for i in range(3):
            hbSum += received_data[i]
        #  if there are parameters (4th byte indicates how many target intensity pairs there are)
        if hbSum != 160 and received_len > 3:
            for i in range(received_data[3] / 2):
                target = received_data[2 * i + 4]
                msg = PackedMessage()
                # convert recieved troy's values to marcus' values
                # neg:126-0 neu:127 pos:128-255 -> neu:0 neg:127-1 pos:128-255
                msg.value = received_data[2 * i + 5]
                if msg.value == 127:
                    msg.value = 0
                elif msg.value < 127:
                    msg.value += 1
                if target == 21:
                    msg.channel_name = "digging/linearActuator"
                    msg.field = "b"
                    msg.value = 250
                    pub.publish(msg)
                if target == 20:
                    msg.channel_name = "digging/linearActuator"
                    msg.field = "b"
                    msg.value = 120
                    pub.publish(msg)
                if target == 8:
                    rospy.loginfo("Target:  " + str(target))
                    msg.channel_name = "digging/motors"
                    rospy.loginfo("Recieved %d. Sending digging value %d",
                                  received_data[2 * i + 5], msg.value)
                    msg.field = "all"
                    pub.publish(msg)
                if target == 7:  # linear actuator need to make sure it is up or down
                    msg.channel_name = "digging/linearActuator"
                    rospy.loginfo(
                        "Recieved %d. Sending linear actuator value %d", received_data[2 * i + 5], msg.value)
                    msg.field = "a"
                    pub.publish(msg)
                    # msg.field = "b"
                    # pub.publish(msg)
                if target == 22:
                    msg.channel_name = "digging/linearActuator"
                    msg.field = "a"
                    msg.value = 250
                    pub.publish(msg)
                if target == 23:
                    msg.channel_name = "digging/linearActuator"
                    msg.field = "a"
                    msg.value = 110
                    pub.publish(msg)
                if target == 24:
                    msg.channel_name = "digging/linearActuator"
                    msg.field = "a"
                    msg.value = 0
                    pub.publish(msg)
                if target == 25:
                    msg.channel_name = "deposition/motors"
                    msg.field = "all"
                    msg.value = 200
                    pub.publish(msg)
                if target == 26:
                    msg.channel_name = "deposition/motors"
                    msg.field = "all"
                    msg.value = 455
                    pub.publish(msg)
                if target == 27:
                    msg.channel_name = "deposition/motors"
                    msg.field = "all"
                    msg.value = 0
                    pub.publish(msg)
                if target == 28:
                    msg.channel_name = "deposition/linearActuator"
                    msg.field = "a"
                    msg.value = 100
                    pub.publish(msg)
                if target == 29:
                    msg.channel_name = "deposition/linearActuator"
                    msg.field = "a"
                    msg.value = 254
                    pub.publish(msg)
                if target == 30:
                    msg.channel_name = "deposition/linearActuator"
                    msg.field = "a"
                    msg.value = 0
                    pub.publish(msg)
                if target == 31:
                    msg.channel_name = "locomotion/direction"
                    msg.field = "forward"
                    pub.publish(msg)
                    rospy.loginfo("Recieved drive forward command, sending %s", msg.value)
                if target == 32:
                    msg.channel_name = "locomotion/turn"
                    msg.field = "left"
                    pub.publish(msg)
                if target==33:
                    msg.channel_name = "locomotion/turn"
                    msg.field = "right"
                    pub.publish(msg)
                if target==34:
                    msg.channel_name = "locomotion/direction"
                    msg.field = "forward"
                    msg.value = 0
                    pub.publish(msg)
                #Panic target
                if target==35:
                    msg.channel_name = "locomotion"
                    msg.field = "panic"
                    pub.publish(msg)
                    
                    msg.channel_name = "digging"
                    msg.field = "panic"
                    pub.publish(msg)
                    
                    msg.channel_name = "depostion"
                    msg.field = "panic"
                    pub.publish(msg)
                if 1<=target<=4:
                    msg.channel_name = "loco/motors"
                    msg.field = motorNames[target-1]
                    rospy.loginfo("Recieved %d. Sending motor %s value %d",received_data[2 * i + 5],motorNames[target-1],msg.value)
                    pub.publish(msg)

                        
        conn.sendto(pack_data([0x4E,0x52,0x00]), (CLIENT_ADDR, CLIENT_PORT))
        time.sleep(0.01)
conn.close()
