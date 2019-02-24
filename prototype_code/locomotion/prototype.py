from ComputerVision import ComputerVision
import time

class CameraProperties:

	def __init__(self. b, f):
		self.b = b # Focal length, need to figure out
		self.f = f # Distance between the cameras

class Locomotion:

    def __init__(self):
        self.Flag_Begin = 1
        self.Flag_Collection = 0
        self.Flag_Deposition = 0

	def disengage():
		# TODO: Disengage drummer from robot
		print "Disengaging drummer from robot \n"
		self.Flag_Begin = 0
		self.Flag_Collection = 1

	def align_depBin():
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


	def align_drummer():
		aligned = False
		while not aligned:
			# TODO: Align the robot with the drummer
			aligned = True
		# TODO: Execute drummer storage code, wait for robot storage to be filled
		print "Waiting for storage to be filled \n"
		time.sleep(2)
		self.Flag_Deposition = 1
		self.Flag_Collection = 0

	def do_task():
		if self.Flag_Begin:
			disengage()
		elif self.Flag_Collection:
			align_drummer()
		elif self.Flag_Deposition:
			align_depBin()

	def autopilot(coords):
		arrived = False
		while arrived:
			# TODO: Move toward the coordinates
			print "Moving towards " + coords
			arrived = True
		do_task()

if __name__ == "__main__":

    loc = Locomotion()
	apriltag_coords = [12, 12] # TODO: Get coords of digging location using apriltag and CV
	bin_coords = [0, 0] # TODO: Coords for the deposition bin
	try:
		while True:

	except KeyboardInterrupt:
		pass



	# cam0 = cv2.VideoCapture(0)
	# cam1 = cv2.VideoCapture(1)
	# cams = [cam0, cam1]
	# detector = apriltag.Detector()
	# props = CameraProperties(0.094, 0.0025)
	# cv = ComputerVision()

	# try:
	# 	while True:
	# 		orig_frames = cv.get_frames(cams)
	# 		gray_frames = cv.recolor_frames(orig_frames)
	# 		coords = None
	# 		try:
	# 			coords = cv.get_coordinates(gray_frames, detector, props, orig_frames)
	# 		except Exception as e:
	# 			pass
	#
	# except KeyboardInterrupt:
	# 	cv.close(cams)
