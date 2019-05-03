#!/usr/bin/env python

import socket, json, os, time
import struct
local_ip = "127.0.0.1"
local_port = 10086
target_ip = "127.0.0.1"
target_port = 10010

def data2bytestring(data):	#data is an integer list, we have to convert it into a bytestring e.g.[0x01,0x02,0x03] -> '\x01\x02\x03'
	s = ''
	for x in data:
		s += struct.pack('B',x)
	return s



def socket_init():
	global conn
	conn = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)	#initialize a socket object
	conn.bind((local_ip, local_port))	#bind local ip and port
	conn.setblocking(0)


socket_init()
work = True
while True:
	receive_data = bytearray(2500)
	try:
		receive_nbytes, receive_addr = conn.recvfrom_into(receive_data, 2048)	#read data from socket
	except:
		receive_nbytes = 0
	if receive_nbytes:
		print "data received:"
		for i in range(receive_nbytes):
			print str(receive_data[i]) + ',',
		print ""
		conn.sendto(data2bytestring([0x4E, 0x52, 0x00]), (target_ip, target_port))
	time.sleep(0.01)

conn.close()
