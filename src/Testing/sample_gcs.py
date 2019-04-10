import sample_socket
import sample_parser
import pygame
import pygame_gui
from Queue import Queue

def main() :
	gcs_to_robot = Queue(maxsize = -1)
	robot_to_gcs = Queue(maxsize = -1)
	commands = Queue(maxsize = -1)
	
	gcsSocket = sample_socket.SampleSocket("Socket_Thread", 
							gcs_to_robot, 
							robot_to_gcs,
							# remote_ip="192.168.1.100",
							remote_ip="10.100.32.215",
							remote_socket=10010)
	gcsParser = sample_parser.SampleParser("Parser_Thread",
											robot_to_gcs,
											commands,
											gcs_to_robot)

	gcsSocket.init_socket()

	gcsSocket.start()
	gcsParser.start()

	command = 0
	pygame_gui.init()

	old_val=0

	while command != 8913168 : #this is equal to holding "exit" on the keyboard
		new_val = pygame_gui.test_pygame()
		if old_val != new_val:
			old_val = new_val
			command = new_val
			commands.put(str(command))
			print(command)
		#command = raw_input()



	print "Exitting GCS"
	gcsSocket.stop()
	gcsParser.stop()

if __name__ == "__main__" :
	main()
