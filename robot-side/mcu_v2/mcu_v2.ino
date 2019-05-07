// Version 2 of the TEENSY CODE
// To get around the latencies of the previous version of the code
// this version uses a interrupt driven code. Essentially, the main
// loop is continually updating sensor values and when the Teensy
// receives data from the ESP, it interrupts the sensor reading and
// updates the motors. 
// Measured latency: UNKNOWN (yet to be tested)

// Communications Codes (rover agnostic)
// COMMANDS
#define WRITE_OUTPUT        0xF0
#define READ_INPUT          0xF1
#define READ_OUTPUT         0xF2
#define REPLY_SENSOR        0xF3
#define REPLY_MOTOR         0xF4
#define PANIC               0xF5
#define STOP                0xFF
// OUTPUT ARGUMENTS
#define GREEN_MOTOR_CHAN_1  0x01
#define BLUE_MOTOR_CHAN_1   0x02
#define BLUE_MOTOR_CHAN_2   0x04
#define GOLD_MOTOR_CHAN_1   0x08
#define GOLD_MOTOR_CHAN_2   0x10
#define SERVO_MOTOR         0x20
#define VIBRATION_MOTOR     0x40
#define DEBUG_LED_ENABLE    0x80
#define NUM_OUTPUTS         8
// INPUT ARGUMENTS
#define PULLUP_1            0x01
#define PULLUP_2            0x02
#define PULLUP_3            0x04
#define PULLUP_4            0x08
#define PULLUP_5            0x10
#define PULLUP_6            0x20
#define LINEAR_POT          0x40
#define NUM_INPUT           8
// ARGUMENT LENGTH
#define ARGUMENT_LENGTH     0x80

// constants for parse phase (define byte types)
#define MOTOR_TYPE            1
#define SENSOR_TYPE           2
#define VALUE_TYPE            3
#define STOP_TYPE             4
#define START_TYPE            5

// Pin Mappings
// Motors
int GreenMotorPWM = 3;
int PurpleMotorPWM = 4;
int BlueMotorPWM = 4;
int GoldMotorPWM = 5;
int BlackMotorPWM = 5;
int VibrationMotorEnable = 20;
int ServoPWM = 23;
// Sensors
int LinearPot = 14;
int Pullup1 = 15;
int Pullup2 = 16;
int Pullup3 = A5;
int Pullup4 = 25;
int Pullup5 = 26;
int Pullup6 = 12;
int MiscSensor = A14;
// Misc
int DebugLED = 13;

// Serial definitions
HardwareSerial *ESP = &Serial1; // ESP WiFi Module
HardwareSerial *GoldMotor = &Serial2;
HardwareSerial *BlueMotor = &Serial3;
#define ESP_BAUDRATE        115200
#define ROBOTEQ_BAUDRATE    115200

// These constants set whether or not the system should use SERIAL
// or PWM to write to the motor outputs (BLUE and GOLD motors)
#define USE_SERIAL          1
#define USE_PWM             2

// Buffers
// pinMappings holds the mapings between the comm code and the pin
void *pinMappings[0xFF];
// motorBuffer holds the current status of each motor
int motorBuffer[0xFF];
// sensorBuffer holds the current status of each sensor
int sensorBuffer[0xFF];

void setup() {
  // Begin listening on serial ports
  ESP->begin(ESP_BAUDRATE);
  BlueMotor->begin(ROBOTEQ_BAUDRATE);
  GoldMotor->begin(ROBOTEQ_BAUDRATE);

  // Pin modes
  pinMode(Pullup1, INPUT);
  pinMode(Pullup2, INPUT);
  pinMode(Pullup3, INPUT);
  pinMode(Pullup4, INPUT);
  pinMode(Pullup5, INPUT);
  pinMode(Pullup6, INPUT);
  pinMode(DebugLED, OUTPUT);
  pinMode(VibrationMotorEnable, OUTPUT);

  // Initialize motorBuffer to 0
  memset(motorBuffer, 0, ARGUMENT_LENGTH);
  // Initialize sensorBuffer to all 0
  memset(sensorBuffer, 0, ARGUMENT_LENGTH);
  // Initialize pinMappings to all 0
  memset(pinMappings, 0, ARGUMENT_LENGTH);
  // Map pins, replace argument with USE_PWM to control motors with PWM
  mapPins(USE_SERIAL);
//  mapPins(USE_PWM);

  // Wait for motor controllers to turn on...
  delay(5000);
}

void loop() {
  // All the main loop does is update the sensors
  commandRoutine();
}

// This function (run in a loop) continually updates all of the sensors
void updateInputs () {
  
}

