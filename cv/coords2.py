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

    #detection left and right
    dL=detector.detect(frames[0])[0]
    dR=detector.detect(frames[1])[0]
    
    x_left, y_left= dL.center
    x_right, y_right= dR.center
    #print dL.corners[0]
    #print dR.corners
    #print "x_left %f x_right %f b %f f %f" % (x_left, x_right, cam_properties.b, cam_properties.f)
    #print "y_left %f y_right %f b %f f %f" % (y_left, y_right, cam_properties.b, cam_properties.f)
    
    colored_frames[0] = cv2.circle(colored_frames[0], (int(x_left), int(y_left)), 15, (0, 0, 255), -1)
    colored_frames[1] = cv2.circle(colored_frames[1], (int(x_right), int(y_right)), 15, (255, 0, 0), -1)
    disparity = abs(x_left - x_right)
    proj_left = np.array([[531.152586, 0.000000, 337.167439, 0.000000], [0.000000, 531.152586, 247.265795, 0.000000], [0.000000, 0.000000, 1.000000, 0.000000]])
    proj_right = np.array([[531.152586, 0.000000, 337.167439, 56.499615], [0.000000, 531.152586, 247.265795, 0.000000], [0.000000, 0.000000, 1.000000, 0.000000]])
    #print np.array([[x_left ], [y_left]]), np.array([[x_right], [y_right]])
    #points4D = cv2.triangulatePoints(proj_left, proj_right, np.array([[x_left], [y_left]]), np.array([[x_right], [y_right]]))
    #print points4D
    pL=np.array([[x_left,dL.corners[0][0],dL.corners[1][0],dL.corners[2][0],dL.corners[3][0]],[y_left,dL.corners[0][1],dL.corners[1][1],dL.corners[2][1],dL.corners[3][1]]])
    pR=np.array([[x_right,dR.corners[0][0],dR.corners[1][0],dR.corners[2][0],dR.corners[3][0]],[y_right,dR.corners[0][1],dR.corners[1][1],dR.corners[2][1],dR.corners[3][1]]])
    # points is in order of C, BL, BR, TR, TL
    points =cv2.triangulatePoints(proj_left, proj_right, pL, pR)
    #print points
    factor = 1.053900 #Factor used to ensure that at the 100 cm that its' around that area 
    for i in range(5):
        points[0][i]/=points[3][i]*factor
        points[1][i]/=points[3][i]*factor
        points[2][i]/=points[3][i]*factor
        points[3][i]/=points[3][i]
    #convert it to 3D point array
    p3D=[[points[0][0],points[1][0],points[2][0]],[points[0][1],points[1][1],points[2][1]],[points[0][2],points[1][2],points[2][2]],[points[0][3],points[1][3],points[2][3]],[points[0][4],points[1][4],points[2][4]]]
    #x=points4D[0]/points4D[3]
    #y=points4D[1]/points4D[3]
    #z=points4D[2]/points4D[3]
    #print p3D
    
    #x=p3D[0][0]
    #y=p3D[0][1]
    #z=p3D[0][2]
    
    #How "straight" is the april tag?
    cvecBL=[p3D[0][0]-p3D[1][0],p3D[0][1]-p3D[1][1],p3D[0][2]-p3D[1][2]]
    cvecBR=[p3D[0][0]-p3D[2][0],p3D[0][1]-p3D[2][1],p3D[0][2]-p3D[2][2]]
    cvecTR=[p3D[0][0]-p3D[3][0],p3D[0][1]-p3D[3][1],p3D[0][2]-p3D[3][2]]
    cvecTL=[p3D[0][0]-p3D[4][0],p3D[0][1]-p3D[4][1],p3D[0][2]-p3D[4][2]]
    
    magBL=math.sqrt(cvecBL[0]*cvecBL[0] + cvecBL[1]*cvecBL[1] + cvecBL[2]*cvecBL[2])
    magBR=math.sqrt(cvecBR[0]*cvecBR[0] + cvecBR[1]*cvecBR[1] + cvecBR[2]*cvecBR[2])
    magTR=math.sqrt(cvecTR[0]*cvecTR[0] + cvecTR[1]*cvecTR[1] + cvecTR[2]*cvecTR[2])
    magTL=math.sqrt(cvecTL[0]*cvecTL[0] + cvecTL[1]*cvecTL[1] + cvecTL[2]*cvecTL[2])


    dot1=(cvecBL[0]*cvecTR[0] + cvecBL[1]*cvecTR[1] + cvecBL[2]*cvecTR[2])
    dot2=(cvecBR[0]*cvecTL[0] + cvecBR[1]*cvecTL[1] + cvecBR[2]*cvecTL[2])
    
    cos1=dot1/magBL/magTR
    cos2=dot2/magBR/magTL
    #ideally the cos should be -1
    #its actually pretty good :O ~98% range
    #print "%f %f" % (cos1,cos2)
    
    magAvg=(magBL+magBR+magTR+magTL)/4
    #ideally the magnitudes should be identical
    #eh good enough ~0.8-1.2 range
    #print "%f %f %f %f" % (magBL/magAvg,magBR/magAvg,magTR/magAvg,magTL/magAvg)
    
    #avg of values
    avgCenter=[0.0,0.0,0.0]
    for i in range(0,5):
        avgCenter[0]+=p3D[i][0]
        avgCenter[1]+=p3D[i][1]
        avgCenter[2]+=p3D[i][2]
    
    avgCenter[0]/=5
    avgCenter[1]/=5
    avgCenter[2]/=5
    inches=39.3701

    #ideally identical
    #steady-low offset ~0.01%-0.05%
    #moving-high offset ~0.10-0.15%
    #print "Center", p3D[0], "Average",avgCenter
    #print "avg %f %f %f" %(p3D[0][0]/avgCenter[0],p3D[0][1]/avgCenter[1],p3D[0][2]/avgCenter[2])
    print "avg %f %f %f" % (avgCenter[0],avgCenter[1],avgCenter[2])
    print "Jin :avg correct units: %f %f %f" % (avgCenter[0]*inches,avgCenter[1]*inches,avgCenter[2]*inches)
    #distance=math.sqrt(x*x+y*y+z*z)
    #print "%f %f %f" % (x, y, z)
    #print "distance?: %f %f" % (z, distance)
    ##global average
    ##average+=avgCenter[2]
    ##print " .-. "
    ##global counter
    ##counter+=1
    ##print "Average: %f" % (average/counter)
    #depth = 1
    focal = 440 #Approximate focal length
    #focal = depth * disparity / cam_properties.b
    #depth = focal * cam_properties.b / disparity
    print "depth: %f, disparity: %f, focal: %f" % (depth, disparity, focal)
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

