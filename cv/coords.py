import cv2
import apriltag
import turtle

class CameraProperties:
    def __init__(self, b, f):
        self.b = b
        self.f = f


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
        cv2.imshow("test", new_frame)
    return output

def get_coordinates(frames, detector, cam_properties):
    if len(frames) != 2:
        return None
    x_left = detector.detect(frames[1])[0].center[0]
    x_right = detector.detect(frames[0])[0].center[0]
    print "x_left %f x_right %f b %f f %f" % (x_left, x_right, cam_properties.b, cam_properties.f)
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
    return coord[0] * 150, coords[1] * 60000000

def display_coords(coords, t):
    x, y = scale(coords)
    print ("x: %f -- z: %f" % (coords[0], coords[1]))
    print ("scaled x: %f -- z: %f" % (x, y))
    t.goto(x, y) 

if __name__ == "__main__":
    t = turtle.Turtle()
    t.color("purple")
    t.width(5)
    props = CameraProperties(0.093, 0.002)
    print props.b
    print "b %f f %f" % (props.b, props.f)
    detector = apriltag.Detector() 
    cam0 = cv2.VideoCapture(0)
    cam1 = cv2.VideoCapture(1)
    cams = [cam0, cam1]
    
    try: 
        while True: 
            orig_frames = get_frames(cams)
            gray_frames = recolor_frames(orig_frames)
            coords = None
            try:
                coords = get_coordinates(gray_frames, detector, props)
            except Exception as e:
                pass 
                #print e
            if coords is not None:
                display_coords(coords, t)

    except KeyboardInterrupt as k:
        close(cams)

