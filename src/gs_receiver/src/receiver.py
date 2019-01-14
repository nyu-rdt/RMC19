#!/usr/bin/env python
import socket, os, time, json, struct
import rospy

from rdt_mqtt.msg import PackedMessage

SERVER_ADDR = '192.168.109.147'
SERVER_PORT = 10010

CLIENT_ADDR = '192.168.109.1'
CLIENT_PORT = 10010

conn = None
def init_socket():
    global conn
    conn = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    conn.bind((SERVER_ADDR,SERVER_PORT))
    conn.setblocking(0)

def pack_data(data):
    s = ''
    for b in data:
        s += struct.pack('B', b)
    return s

init_socket()
while not rospy.is_shutdown():
    pub = rospy.Publisher('bridge', PackedMessage, queue_size = 20)
    rospy.init_node('gs_recv', anonymous=True)
    
    
    received_data = bytearray(2500)
    try:
        received_len, addr = conn.recvfrom_into(received_data, 2048)
    except Exception as e:
        received_len = 0

    if received_len:
        msg = PackedMessage()
        msg.channel_name = 'Channel'
        msg.field = 'dafield'
        data = 0
        for i in range(received_len):
            data += received_data[i]
        msg.value = data 
        
        rospy.loginfo(data)
        pub.publish(msg) 

        conn.sendto(pack_data([0x4E,0x52,0x00]), (CLIENT_ADDR, CLIENT_PORT))
        rospy.loginfo('Sent ACK')
        rospy.loginfo("-------------")

    time.sleep(0.01)

conn.close()
