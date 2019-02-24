# from ComputerVision import ComputerVision
import time
import rospy
from std_msgs.msg import String

class locomotion:
	def __init__(self):
		self.Flag_Begin = 1
		self.Flag_Collection = 0
		self.Flag_Deposition = 0

		rospy.init_node('listener', anonymous=True)
		rospy.Subscriber("coords_get", String, callback)
		rospy.Subscriber("state_get", String, callback)

	def disengage(self):
		# TODO: Disengage drummer from robot
		print "Disengaging drummer from robot"
		self.Flag_Begin = 0
		self.Flag_Collection = 1

	def align_depBin(self):
		aligned = False
		while not aligned:
			# TODO: Align the robot to the deposition bin
			aligned = True
		# TODO: Call the Deposition code to deposit stuff
		print "Depositing right now in bin"
		time.sleep(1)
		print "Finish depositing in bin"
		self.Flag_Deposition = 0
		self.Flag_Collection = 1

	def align_drummer(self):
		aligned = False
		while not aligned:
			# TODO: Align the robot with the drummer
			aligned = True
		# TODO: Execute drummer storage code, wait for robot storage to be filled
		print "Waiting for storage to be filled"
		time.sleep(2)
		self.Flag_Deposition = 1
		self.Flag_Collection = 0

	def do_task(self):
		if self.Flag_Begin:
			self.disengage()
		elif self.Flag_Collection:
			self.align_drummer()
		elif self.Flag_Deposition:
			self.align_depBin()

	def autopilot(self, coords):
		arrived = False
		while not arrived:
			# TODO: Move toward the coordinates
			print "Moving towards " + ", ".join(str(x) for x in coords)
			time.sleep(2)
			arrived = True
		self.do_task()

	def get_coordinates(self):
		if self.Flag_Begin:
			return [12, 12] # TODO: Get coords of digging location using apriltag and CV
		elif self.Flag_Collection:
			return [12, 12] # TODO: Coords of digging location
		elif self.Flag_Deposition:
			return [0, 0] # TODO: Coords for the deposition bin

def main():
	try:
		loc = locomotion()
		while True:
			coords = loc.get_coordinates()
			loc.autopilot(coords)
	except KeyboardInterrupt:
		pass

if __name__ == "__main__":
	main()
