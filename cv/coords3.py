import cv2
import apriltag
import time
import numpy as np
import math

##average=0.0 to be or not to be calibrating factor
##counter=0.0
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
    return output

def get_coordinates(frames, detector, cam_properties, colored_frames):
    if len(frames) != 2:
        return None

    #all detections left and right
    dLA=detector.detect(frames[0])
    dRA=detector.detect(frames[1])
    
    #projection matricies replace with a file parser and global/parameter later
    proj_left = np.array([[531.152586, 0.000000, 337.167439, 0.000000], [0.000000, 531.152586, 247.265795, 0.000000], [0.000000, 0.000000, 1.000000, 0.000000]])
    proj_right = np.array([[531.152586, 0.000000, 337.167439, 56.499615], [0.000000, 531.152586, 247.265795, 0.000000], [0.000000, 0.000000, 1.000000, 0.000000]])
    #find match-ups of detections and insert into array of tuples
    matches=[]
    print "%d %d"%(len(dLA),len(dRA))
    for i in range(len(dLA)):
        for j in range(len(dRA)):
            if(dLA[i].tag_id==dRA[j].tag_id):
                print "%s %d %d" % (dRA[j].tag_id,i,j)
                matches.append((dRA[j].tag_id,i,j))#stores id,left, right
    pL=[[],[]]
    pR=[[],[]]
    for i in range(len(matches)):
        dL=dLA[matches[i][1]]
        dR=dRA[matches[i][2]]
        x_left, y_left= dL.center
        x_right, y_right= dR.center
        #I really feel like removing this
        colored_frames[0] = cv2.circle(colored_frames[0], (int(x_left), int(y_left)), 2, (0, 0, 255), -1)
        colored_frames[1] = cv2.circle(colored_frames[1], (int(x_right), int(y_right)), 2, (255, 0, 0), -1)
    
        pL[0]+=[x_left,dL.corners[0][0],dL.corners[1][0],dL.corners[2][0],dL.corners[3][0]]
        pL[1]+=[y_left,dL.corners[0][1],dL.corners[1][1],dL.corners[2][1],dL.corners[3][1]]
    
        pR[0]+=[x_right,dR.corners[0][0],dR.corners[1][0],dR.corners[2][0],dR.corners[3][0]]
        pR[1]+=[y_right,dR.corners[0][1],dR.corners[1][1],dR.corners[2][1],dR.corners[3][1]]
    
    # points is in order of C, BL, BR, TR, TL per matrix
    pL=np.array(pL)
    pR=np.array(pR)
    points =cv2.triangulatePoints(proj_left, proj_right, pL, pR)
    #print points
    factor = 1.053900 #Factor used to ensure that at the 100 cm that its' around that area
    p3D=[]
    for i in range(len(matches)):
        for j in range(5):
            p3D.append((points[0][i*5+j]/points[3][i*5+j]*factor,points[1][i*5+j]/points[3][i*5+j]*factor,points[2][i*5+j]/points[3][i*5+j]*factor))
            #print p3D[i*5+j]
    #get averages
    points=[]
    for i in range(len(matches)):
        points.append([0.0,0.0,0.0])
        for j in range(0,5):
            points[i][0]+=p3D[i*5+j][0]
            points[i][1]+=p3D[i*5+j][1]
            points[i][2]+=p3D[i*5+j][2]
        points[i][0]/=5
        points[i][1]/=5
        points[i][2]/=5
    #I prefer metric
    for i in range(len(points)):
        print "avg %f %f %f" % (points[i][0],points[i][1],points[i][2])
    
    x_left = abs(960 - x_left)
    x_right = abs(960 - x_right)
    
    z = (cam_properties.b  * cam_properties.f) / (abs(x_left - x_right)) 
    x = (z / cam_properties.f) * ((x_left + x_right) / 2)
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

def display_coords(coords):
    x, y = scale(coords)
    print ("x: %f -- z: %.10f" % (coords[0], coords[1]))
    print ("scaled x: %f -- z: %f" % (x, y))

if __name__ == "__main__":
    props = CameraProperties(0.094, 0.0025)
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
                coords = get_coordinates(gray_frames, detector, props, orig_frames)
            except Exception as e:
                #print e
                pass
            cv2.imshow('left', orig_frames[0])
            cv2.imshow('right', orig_frames[1])
            if cv2.waitKey(1) & 0xff == ord('q'):
                close(cams)
            if coords is not None:
                display_coords(coords)
            time.sleep(0.01)
    except KeyboardInterrupt:
        close(cams)

