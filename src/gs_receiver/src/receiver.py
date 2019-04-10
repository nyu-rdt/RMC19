#!/usr/bin/env python
import socket, os, time, json, struct
import rospy

from rdt_mqtt.msg import PackedMessage

SERVER_ADDR = '0.0.0.0'
# SERVER_PORT = 10010
SERVER_PORT = 10010

CLIENT_ADDR = '10.100.32.215'
# CLIENT_ADDR = '127.0.0.1'
CLIENT_PORT = 10010
#CLIENT_PORT = 1883

#lazy hex conversion. Ascii values are consecutive so just do (int) char-'0'
HEX="0123456789:;<=>?"

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

#takes a value from 
def byte_to_hex(byte):
    h=byte // 16
    l=byte - (16 * h)
    return HEX[h] + HEX[l]

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
        msg.channel_name = "EXAMPLE/input"
        msg.field = 'rawData'
        data = ""
        dataHex=""
        sum=0
        for i in range(received_len):
            data += str(received_data[i])
            data +=" "
            dataHex += byte_to_hex(received_data[i])
            sum += received_data[i]
        """
        #converts data recieved to a hex string to be converted on the reciever
        msg.value = dataHex# + data
        rospy.loginfo(data)
        pub.publish(msg)
        """
        if sum != 160:
            msg.value = dataHex# + data
            rospy.loginfo(data)
            pub.publish(msg) 
	
        conn.sendto(pack_data([0x4E,0x52,0x00]), (CLIENT_ADDR, CLIENT_PORT))
        """
        if sum != 160:
            rospy.loginfo('Sent ACK')
            rospy.loginfo("-------------")
        """

    time.sleep(0.01)

conn.close()
