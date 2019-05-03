#!/usr/bin/env python

import socket, json, os, time
import threading, Queue
import struct

class AtlasParser(threading.Thread):
	def __init__(self, threadName, input_buffer, output_buffer, command_buffer, command_buffer_lock, UI_buffer, report_rate = 20):
		self.input_buffer = input_buffer
		self.output_buffer = output_buffer
		self.command_buffer = command_buffer
		self.command_buffer_lock = command_buffer_lock
		self.UI_buffer = UI_buffer
		self.report_rate = report_rate
		self.isHeartBeatPrev = False
		self.load_commands('atlas_commands.json')
		super(AtlasParser, self).__init__(name = threadName)
	def run(self):
		while True:
			self.command_buffer_lock.acquire()
			if not self.command_buffer.empty():
				command = self.command_buffer.get()
				frame = [0x4E,0x52,0x01,0x00]
				for x in command:
					if x['name'] in self.commands:
						print x['name']
						frame.append(self.commands[x['name']]['cmd'])
						frame[3] += 1
						if self.commands[x['name']]['writedata'] == 0x00:
							for i in range(self.commands[x['name']]['writebits'] / 8)[::-1]: 
								temp_value = 0
								temp_value = 0xFF & x['value'] >> (i * 8)
								frame.append(temp_value)
								frame[3] += 1
						else:
							frame.append(self.commands[x['name']]['writedata'])
							frame[3] += 1
				print frame
				self.output_buffer.put(frame)
			self.command_buffer_lock.release()

			if not self.input_buffer.empty():
				UI_data = {}
				received_data = self.input_buffer.get()
				if received_data['type'] == 'connection_break':
					UI_data['type'] = 'connection_break'
					self.UI_buffer.put(UI_data)
					self.isHeartBeatPrev = False
				elif received_data['type'] == 'regular_data':
					if received_data['data'][0] == 0x4E and received_data['data'][1] == 0x52 and received_data['data'][2] == 0x02:
						UI_data['addr'] = received_data['addr']
						UI_data['data'] = []
						UI_data_index = -1
						parsering_command = False
						for i in range(received_data['data'][3]):
							if not parsering_command:
								for j in self.commands:
									if self.commands[j]['cmd'] == received_data['data'][4+i]:
										if UI_data_index >= 0:
											UI_data['data'][UI_data_index]['valid'] = True
										parsering_command = True
										UI_data_index += 1
										temp_value = 0
										length_counter = 0
										UI_data['data'].append({'name':j, 'valid':False})
										break;
							else:
								temp_value = temp_value | received_data[4+i]
								temp_value = temp_value << 8
								length_counter += 8
								if length_counter == self.commands[UI_data['data'][UI_data_index]['name']]['readbits']:
									UI_data['data'][UI_data_index]['value'] = temp_value
									parsering_command = False
									if i == received_data['data'][3] - 1:
										UI_data['data'][UI_data_index]['valid'] = True
						self.UI_buffer.put(UI_data)
						self.isHeartBeatPrev = False
					elif received_data['data'][0] == 0x4E and received_data['data'][1] == 0x52 and received_data['data'][2] == 0x00:
						if not self.isHeartBeatPrev:
							self.isHeartBeatPrev = True
							UI_data['type'] = 'heart_beat'
							self.UI_buffer.put(UI_data)
			time.sleep(1.0 / (2 * self.report_rate))
	def load_commands(self, filename):
		with open(filename, 'r') as f:
			self.commands = json.load(f)
	def change_report_rate(self, report_rate):
		self.report_rate = report_rate
