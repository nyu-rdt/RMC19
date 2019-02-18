import cv2
import apriltag

class CameraProperties:

	def __init__(self. b, f):
		self.b = b # Focal length, need to figure out
		self.f = f # Distance between the cameras

class ComputerVision:

	def get_frames(cams):
	    output = []
	    for cam in cams:
	        ret, frame = cam.read()
	        if ret:
	            output.append(frame)
	    return output

	def recolor_frames(frames):
	    output = []
	    for f in frames:
	        new_frame = cv2.cvtColor(f, cv2.COLOR_BGR2GRAY)
	        output.append(new_frame)
	    return output

	def get_coordinates(frames, detector, cam_properties, colored_frames):
	    if len(frames) != 2:
	        return None
	    x_left, y_left = detector.detect(frames[0])[0].center
	    x_right, y_right = detector.detect(frames[1])[0].center
	    print "x_left %f x_right %f b %f f %f" % (x_left, x_right, cam_properties.b, cam_properties.f)

	    colored_frames[0] = cv2.circle(colored_frames[0], (int(x_left), int(y_left)), 15, (0, 0, 255), -1)
	    colored_frames[1] = cv2.circle(colored_frames[1], (int(x_right), int(y_right)), 15, (255, 0, 0), -1)

	    x_left = abs(960 - x_left)
	    x_right = abs(960 - x_right)

	    z = (cam_properties.b  * cam_properties.f) / (abs(x_left - x_right))
	    x = (z / cam_properties.f) * ((x_left + x_right) / 2)
	    print "x %f z %f" % (x ,z)
	    return (x, z)

	def close(cams):
	    for c in cams:
	        c.release()
	    cv2.destroyAllWindows()

	def scale(coord):
	    #return coord[0] * 150, coords[1] * 60000000
	    x = coord[0]
	    y = coord[1]
	    x = 150 - (x * 150)
	    y = 300 - (y * 80000000)
	    x *= 3
	    y *= 3
	    return x, y

	def display_coords(coords, t):
	    x, y = scale(coords)
	    print ("x: %f -- z: %.10f" % (coords[0], coords[1]))
	    print ("scaled x: %f -- z: %f" % (x, y))
	    t.goto(x, y)

class Locomotion:

    def __init__(self):
        self.Flag_Begin = 1
        self.Flag_Collection = 0
        self.Flag_Deposition = 0

	def align_depBin():
		aligned = False
		while not aligned:
			# TODO: Align the robot to the deposition bin
			aligned = True
		deposit();

	def deposit():
		done = False
		while not done:
			# TODO: Code for deposition, wait for it to be done
			done = True # TODO: change later to correspond with deposition team output
		self.Flag_Deposition = 0
		self.Flag_Collection = 1

	def align_drummer():
		done = False
		while not done:
			# TODO: Align the robot with the digging area
			done = True # TODO: Wait for storage to be deposited, get value from storage subsystem
		self.Flag_Deposition = 1
		self.Flag_Collection = 0

if __name__ == "__main__":

    loc = Locomotion()
	cam0 = cv2.VideoCapture(0)
	cam1 = cv2.VideoCapture(1)
	cams = [cam0, cam1]
	detector = apriltag.Detector()
	props = CameraProperties(0.094, 0.0025)
	cv = ComputerVision()

	try:
		while True:
			orig_frames = cv.get_frames(cams)
			gray_frames = cv.recolor_frames(orig_frames)
			coords = None
			try:
				coords = cv.get_coordinates(gray_frames, detector, props, orig_frames)
			except Exception as e:
				pass
			
	except KeyboardInterrupt:
		cv.close(cams)
