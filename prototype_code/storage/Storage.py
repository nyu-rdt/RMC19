#define stuff using rosparams

ROBOT_WEIGHT_THRESHHOLD=10 #in kilograms
ROBOT_INFRARED_THRESHOLD=0.75 # percentage of max distance required for infrared to go off

class storage:
    def __init__(self, infraredParams, infraredCount, weightParams):
        self.infraredParams=infraredParams #parameters for converting infrared input and the threshold distance
        self.infrared=[0]*infraredCount
        self.infraredCount=infraredCount
        self.weightParams=weightParams #parameters for converting weight input to usable value and the threshold
        self.weight=0
        self.status="empty"

    def infraredToDistance(self,input): #stores an array of inputs from infrared
        for i in range(len(input)):
            self.infraredToDistance(input[i],i)

    def infraredToDistance(self,input,index): #converts infrared input to distance
        self.infrared[index]=input #idk the logic right now

    def infraredVal(self):
        val=0
        for i in range(len(self.infraredCount)):
            if self.infrared<ROBOT_INFRARED_THRESHOLD*self.infraredParams: # only parameter right now is the distance of sensor
                val+=1 # maybe use a floating point value based on how close to full it is
        return val

    def weightConversion(self,input):
        self.weight=input #idk the logic right now

    def updateStatus(self):
        #update weight and infrared information
        val =self.infraredVal()

        if self.weight>ROBOT_WEIGHT_THRESHHOLD or val>=self.infraredCount:
            self.status="full"
        elif self.weight>ROBOT_WEIGHT_THRESHHOLD*0.5 or val>=self.infraredCount*0.5:
            status="nearly full"
        elif self.weight>ROBOT_WEIGHT_THRESHHOLD*0.1 or val>=self.infraredCount*0.1:
            status="filling"
        else:
            self.status = "empty"

    def dump(self):
        print "dumping stuff"

    def closedump(self):
        print "closing dump"

def main():
    s=storage([],4,[])

if __name__=="__main__":
    print "Hello \n"
    main()