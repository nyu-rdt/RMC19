#!/usr/bin/env python

import socket, json, os, time
import threading, Queue
import atlas_parser, atlas_socket
import pygame
import sys
import platform
	
class DrawStatus:
	def __init__(self, screen, center=(50, 50), radius=20):
		self.center = center
		self.radius = radius
		self.connected_color = (0, 255, 0)
		self.disconnected_color = (255, 0, 0)
		self.disconnected = False
		self.screen = screen
	def draw(self):
		if self.disconnected:
			pygame.draw.circle(self.screen, self.disconnected_color, self.center, self.radius, 0)
		else:
			pygame.draw.circle(self.screen, self.connected_color, self.center, self.radius, 0)
	def setStatus(self, status):
		self.disconnected = status

def putincommand(data):
	if not connection_break:
		command_buffer_lock.acquire()
		command_buffer.put(data)
		command_buffer_lock.release()

# while True:
# 	if not ui_buffer.empty():
# 		dic = ui_buffer.get()
# 		if 'type' in dic:
# 			print dic['type']
# 	else:
# 		print "connection okay"
# 	time.sleep(2)

# Program Initialization
socket_to_parser = Queue.Queue(maxsize = -1)
parser_to_socket = Queue.Queue(maxsize = -1)
command_buffer = Queue.Queue(maxsize = -1)
ui_buffer = Queue.Queue(maxsize = -1)

command_buffer_lock = threading.Lock()

local_ip = "0.0.0.0"
local_port = 10010

# if len(sys.argv) == 1:
# 	target_ip = "192.168.1.120"
# 	target_port = 10010
# elif sys.argv[1] == 'local':
# 	target_ip = "127.0.0.1"
# 	target_port = 10086

#target_ip = "127.0.0.1"
# target_port = 10086
target_ip = "192.168.1.102"
target_port = 10010
#target_ip = "127.0.0.1"
#target_port = 10086

connection_break = False

socket_thread = atlas_socket.AtlasSocket("thread-socket", 
											parser_to_socket, 
											socket_to_parser, 
											target_ip, 
											target_port, 
											local_ip, 
											local_port, 30, 10)
parser_thread = atlas_parser.AtlasParser("thread-parser", 
											socket_to_parser, 
											parser_to_socket, 
											command_buffer, 
											command_buffer_lock, 
											ui_buffer, 20)

socket_thread.start()
parser_thread.start()

socket_thread.socket_start()

# look at the ethernet connection condition
# if(connection breaks)
# 	socket.socket_destroy;
# if(connection recovered)
# 	socket.start();
# PyGame Initialization
pygame.init()
pygame.joystick.init()
pygame.display.set_caption('AtlasGCS')
size = [100, 100]
screen = pygame.display.set_mode(size)
clock = pygame.time.Clock()
connection_status = DrawStatus(screen)
is_os = False
if platform.system() == 'Darwin':
	is_os = True

# Variable Initialization
BLACK = (0, 0, 0)
WHITE = (255, 255, 255)
done = False
mode = ''
belt_mode = ''
lin_act_mode = ''
sweep_mode = 'sweep_stop'
keyboard_speed_setting_toggle = False
digging_setting_toggle = False
door_setting_toggle = False
lin_act_setting_toggle = False

#modify backward left straightforward right values to change wheel motor values
#order of values is back left, back right, front left, front right
#conversion is val(0 to 255) - 127
speed_list = {'backward':[227,227,27,27], 'left':[27,27,27,27], 
		'straightforward':[27,27,227,227], 'right':[227,227,227,227], 'digging':[228,26,228,26], 
		'stop':[127,127,127,127], 'scissor_up':255, 'scissor_down':0, 'scissor_stop': 127, #HERE MODIFY THE SPEED OF UP AND DOWN--- 
												#we need to convert the up and down on the bin - charles 
												# 127 is the 0 zone. anything <127 will be positive power, >127 is negative 		
		'belt_fwd': 255, 'belt_bwd': 0, 'belt_stop': 127, 'sweep_start': 255, 'sweep_stop': 127,
		'back_digging':[228, 127, 127, 26], 'front_digging':[127, 26, 228, 127]}

SPEED = 500

