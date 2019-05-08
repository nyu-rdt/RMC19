# !/usr/bin/env python

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
EXIT_GCS_WITH_BKSP = True # Disable if you do not want to exit gcs with backspace key

BLACK = (0, 0, 0)
WHITE = (255, 255, 255)
done = False

move_mode = ''
drum_mode = ''
door_mode = ''
lin_act_mode = ''
depo_mode = ''
depo_lin_act_mode = ''

move_setting_toggle = False
digging_setting_toggle = False
door_setting_toggle = False
lin_act_setting_toggle = False
depo_setting_toggle = False
depo_lin_act_setting_toggle = False

MOVEMENT_SPEED = 100
DRUM_SPEED = 500
LIN_ACT_SPEED = 0

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
				move_mode = 'forward'
				move_setting_toggle = True
				print "Forward pressed"
			elif event.key == pygame.K_d:
				move_mode = 'right'
				print "Right pressed"
				move_setting_toggle = True
			elif event.key == pygame.K_s:
				move_mode = 'backward'
				print "Backward pressed"
				move_setting_toggle = True
			elif event.key == pygame.K_a:
				move_mode = 'left'
				print "Left pressed"
				move_setting_toggle = True

			# Drum spin at last set speed command (default: 50)
			elif event.key == pygame.K_q:   
				drum_mode = 'drum_start'
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
				door_mode = 'door_open'
				print "Belt forward pressed"
				door_setting_toggle = True

			# Door close command
			elif event.key == pygame.K_l:
				door_mode = 'door_close'
				print "Belt forward pressed"
				door_setting_toggle = True

			# Drum speed adjustments; 0 is off, 1-4 is range of values lowest to highest
			elif event.key == pygame.K_0:
				DRUM_SPEED=127
				print "Speed set to ", DRUM_SPEED
				digging_setting_toggle = True
			elif event.key == pygame.K_1:
				DRUM_SPEED=200
				print "Dig speed set to ", DRUM_SPEED
				digging_setting_toggle = True
			elif event.key == pygame.K_2:
				DRUM_SPEED=210
				print "Dig speed set to ", DRUM_SPEED
				digging_setting_toggle = True
			elif event.key == pygame.K_3:
				DRUM_SPEED=230
				print "Dig speed set to ", DRUM_SPEED
				digging_setting_toggle = True
			elif event.key == pygame.K_4:
				DRUM_SPEED=500
				print "Dig speed set to ", DRUM_SPEED
				digging_setting_toggle = True

			# Locomotion speed adjustments; 6-9 is range of values lowest to highest
			elif event.key == pygame.K_6:
				MOVEMENT_SPEED=100
				print "Drive speed set to ", MOVEMENT_SPEED
				move_setting_toggle = True
			elif event.key == pygame.K_7:
				MOVEMENT_SPEED=150
				print "Drive speed set to ", MOVEMENT_SPEED
				move_setting_toggle = True
			elif event.key == pygame.K_8:
				MOVEMENT_SPEED=200
				print "Drive speed set to ",MOVEMENT_SPEED
				move_setting_toggle = True
			elif event.key == pygame.K_9:
				MOVEMENT_SPEED=255
				print "Drive speed set to ", MOVEMENT_SPEED
				move_setting_toggle = True

			# Depo sweep in command
			elif event.key == pygame.K_u:
				depo_mode = 'depo_sweep_in'
				print "Depo sweep in pressed"
				depo_setting_toggle = True

			# Depo sweep out command
			elif event.key == pygame.K_j:
				depo_mode = 'depo_sweep_out'
				print "Depo sweep out pressed"
				depo_setting_toggle = True

			# Depo linear actuators up command
			elif event.key == pygame.K_y:
				depo_lin_act_mode = 'depo_lin_acts_up'
				print "Depo linear actuators up pressed"
				depo_lin_act_setting_toggle = True
			
			# Depo linear actuators up command
			elif event.key == pygame.K_h:
				depo_lin_act_mode = 'depo_lin_acts_down'
				print "Depo linear actuators down pressed"
				depo_lin_act_setting_toggle = True

			elif event.key == pygame.K_BACKSPACE:
				if EXIT_GCS_WITH_BKSP:
					done = True
					socket_thread.socket_destroy()


		# Defining key released events for each key
		if event.type == pygame.KEYUP: 
			if event.key == pygame.K_w or event.key == pygame.K_a or event.key == pygame.K_s or event.key == pygame.K_d:
				move_mode = 'stop'
				print "Key released"
				move_setting_toggle = True
			elif event.key == pygame.K_q: 
				drum_mode = 'drum_stop'
				print "Key released"
				digging_setting_toggle = True
			elif event.key == pygame.K_o:
				door_mode = 'door_open'
				door_setting_toggle = True
			elif event.key == pygame.K_l:
				door_mode = 'door_close'
				door_setting_toggle = True
			elif event.key == pygame.K_i or event.key == pygame.K_k:
				lin_act_mode = 'lin_acts_stop'  
				print "Linear actuators stopped"
				lin_act_setting_toggle = True
			elif event.key == pygame.K_u or event.key == pygame.K_j:
				depo_mode = 'depo_sweep_stop'
				print "Depo sweep stop pressed"
				depo_setting_toggle = True
			elif event.key == pygame.K_y or event.key == pygame.K_h:
				depo_lin_act_mode = 'depo_lin_acts_stop'
				print "Depo linear actuators stop pressed"
				depo_lin_act_setting_toggle = True

		
	
	if move_setting_toggle or digging_setting_toggle or door_setting_toggle \
	or lin_act_setting_toggle or depo_setting_toggle or depo_lin_act_setting_toggle:
		send_data = []

		if move_setting_toggle:
			if move_mode == 'stop':
				send_data.append({'name': 'Loco Stop', 'value': 0})
			elif move_mode == 'forward':
				send_data.append({'name': 'Loco Forward', 'value': MOVEMENT_SPEED})
			elif move_mode == 'backward':
				send_data.append({'name': 'Loco Forward', 'value': -MOVEMENT_SPEED})
			elif move_mode == 'left':
				send_data.append({'name': 'Loco Left', 'value': MOVEMENT_SPEED})
			elif move_mode == 'right':
				send_data.append({'name': 'Loco Right', 'value': MOVEMENT_SPEED})

			move_setting_toggle = False
              
		if digging_setting_toggle:
			if drum_mode == 'drum_stop':
				send_data.append({'name': 'W/R Servo1', 'value': 127}) #changed to servo 1 so it doesn't mix up with the locomotion motors
			else:
				send_data.append({'name': 'W/R Servo1', 'value': DRUM_SPEED}) 
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
			if door_mode == 'door_open':
				send_data.append({'name': 'Door Open', 'value': 120})
			elif door_mode == 'door_close':
				send_data.append({'name': 'Door Close', 'value': 250})

		if depo_setting_toggle:
			if depo_mode == 'depo_sweep_stop':
				send_data.append({'name': 'Depo Sweep Stop', 'value': 0})
			elif depo_mode == 'depo_sweep_in':
				send_data.append({'name': 'Depo Sweep In', 'value': 200})
			elif depo_mode == 'depo_swseep_out':
				send_data.append({'name': 'Depo Sweep Out', 'value': -200})
			depo_setting_toggle = False

		if depo_lin_act_setting_toggle:
			if depo_lin_act_mode == 'depo_lin_acts_stop':
				send_data.append({'name': 'Depo Linear Actuators Stop', 'value': 0})
			if depo_lin_act_mode == 'depo_lin_acts_up':
				send_data.append({'name': 'Depo Linear Actuators Up', 'value': 100})
			if depo_lin_act_mode == 'depo_lin_acts_down':
				send_data.append({'name': 'Depo Linear Actuators Down', 'value': 255})
			depo_lin_act_setting_toggle = False
		
		move_setting_toggle = False
		door_setting_toggle = False
		putincommand(send_data)
	connection_status.draw()
	pygame.display.flip()
	clock.tick(60)
pygame.quit()
sys.exit()
