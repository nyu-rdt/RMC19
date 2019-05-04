#!/usr/bin/env python
import socket, os, time, json, struct
import rospy

from rdt_mqtt.msg import PackedMessage

SERVER_ADDR = '0.0.0.0'
SERVER_PORT = 10010

#CLIENT_ADDR = '10.100.32.215'
#CLIENT_ADDR = '127.0.0.1'
CLIENT_ADDR = '192.168.1.102'
#CLIENT_ADDR = '192.168.1.104'
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

#motorNames=["BackLeft","BackRight","FrontLeft","FrontRight"]
""" Hopefully we can be more organized with the motor naming next semester
        F
2 FL+-------+FR 3
    |       |
L   | ROBOT |   R
    |       |
0 BL+-------+BR 1
        B
"""
motorNames=["a","b","c","d"]

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
        data = ""
        hbSum = 0
        # first 3 recieved bytes are for the heartbeat
        for i in range(3):
            hbSum+= received_data[i]
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
                if target==8:
                    msg.channel_name = "digging/motors"
                    rospy.loginfo("Recieved %d. Sending digging value %d",received_data[2 * i + 5],msg.value)
                    msg.field = "a"
                    pub.publish(msg)
                    msg.field = "b"
                    pub.publish(msg)
                if target==7:# linear actuator need to make sure it is up or down
                    msg.channel_name = "digging/linearActuator"
                    rospy.loginfo("Recieved %d. Sending linear actuator value %d",received_data[2 * i + 5],msg.value)
                    msg.field = "a"
                    pub.publish(msg)
                    msg.field = "b"
                    pub.publish(msg)
                if 1<=target<=4:#locomotion motors
                    msg.channel_name = "loco/motors"
                    msg.field = motorNames[target-1]
                    rospy.loginfo("Recieved %d. Sending motor %s value %d",received_data[2 * i + 5],motorNames[target-1],msg.value)
                    pub.publish(msg)

                        
        conn.sendto(pack_data([0x4E,0x52,0x00]), (CLIENT_ADDR, CLIENT_PORT))    
            
	"""
        dataHex=""
        sum=0
        for i in range(received_len):
            data += str(received_data[i])
            data += " "
            dataHex += byte_to_hex(received_data[i])
            sum += received_data[i]
            
        #converts data recieved to a hex string to be converted on the reciever
        #msg.value = dataHex# + data
        #rospy.loginfo(data)
        #pub.publish(msg)
        
        if sum != 160:
            msg.value = dataHex# + data
            rospy.loginfo(data)
            pub.publish(msg) 
	
        conn.sendto(pack_data([0x4E,0x52,0x00]), (CLIENT_ADDR, CLIENT_PORT))
        
        if sum != 160:
            rospy.loginfo('Sent ACK')
            rospy.loginfo("-------------")"""

    time.sleep(0.01)

conn.close()
