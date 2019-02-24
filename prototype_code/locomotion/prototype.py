# from ComputerVision import ComputerVision
import time

class CameraProperties:

	def __init__(self, b, f):
		self.b = b # Focal length, need to figure out
		self.f = f # Distance between the cameras

class Locomotion:
    def __init__(self):
    	self.Flag_Begin = 1
        self.Flag_Collection = 0
        self.Flag_Deposition = 0

	def disengage(self):
		# TODO: Disengage drummer from robot
		print "Disengaging drummer from robot \n"
		self.Flag_Begin = 0
		self.Flag_Collection = 1

	def align_depBin(self):
		aligned = False
		while not aligned:
			# TODO: Align the robot to the deposition bin
			aligned = True
		# TODO: Call the Deposition code to deposit stuff
		print "Depositing right now in bin\n"
		time.sleep(1)
		print "Finish depositing in bin\n"
		self.Flag_Deposition = 0
		self.Flag_Collection = 1


	def align_drummer(self):
		aligned = False
		while not aligned:
			# TODO: Align the robot with the drummer
			aligned = True
		# TODO: Execute drummer storage code, wait for robot storage to be filled
		print "Waiting for storage to be filled \n"
		time.sleep(2)
		self.Flag_Deposition = 1
		self.Flag_Collection = 0

	def do_task(self):
		if self.Flag_Begin:
			disengage()
		elif self.Flag_Collection:
			align_drummer()
		elif self.Flag_Deposition:
			align_depBin()

	def autopilot(self, coords):
		arrived = False
		while arrived:
			# TODO: Move toward the coordinates
			print "Moving towards " + coords
			time.sleep(2)
			arrived = True
		do_task()

	def get_coordinates(self):
		if self.Flag_Begin:
			return (12, 12) # TODO: Get coords of digging location using apriltag and CV
		elif self.Flag_Collection:
			return (12, 12) # TODO: Coords of digging location
		elif self.Flag_Deposition:
			return (0, 0) # TODO: Coords for the deposition bin

if __name__ == "__main__":
	print "hello"
	loc = Locomotion()
	try:
		while True:
			coords = loc.get_coordinates()
			loc.autopilot(coords)
	except KeyboardInterrupt:
		pass
