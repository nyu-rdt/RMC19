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

def linear_remap_signed(dead_zone, input_data):
	if input_data > -dead_zone and input_data < dead_zone:
		return 127
	if input_data > 0:
		return (input_data - dead_zone) * 127 / (1000 - dead_zone) + 127
	if input_data < 0:
		return 127 - (-input_data - dead_zone) * 127 / (1000 - dead_zone)

def linear_remap(input_data):
	if input_data >= 0:
		return (input_data / 1000.0) * 126 + 127
	if input_data < 0:
		return 127 + (input_data / 1000.0) * 126

def dead_zone_remove(dead_zone, input_data):
	if input_data > -dead_zone and input_data < dead_zone:
		return 0
	if input_data > 0:
		return ((input_data - dead_zone) / (1000.0 - dead_zone)) * 1000
	if input_data < 0:
		return (-(-input_data - dead_zone) / (1000.0 - dead_zone)) * 1000

def restrict_range(n):
	if n >= 1000:
		return 1000
	elif n <= -1000:
		return -1000
	else:
		return n

def mix_control(x_axis, y_axis, wheel):
	x_processed = dead_zone_remove(dead_zone_joystick, x_axis)
	y_processed = dead_zone_remove(dead_zone_joystick, y_axis)
	if wheel == 0 or wheel == 1:
		return int(linear_remap(restrict_range(x_processed + y_processed) * motor_reverse_bit[wheel]))
	if wheel == 2 or wheel == 3:
		return int(linear_remap(-restrict_range(x_processed - y_processed) * motor_reverse_bit[wheel]))

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

# target_ip = "127.0.0.1"
# target_port = 10086
target_ip = "10.100.32.179"
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
BLACK = (   0,   0,   0)
WHITE = ( 255, 255, 255)
done = False
mode = ''
belt_mode = ''
scissor_mode = ''
sweep_mode = 'sweep_stop'
keyboard_speed_setting_toggle = False
joystick_speed_setting_toggle = False
digging_setting_toggle = False
sweep_setting_toggle = False
belt_setting_toggle = False
scissor_setting_toggle = False
toggle_scissor_mode = False
toggle_sweep_mode = False
pulse_sweep_mode = False
pulse_counter = 0
pulse_threshold = 15
pulse_status = 0
speed_list = {'backward':[228,228,26,26], 'left':[26,26,26,26], 
		'straightforward':[26,26,228,228], 'right':[228,228,228,228], 'digging':[228,26,228,26], 
		'stop':[127,127,127,127], 'scissor_up':26, 'scissor_down':228, 'scissor_stop': 127, 
		'belt_fwd': 255, 'belt_bwd': 0, 'belt_stop': 127, 'sweep_start': 255, 'sweep_stop': 127,
		'back_digging':[228, 127, 127, 26], 'front_digging':[127, 26, 228, 127]}
belt_list = {'belt_fwd': 1, 'belt_stop': 0}
motor_reverse_bit = [1, 1, -1, -1]
switches = 0;
dead_zone_joystick = 150
axis_change_th = 20
x_axis_pre = 0
y_axis_pre = 0
belt_button_pre = 0
scissor_button_pre = 0
dig_button_pre = 0
sweep_button_pre = 0
back_dig_button_pre = 0
front_dig_button_pre = 0
pulse_status_pre = 0
dig_mode = False
x_axis_raw = 0
y_axis_raw = 0
x_axis_remapped = 0
y_axis_remapped = 0
x_axis_home = False
y_axis_home = False

