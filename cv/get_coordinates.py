import cv2
import apriltag


cap = cv2.VideoCapture(0) # One with stand
cap2 = cv2.VideoCapture(2) # Without stand
detector = apriltag.Detector()
f = 0.002 # Focal length, need to figure out
b = 0.1 # Distance between the cameras

while(True):
   ret, frame = cap.read()
   ret2, frame2 =  cap2.read()
   gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
   gray2 = cv2.cvtColor(frame2, cv2.COLOR_BGR2GRAY)

   xl1 = detector.detect(gray2)[0].center[0]
   xr1 = detector.detect(gray)[0].center[0]

   xl = abs(960-xl1)
   xr = abs(960-xr1)

   z = ((b * f) / (abs(xl - xr)))
   x = ((z / f) * ((xl +  xr)/2))

   print('x : ', x,', z : ',z)

cap.release()
cv2.destroyAllWindows()