// This function (run in a loop) continually listens for commands
void commandRoutine () {
  int nextByteType = START_TYPE; // What is the next byte to expect?
  int motorIdentifier = 0; // What motor to expect a value for?
  int sensorIdentifier = 0; // What motor to expect a value for?
  // variables for recovering incomplete segments
  int endOfSegment = 0;
  int incompleteSegmentStart = 0;

  // Keep reading until you read the STOP BYTE (don't leave a command hanging)
  do
  {
    // For each byte in the read buffer
    while (ESP->available() > 0) {
      int currentByte = ESP->read();
      
      if (nextByteType == START_TYPE && currentByte == WRITE_OUTPUT) {
        nextByteType = MOTOR_TYPE;
      }
      else if (nextByteType == START_TYPE && currentByte == READ_INPUT) {
        nextByteType = SENSOR_TYPE;
      }
      else if (nextByteType == START_TYPE && currentByte == PANIC) {
        panic();
      }
      else if (nextByteType == MOTOR_TYPE) {
        motorIdentifier = currentByte;
        nextByteType = VALUE_TYPE;
      }
      else if (nextByteType == SENSOR_TYPE) {
        // Enable sensor read
        sensorIdentifier = currentByte;
        nextByteType = STOP_TYPE;
      }
      else if (nextByteType == VALUE_TYPE) {
        // Set motor speed
        motorBuffer[motorIdentifier] = currentByte;
        nextByteType = STOP_TYPE;
      }
      else if (nextByteType == STOP_TYPE && currentByte == STOP){
        nextByteType = START_TYPE;
        commit(motorIdentifier, sensorIdentifier, motorBuffer[motorIdentifier]);
        motorIdentifier = 0;
        sensorIdentifier = 0;
      }
      else {
        nextByteType = START_TYPE;
      }
    }
  } while (nextByteType != START_TYPE);
}

// This function commits changes to the robot state
void commit(int motor, int sensor, int value) {
  if (motor != 0) {
    // A motor
    writeMotor(motor, value);
  }
  else if (sensor != 0) {
    // A sensor 
    readSensor();
  }
}

// Write a value to a motor
void writeMotor(int motorNum, int power) {
  void *motor = pinMappings[motorNum];
  if (((HardwareSerial *)motor == BlueMotor) || ((HardwareSerial *)motor == GoldMotor)) {
    int signalSerial = (power != 0) * ((power - 128) * 3937);
    String chan = String((((motorNum == BLUE_MOTOR_CHAN_2) || (motorNum == GOLD_MOTOR_CHAN_2)) + 1));
    String power = String(signalSerial);
    ((HardwareSerial *)motor)->println("!G " + chan + " " + power + "\r");
  }
  else {
    // Write to the serial port
    int signalPWM = power;
//    unsigned int signalPWM = (unsigned long)power * 205 / 255 + 204; // Equation to map from 0-255 ---> 204-409 (12-bit resolution)
    analogWrite(*(int *)motor, signalPWM);
  }
}

void readSensor() {
  // Not yet implemented.
}

// This functions sends a reply to the ESPs
int reply(int replyCode, int value) {
  // TODO 
}

// This function stops ALL OUTPUTS. Used during disconnect
void panic() {
  for (int i = 0; i < NUM_OUTPUTS; ++i) {
    writeMotor(1 << i, 0);
  }
}

// This function maps the communication codes to the pins
void mapPins(int isSerial) {
  if (isSerial == USE_SERIAL) {
    pinMappings[BLUE_MOTOR_CHAN_1] = (void *)BlueMotor;
    pinMappings[BLUE_MOTOR_CHAN_2] = (void *)BlueMotor ;
    pinMappings[GOLD_MOTOR_CHAN_1] = (void *)GoldMotor;
    pinMappings[GOLD_MOTOR_CHAN_2] = (void *)GoldMotor;
  }
  else {
    // Multiple channels are not supported using PWM
    pinMappings[BLUE_MOTOR_CHAN_1] = (void *)&BlueMotorPWM;
    pinMappings[BLUE_MOTOR_CHAN_2] = 0;
    pinMappings[GOLD_MOTOR_CHAN_1] = (void *)&GoldMotorPWM;
    pinMappings[GOLD_MOTOR_CHAN_2] = 0;
  }
  // Normal output mappings
  pinMappings[SERVO_MOTOR] = (void *)&ServoPWM;
  pinMappings[VIBRATION_MOTOR] = (void *)&VibrationMotorEnable;
  pinMappings[DEBUG_LED_ENABLE] = (void *)&DebugLED;
  pinMappings[GREEN_MOTOR_CHAN_1] = (void *)&GreenMotorPWM;
  // Sensor mappings (XORed by 0xFF)
  pinMappings[0xFF ^ PULLUP_1] = (void *)&Pullup1;
  pinMappings[0xFF ^ PULLUP_2] = (void *)&Pullup2;
  pinMappings[0xFF ^ PULLUP_3] = (void *)&Pullup3;
  pinMappings[0xFF ^ PULLUP_4] = (void *)&Pullup4;
  pinMappings[0xFF ^ PULLUP_5] = (void *)&Pullup5;
  pinMappings[0xFF ^ PULLUP_6] = (void *)&Pullup6;
  pinMappings[0xFF ^ LINEAR_POT] = (void *)&LinearPot;
}
