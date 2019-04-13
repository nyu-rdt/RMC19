import threading

class SampleParser (threading.Thread) :
	def __init__(self, threadName, input_buffer, command_buffer, output_buffer) :
		self.input_buffer = input_buffer
		self.output_buffer = output_buffer
		self.command_buffer = command_buffer

		super(SampleParser, self).__init__(name = threadName)

	def run(self) :
		while True :
			if not self.input_buffer.empty() :
				print "Received: " + self.input_buffer.get()[0]
			if not self.command_buffer.empty() :
				self.output_buffer.put(self.command_buffer.get())

	def stop(self) :
		self._is_running = False