# Main loop
while done == False:
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
				keyboard_speed_setting_toggle = True
			elif event.key == pygame.K_a:
				mode = 'left'
				print "Left pressed"
				keyboard_speed_setting_toggle = True
			elif event.key == pygame.K_q:
				mode = 'digging'
				sweep_mode = 'sweep_start'
				print "Digging pressed"
				digging_setting_toggle = True
			elif event.key == pygame.K_i:
				scissor_mode = 'scissor_up'
				print "Scissor lift up pressed"
				scissor_setting_toggle = True
			elif event.key == pygame.K_l:
				belt_mode = 'belt_fwd'
				print "Belt forward pressed"
				belt_setting_toggle = True
			elif event.key == pygame.K_k:
				scissor_mode = 'scissor_down'
				print "Scissor lift down pressed"
				scissor_setting_toggle = True
			elif event.key == pygame.K_j:
				sweep_mode = 'sweep_start'
				print "Sweep pressed"
				sweep_setting_toggle = True

		if event.type == pygame.KEYUP: 
			if event.key == pygame.K_w or event.key == pygame.K_a or event.key == pygame.K_s or event.key == pygame.K_d:
				mode = 'stop'
				print "Key released"
				keyboard_speed_setting_toggle = True
			elif event.key == pygame.K_q:
				mode = 'stop'
				print "Key released"
				digging_setting_toggle = True
			elif event.key == pygame.K_l:
				belt_mode = 'belt_stop'
				belt_setting_toggle = True
			elif event.key == pygame.K_i or event.key == pygame.K_k:
				scissor_mode = 'scissor_stop'
				scissor_setting_toggle = True
			elif event.key == pygame.K_j:
				sweep_mode = 'sweep_stop'
				sweep_setting_toggle = True
		# Get joystick number
		joystick_count = pygame.joystick.get_count()
		# print joystick_count
		if joystick_count:
			joystick = pygame.joystick.Joystick(0)
			joystick.init()
			x_axis_raw = 1000 * joystick.get_axis(0)
			y_axis_raw = 1000 * joystick.get_axis(1)
			if is_os:
				dig_button = joystick.get_button(11)
				back_dig_button = joystick.get_button(12)
				belt_button = joystick.get_button(14)
				sweep_button = joystick.get_button(13)
				pulse_mode_button = joystick.get_button(5)
				toggle_mode_button = joystick.get_button(4)
				opt_scissor_button = (joystick.get_button(0), joystick.get_button(1))
				if opt_scissor_button == (0,0) or opt_scissor_button == (1,1):
					scissor_button = 0
				elif opt_scissor_button == (1,0):
					scissor_button = 1
				elif opt_scissor_button == (0,1):
					scissor_button = -1
			else:
				dig_button = joystick.get_button(0)
				_, scissor_button = joystick.get_hat(0)
				belt_button = joystick.get_button(1)
				sweep_button = joystick.get_button(4)
				back_dig_button = joystick.get_button(2)
				front_dig_button = joystick.get_button(3)
				pulse_mode_button = joystick.get_button(6)
				toggle_mode_button = joystick.get_button(7)
			# print x_axis_raw, " ", y_axis_raw
			if (dig_button == 0 and dig_button_pre == 0) and (back_dig_button == 0 and back_dig_button_pre == 0) and (front_dig_button == 0 and front_dig_button_pre == 0):
				if int(linear_remap_signed(dead_zone_joystick, x_axis_raw)) == 127 and not x_axis_home:
					joystick_speed_setting_toggle = True
					keyboard_speed_setting_toggle = False
					x_axis_home = True
					x_axis_remapped = 0
				elif abs(x_axis_raw - x_axis_pre) > axis_change_th and int(linear_remap_signed(dead_zone_joystick, x_axis_raw)) != 127:
					joystick_speed_setting_toggle = True
					keyboard_speed_setting_toggle = False
					x_axis_home = False
					x_axis_remapped = x_axis_raw
					# x_axis_remapped = int(linear_remap_signed(dead_zone_joystick, x_axis_raw))
				if int(linear_remap_signed(dead_zone_joystick, y_axis_raw)) == 127 and not y_axis_home:
					joystick_speed_setting_toggle = True
					keyboard_speed_setting_toggle = False
					y_axis_home = True
					y_axis_remapped = 0
				elif abs(y_axis_raw - y_axis_pre) > axis_change_th and int(linear_remap_signed(dead_zone_joystick, y_axis_raw)) != 127:
					joystick_speed_setting_toggle = True
					keyboard_speed_setting_toggle = False
					y_axis_home = False
					y_axis_remapped = y_axis_raw
					# y_axis_remapped = int(linear_remap_signed(dead_zone_joystick, y_axis_raw))
			elif dig_button == 1 and dig_button_pre == 0:
				mode = 'digging'
				joystick_speed_setting_toggle = False
				keyboard_speed_setting_toggle = False
				digging_setting_toggle = True
			elif dig_button == 0 and dig_button_pre == 1:
				mode = 'stop'
				joystick_speed_setting_toggle = False
				keyboard_speed_setting_toggle = False
				digging_setting_toggle = True
			elif back_dig_button == 1 and back_dig_button_pre == 0:
				mode = 'back_digging'
				joystick_speed_setting_toggle = False
				keyboard_speed_setting_toggle = False
				digging_setting_toggle = True
			elif back_dig_button == 0 and back_dig_button_pre == 1:
				mode = 'stop'
				joystick_speed_setting_toggle = False
				keyboard_speed_setting_toggle = False
				digging_setting_toggle = True
			elif front_dig_button == 1 and front_dig_button_pre == 0:
				mode = 'front_digging'
				joystick_speed_setting_toggle = False
				keyboard_speed_setting_toggle = False
				digging_setting_toggle = True
			elif front_dig_button == 0 and front_dig_button_pre == 1:
				mode = 'stop'
				joystick_speed_setting_toggle = False
				keyboard_speed_setting_toggle = False
				digging_setting_toggle = True
			if sweep_button_pre != sweep_button:
				if toggle_mode_button == 1:
					toggle_sweep_mode = True
				else:
					toggle_sweep_mode = False
				if pulse_mode_button == 1:
					pulse_sweep_mode = True
				else:
					pulse_sweep_mode = False
				if sweep_button == 1:
					sweep_mode = 'sweep_start'
				elif sweep_button == 0 and toggle_sweep_mode == False and pulse_sweep_mode == False:
					sweep_mode = 'sweep_stop'
				sweep_setting_toggle = True
			if belt_button != belt_button_pre:
				# if belt_button == 1:
				# 	belt_mode = 'belt_fwd'
				# elif belt_button == -1:
				# 	belt_mode = 'belt_bwd'
				# elif belt_button == 0:
				# 	belt_mode = 'belt_stop'
				if belt_button == 1:
					belt_mode = 'belt_fwd'
				elif belt_button == 0:
					belt_mode = 'belt_stop'			
				belt_setting_toggle = True
			if scissor_button_pre != scissor_button:
				if toggle_mode_button == 1:
					toggle_scissor_mode = True
				else:
					toggle_scissor_mode = False
				if scissor_button == 1:
					scissor_mode = 'scissor_up'
				elif scissor_button == -1:
					scissor_mode = 'scissor_down'
				elif scissor_button == 0 and toggle_scissor_mode == False:
					scissor_mode = 'scissor_stop'
				scissor_setting_toggle = True
			sweep_button_pre = sweep_button
			scissor_button_pre = scissor_button
			belt_button_pre = belt_button
			dig_button_pre = dig_button
			front_dig_button_pre = front_dig_button
			back_dig_button_pre = back_dig_button
			x_axis_pre = x_axis_raw
			y_axis_pre = y_axis_raw
	if pulse_status_pre != pulse_status:
		if pulse_sweep_mode:
			if pulse_status == 1:
				sweep_mode = 'sweep_start'
			else:
				sweep_mode = 'sweep_stop'
			sweep_setting_toggle = True
	pulse_status_pre = pulse_status
	if joystick_speed_setting_toggle or keyboard_speed_setting_toggle \
		or digging_setting_toggle or belt_setting_toggle or scissor_setting_toggle or sweep_setting_toggle:
		send_data = []
		if joystick_speed_setting_toggle:
			send_data.append({'name': 'W/R Motor0', 'value': mix_control(x_axis_remapped, y_axis_remapped, 0)})
			send_data.append({'name': 'W/R Motor1', 'value': mix_control(x_axis_remapped, y_axis_remapped, 1)})
			send_data.append({'name': 'W/R Motor2', 'value': mix_control(x_axis_remapped, y_axis_remapped, 2)})
			send_data.append({'name': 'W/R Motor3', 'value': mix_control(x_axis_remapped, y_axis_remapped, 3)})
			joystick_speed_setting_toggle = False
		if keyboard_speed_setting_toggle:
			send_data.append({'name': 'W/R Motor0', 'value': speed_list[mode][0]})
			send_data.append({'name': 'W/R Motor1', 'value': speed_list[mode][1]})
			send_data.append({'name': 'W/R Motor2', 'value': speed_list[mode][2]})
			send_data.append({'name': 'W/R Motor3', 'value': speed_list[mode][3]})
			keyboard_speed_setting_toggle = False
		if digging_setting_toggle:
			send_data.append({'name': 'W/R Motor0', 'value': speed_list[mode][0]})
			send_data.append({'name': 'W/R Motor1', 'value': speed_list[mode][1]})
			send_data.append({'name': 'W/R Motor2', 'value': speed_list[mode][2]})
			send_data.append({'name': 'W/R Motor3', 'value': speed_list[mode][3]})
			digging_setting_toggle = False
		if belt_setting_toggle:
			# send_data.append({'name': 'W/R Servo1', 'value': speed_list[belt_mode]})
			send_data.append({'name': 'W/R Single Switch', 'value': belt_list[belt_mode]})
			belt_setting_toggle = False
		if scissor_setting_toggle:
			send_data.append({'name': 'W/R Servo0', 'value': speed_list[scissor_mode]})
			scissor_setting_toggle = False
		if sweep_setting_toggle:
			send_data.append({'name': 'W/R Servo1', 'value': speed_list[sweep_mode]})
			sweep_setting_toggle = False
		putincommand(send_data)
	connection_status.draw()
	pygame.display.flip()
	pulse_counter += 1
	if pulse_counter > pulse_threshold:
		pulse_counter = 0
		pulse_status = 1 - pulse_status
		# print "pulse_status", pulse_status
	clock.tick(60)
pygame.quit()
