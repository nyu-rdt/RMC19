#!/usr/bin/env python

import socket, json, os, time
import threading, Queue
import struct

class AtlasSocket(threading.Thread):
	def __init__(self, threadName, input_buffer, output_buffer, target_ip = '127.0.0.1', target_port = 10010, local_ip = '127.0.0.1', local_port = 10010, report_rate = 20, failsafe_th = 5):
		self.input_buffer = input_buffer
		self.output_buffer = output_buffer
		self.target_ip = target_ip
		self.target_port = target_port
		self.local_ip = local_ip
		self.local_port = local_port
		self.initialized = False
		self.no_response_counter = 0
		self.conn_break = False
		self.hanging = True
		self.failsafe_th = failsafe_th
		self.report_rate = report_rate
		self.reconnection_flag = False
		self.destroy = False
		self.HEARTBEAT_PACKAGE = self.data2bytestring([0x4E, 0x52, 0x00])	#initialize the heartbeat package
		self.last_data = self.HEARTBEAT_PACKAGE	#default value of lastdata is heartbeat package
		super(AtlasSocket, self).__init__(name = threadName)
	def run(self):
		while not self.destroy:
			if not self.hanging:
				receive_data = bytearray(2500)
				receive_dict = {}	#initialize the dict which used for reciving data
				try:
					receive_nbytes, receive_addr = self.conn.recvfrom_into(receive_data, 2048)	#read data from socket
				except:
					receive_nbytes = 0
				# print receive_nbytes
				if receive_nbytes == 0:		#if there is no data in receiving buffer. Obviously, if there is no alive connection, socket will attempt connect to target continously
					self.no_response_counter += 1		#increasing no-response counter
					if self.no_response_counter >= self.failsafe_th:	#if the no-response counter reach the threshold
						self.no_response_counter = self.failsafe_th		#limit the no-response counter value
						self.conn_break = True		#set the connection break flag
						receive_dict['type'] = 'connection_break'	#make a 'connection_break' data to inform the main thread
						self.output_buffer.put(receive_dict)		#put dict into output buffer(queue)
				else:	#if there are some return values
					self.no_response_counter = 0	#clear the no-response counter
					self.conn_break = False 		#clear the connection break flag
					receive_dict['type'] = 'regular_data'	#identify the data type
					receive_dict['data'] = receive_data		#insert data into dict
					receive_dict['addr'] = receive_addr		
					receive_dict['nbytes'] = receive_nbytes
					self.output_buffer.put(receive_dict)	#put dict into output buffer(queue)
				if self.no_response_counter > 0:			#handle the no-response situation(reconnection)
					self.conn.sendto(self.HEARTBEAT_PACKAGE, (self.target_ip, self.target_port))	#send self.HEARTBEAT_PACKAGE to reconnect to the robot rather than sending command
					print "recover package sent"
					self.reconnection_flag = True
				elif not self.input_buffer.empty():		#if there is at least one command need to be send in the input buffer(queue), send it
					if self.reconnection_flag:
						self.reconnection_flag = False 		#clear the flag of reconnection
					else:
						self.last_data = self.data2bytestring(self.input_buffer.get())	#get new command from queue
					self.conn.sendto(self.last_data, (self.target_ip, self.target_port))
				else:	#if the connection is still alive but there is no data needed to be sent, just send the heartbeat package
					self.conn.sendto(self.HEARTBEAT_PACKAGE, (self.target_ip, self.target_port))
			time.sleep(1.0 / self.report_rate)	#sleep thread for the inverse of report rate
		self.conn.close()
	def socket_init(self):
		if self.initialized:	#if socket has been initialized, close it and initialize again
			self.conn.close()
			self.initialized = False
		try:
			self.conn = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)	#initialize a socket object
			self.conn.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
			self.conn.bind((self.local_ip, self.local_port))	#bind local ip and port
			self.conn.setblocking(0)
		except:
			self.conn.close()
		self.initialized = True 	#set initialized flag
	def change_target(self, target_ip, target_port):	#change the target information if necessary
		self.target_ip = target_ip
		self.target_port = target_port
	def change_local(self, local_ip, local_port):	#change the local information if necessary
		self.local_ip = local_ip
		self.local_port = local_port
		self.socket_init()
	def change_report_rate(self, report_rate):	#change the report rate if necessary
		self.report_rate = report_rate
	def change_failsafe_threshold(self, failsafe_th):	#change the failsafe threshold if necessary
		self.failsafe_th = failsafe_th
	def data2bytestring(self, data):	#data is an integer list, we have to convert it into a bytestring e.g.[0x01,0x02,0x03] -> '\x01\x02\x03'
		s = ''
		for x in data:
			s += struct.pack('B',x)
		return s
	def socket_stop(self):
		self.hanging = True
	def socket_start(self):
		self.socket_init()
		self.hanging = False
	def socket_destroy(self):
		self.destroy = True
		
