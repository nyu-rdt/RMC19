
class digging:
    def __init__(self, storage):
        #from 0 to the lowest point (increasing)
        self.curr_pos = 0
        self.lowest_pos = 90000000000
        self.curr_weight = storage
        self.target_weight = 90000000000 #how much we can target/get
        # when rack is false, it is at original position
        self.full = False
        self.stuck = False
        self.drum_rate = 0 #the rate that the deposition is changing
        self.actuator_speed = 0 #if the weight is constant

    def actuator(self):
        if self.full :
            print "Actuator going up all the way"
        elif self.stuck:
            print "Actuator going up a little bit"
            self.curr_pos -= 1 #some constant delta
        else:
            print "Actuator going down"
            self.curr_pos += 0.5*self.drum_rate #some constant that works based on digging rate

    def motor(self, target_speed = 5):
        #target speed has a default speed, but sometimes storage might need to manually set it
        if self.full:
            print "stop the motor"
            self.drum_rate = 0
        elif self.stuck:
            self.drum_rate = 1 #some constant that allows not breaking the drum while going up
        else:
            self.drum_rate = target_speed #some constant that works

    def limit_switch(self, cens_input = False):
        self.stuck = cens_input

    def storage(self, new_weight):
        self.weight = new_weight
        if new_weight >= self.target_weight:
            self.full = True
        else:
            self.full = False


        
def main():
    print "Hello World! \n"
    test = digging(5)
    test.motor()
    test.actuator()
    test.storage(10)
    test.limit_switch()
    

    

if __name__=="__main__":
    main()  