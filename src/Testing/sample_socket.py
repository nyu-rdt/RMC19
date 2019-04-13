import socket
import sys
import threading


class SampleSocket(threading.Thread) :	
	def __init__ (self, threadName, input_buffer, output_buffer, remote_ip="127.0.0.1", remote_socket=10010, local_ip="0.0.0.0", local_socket=10010) :
		super
		self.remote_ip = remote_ip
		self.remote_socket = remote_socket
		self.local_ip = local_ip
		self.local_socket = local_socket

		self.remote = (self.remote_ip, self.remote_socket)
		self.local = (self.local_ip, self.local_socket)

		self.output_buffer = output_buffer
		self.input_buffer = input_buffer 

		self.initialized = False
		self.destroy = False
		self.hanging = False

		super(SampleSocket, self).__init__(name = threadName)

	def init_socket (self) :
		if not self.initialized : 
			self.initialized = True
			self.socket = socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM)
			self.socket.bind(self.local)
			self.socket.setblocking(0)
		else :
			self.close()

	def run (self) :
		while not self.destroy :
			if not self.hanging :
				received_data = bytearray(2500)
				try:
					data_len, addr = self.socket.recvfrom_into(received_data, 2048)
					#print (data_len, addr)
					#print(received_data[0],received_data[1],received_data[2])
					#data = self.socket.recv(2048)	#read data from socket
					#print('0')
					data_length = len(data)
				except:
					data_length = 0
					#print('1')
				if data_length != 0 :
					self.output_buffer.put((data, data_length))
					#print('2')
				else :
					#print('3')
					pass
				if not self.input_buffer.empty() :
					byteCommand = str.encode(self.input_buffer.get())
					self.socket.sendto(byteCommand, self.remote)

		self.close()

	def stop (self) :
		self._is_running = False
		self.close()

	def close (self) :
		self.socket.close()