# Main loop
while done == False:

	# Draw action detector
	screen.fill(BLACK)
	if not ui_buffer.empty():
		ui_data = ui_buffer.get()
		if ui_data['type'] == "connection_break":
			connection_break = True
		else:
			connection_break = False
		connection_status.setStatus(connection_break)

	for event in pygame.event.get(): # User did something
		if event.type == pygame.QUIT: # If user clicked close
			done=True # Flag that we are done so we exit this loop
			socket_thread.socket_destroy() # Destroy socket_thread to ensure opened port be closed properly
		# Get keyboard input
		if event.type == pygame.KEYDOWN:
			if event.key == pygame.K_w:
				mode = 'straightforward'
				keyboard_speed_setting_toggle = True
				print "Forward pressed"
			elif event.key == pygame.K_d:
				mode = 'right'
				print "Right pressed"
				keyboard_speed_setting_toggle = True
			elif event.key == pygame.K_s:
				mode = 'backward'
				print "Backward pressed"
				keyboard_speed_setting_toggle = True #CAN GO SPIN NOW WE NEED TO ADD SPEED?  ------------------ng_toggle = True
			elif event.key == pygame.K_a:
				mode = 'left'
				print "Left pressed"
				keyboard_speed_setting_toggle = True

			# Drum spin at last set speed command (default: 50)
			elif event.key == pygame.K_q:   
				mode = 'digging'
				sweep_mode = 'sweep_start'
				print "Digging pressed"
				digging_setting_toggle = True

			# Linear actuators down command
			elif event.key == pygame.K_k:
				lin_act_mode = 'lin_acts_down' 
				print "Linear actuators down pressed"
				lin_act_setting_toggle = True

			# Linear actuators up command
			elif event.key == pygame.K_i:
				lin_act_mode = 'lin_acts_up'
				print "Linear actuators up pressed"
				lin_act_setting_toggle = True

			# Door open command
			elif event.key == pygame.K_o:
				belt_mode = 'door_open'
				print "Belt forward pressed"
				door_setting_toggle = True

			# Door close command
			elif event.key == pygame.K_l:
				belt_mode = 'door_close'
				print "Belt forward pressed"
				door_setting_toggle = True

			# Drum speed adjustments; 0 is off, 1-4 is range of values lowest to highest
			elif event.key == pygame.K_0:
				SPEED=127
				print "Speed set to ",SPEED
				digging_setting_toggle = True
			elif event.key == pygame.K_1:
				SPEED=200
				print "Speed set to ",SPEED
				digging_setting_toggle = True
			elif event.key == pygame.K_2:
				SPEED=210
				print "Speed set to ",SPEED
				digging_setting_toggle = True
			elif event.key == pygame.K_3:
				SPEED=230
				print "Speed set to ",SPEED
				digging_setting_toggle = True
			elif event.key == pygame.K_4:
				SPEED=500
				print "Speed set to ",SPEED
				digging_setting_toggle = True

		# Defining key released events for each key
		if event.type == pygame.KEYUP: 
			if event.key == pygame.K_w or event.key == pygame.K_a or event.key == pygame.K_s or event.key == pygame.K_d:
				mode = 'stop'
				print "Key released"
				keyboard_speed_setting_toggle = True
			elif event.key == pygame.K_q: #STOP DIGGING HERE -----------------
				mode = 'drum_stop'
				print "Key released"
				digging_setting_toggle = True
			elif event.key == pygame.K_o:
				belt_mode = 'door_open'
				door_setting_toggle = True
			elif event.key == pygame.K_l:
				belt_mode = 'door_close'
				door_setting_toggle = True
			elif event.key == pygame.K_i or event.key == pygame.K_k:
				lin_act_mode = 'lin_acts_stop'  
				print "Linear actuators stopped"
				lin_act_setting_toggle = True
		
	
	if keyboard_speed_setting_toggle \
		or digging_setting_toggle or door_setting_toggle or lin_act_setting_toggle:
		send_data = []

		if keyboard_speed_setting_toggle:
			send_data.append({'name': 'W/R Motor0', 'value': speed_list[mode][0]})
			send_data.append({'name': 'W/R Motor1', 'value': speed_list[mode][1]})
			send_data.append({'name': 'W/R Motor2', 'value': speed_list[mode][2]})
			send_data.append({'name': 'W/R Motor3', 'value': speed_list[mode][3]})
			keyboard_speed_setting_toggle = False
                """
		"""				
		if digging_setting_toggle:
			if mode == 'drum_stop':
				send_data.append({'name': 'W/R Servo1', 'value': 127}) #changed to servo 1 so it doesn't mix up with the locomotion motors
			else:
				send_data.append({'name': 'W/R Servo1', 'value': SPEED}) 
			digging_setting_toggle = False

		if lin_act_setting_toggle:
			if lin_act_mode == 'lin_acts_stop':
				send_data.append({'name': 'Linear Actuators Stop', 'value': 0})
			elif lin_act_mode == 'lin_acts_up':
				send_data.append({'name': 'Linear Actuators Up', 'value': 220})
			elif lin_act_mode == 'lin_acts_down':
				send_data.append({'name': 'Linear Actuators Down', 'value': 110})
			lin_act_setting_toggle=False	

		if door_setting_toggle:
			if belt_mode == 'door_open':
				send_data.append({'name': 'Door Open', 'value': 120})
			elif belt_mode == 'door_close':
				send_data.append({'name': 'Door Close', 'value': 250})
		
		keyboard_speed_setting_toggle = False
		door_setting_toggle = False
		putincommand(send_data)
	connection_status.draw()
	pygame.display.flip()
	clock.tick(60)
pygame.quit()
