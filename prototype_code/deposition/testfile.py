class deposition:
	def __init__(self):
		self.vibration = False
		# position z is when storage is ready to deposit gravel
		self.pos_z = 45
		# position x is when we deposit to the bin
		self.pos_x = 55
		# position y is the lowest bin position
		self.pos_y = 0
		self.curr_pos = 0
		self.weight = 50
		self.weight_target = 90000000000 #how much we can target/get
		self.accetable_weight = 2 #how much the weight can sense when fully empty
		# when rack is false, it is at original position
		self.rack = False
		self.rate = .07 #the rate that the deposition is changing
		self.rate_stall = 0.9999999 #if the weight is constant

	def actuator(self, target_pos):
		diff = self.curr_target - target_pos
		if diff >=  0:
			print "Actuator going up"
		else:
			print "Actuator going down"

	def vibration(self):
		if self.vibration:
			print "Turning off vibration"
			self.vibration = False
		else:
			print "Turning on vibration"
			self.vibration = True

	def deposition(self, purpose):
		if purpose == "collect":
			self.actuator(self.pos_z)
			self.vibration()
			while(self.weight<self.weight_target):
				self.weight=update() #getting update from the weight
			self.vibration()
		elif purpose == "deposit":
			self.actuator(self.pos_x)
			self.push()
			weight_five_seconds_ago=self.weight
			while(self.weight>self.acceptable_weight):
				self.weight=update() #getting update from the weight
				rate =(self.weight-weight_give_seconds_Ago)/weight_give_seconds_ago
				elif(rate<=self.rate_stall):
					push("Attack")
			self.push()
		self.actuator.pos_y()

	def push(self,attack_mode=''):
		if(attack_mode=="Attack"):
			print "GO BACK AND FOWARD"
		elif self.rack:
			print "Not Rack and pinion pushing"
			self.rack=False
		else:
			print "Rack and pinion pushing"
			self.rack=True
		
	
		
		 
		
    		
