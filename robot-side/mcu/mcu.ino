// Version 1 of the TEENSY CODE:
// This code is "Good" it works, but the issue is that the use of a timed out phases
// means that some of the instructions are being executed slightly after the other instructions
// due to delays in the propogation of the instructions from the central server to the
// rovers.
// Therefore, the next version of the code should not implement timeouts or block execution
// instead it should dispatch commands upon receiving them.
// Measured latency: < 0.5s per write command

// These are the definiions for the phase numbers
#define READ_PHASE_NO         0
#define PARSE_PHASE_NO        1
#define EXEC_PHASE_NO         2
#define WRITE_PHASE_NO        3

// Include general communications protocol definitions
// This file defines constants for communications
#define START_BYTE_MOTOR        0xF0
#define START_BYTE_SENSOR       0xF1
#define RESPONSE_BYTE_SENSOR    0xF2
#define PANIC_BYTE              0xF3
#define STOP_BYTE               0xFF

// Constants for read phase (parameters)
#define READ_BUFFER_LEN       256
#define READ_TIMEOUT_MILLIS   200  

// constants for parse phase (define byte types)
#define MOTOR_TYPE            1
#define SENSOR_TYPE           2
#define VALUE_TYPE            3
#define STOP_TYPE             4
#define START_TYPE            5

#define SENSOR_DISABLE        0
#define SENSOR_ENABLE         1
#define MOTOR_STOP            0
#define MOTOR_FULL_REVERSE    255
#define MOTOR_FULL_FORWARD    127

// Serial Definitions
#define ESP_BAUD             115200
#define ROBOTEQ_BAUD          115200

// Serial Port Mappings
HardwareSerial ESP = Serial1; // ESP WiFi Module
HardwareSerial BlueMotor = Serial3;
HardwareSerial PurpleMotor = Serial3;
HardwareSerial GoldMotor = Serial2;
HardwareSerial BlackMotor = Serial2;

// How many motors are on a single controller channel (set to two for dual channel controller)
int motorsPerChannel = 1;

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

// Servo Definition

// DRUM COMMUNICATIONS CODES
// Include drum protocol definitions
// Drum protocol definitions
#define DRUM_MOTOR_1_COMM       0x01
#define DRUM_MOTOR_2_COMM       0x02
#define DRUM_LINEAR_ACTUATOR_COMM    0x04
#define VIBRATION_MOTOR_COMM    0x08
#define DOOR_ACTUATOR_COMM      0x10

#define LINEAR_POT_COMM         0x01
#define MAGNETIC_1_COMM         0x02
#define MAGNETIC_2_COMM         0x04
#define WEIGHT_COMM             0x08
#define DRUMENC_1_COMM          0x10
#define DRUMENC_2_COMM          0x20
#define TOP_LIMIT_COMM          0x40
#define BOTTOM_LIMIT_COMM       0x80

// DEPO COMMUNICATIONS CODES
// Include depo protocol definitions
// depo protocol definitions
#define DEPO_MOTOR_1_COMM       0x01
#define DEPO_MOTOR_2_COMM       0x02
#define DEPO_LINEAR_ACTUATOR_COMM    0x04

#define POS_LIMIT_1_COMM        0x01
#define POS_LIMIT_2_COMM        0x02
#define RACK_LIMIT_L_1_COMM     0x04
#define RACK_LIMIT_L_2_COMM     0x08
#define RACK_LIMIT_R_1_COMM     0x10
#define RACK_LIMIT_R_2_COMM     0x20

// LOCO COMMUNICATIONS CODES
// Include loco protocol definitions
// Loco protocol definitions
#define LOCO_MOTOR_LF_COMM      0x01
#define LOCO_MOTOR_LB_COMM      0x02
#define LOCO_MOTOR_RF_COMM      0x04
#define LOCO_MOTOR_RB_COMM      0x08

#define FORKLIFT_LIMIT_1_COMM   0x01
#define FORKLIFT_LIMIT_2_COMM   0x02

// Motor lookup DRUM
#define MOTOR_LOOKUP
#define MOTOR_COMM_WIDTH        DOOR_ACTUATOR_COMM + 1

void *motorPinlookup[MOTOR_COMM_WIDTH];

// Sensor lookup
#define SENSOR_LOOKUP
#define SENSOR_COMM_WIDTH       BOTTOM_LIMIT_COMM + 1

void *sensorPinlookup[SENSOR_COMM_WIDTH]; 

int numMotors;
int numSensors;

// Pin mappings
void initializeDepoMappings();
void initializeLocoMappings();
void initializeDrumMappings();
////////////////////



// Globals
int oPhase = 0;

// Buffers and end-of-buffer pointers
int motorExecBuffer[MOTOR_COMM_WIDTH];
int sensorExecBuffer[SENSOR_COMM_WIDTH];
int sensorValueBuffer[SENSOR_COMM_WIDTH];

// Function prototypes
// They return the number indicator of the next phase to enter
int readPhase();
int parsePhase();
int execPhase();
int writePhase();
/////////////////


// ARDUINO FUNCTIONS
void setup() {
  // start serial ports
  ESP.begin(ESP_BAUD);
  BlueMotor.begin(ROBOTEQ_BAUD);
  GoldMotor.begin(ROBOTEQ_BAUD);

  // Pin modes
  pinMode(Pullup1, INPUT);
  pinMode(Pullup2, INPUT);
  pinMode(Pullup3, INPUT);
  pinMode(Pullup4, INPUT);
  pinMode(Pullup5, INPUT);
  pinMode(Pullup6, INPUT);
  pinMode(DebugLED, OUTPUT);
  pinMode(VibrationMotorEnable, OUTPUT);
  
  pinMode(BlueMotorPWM, OUTPUT);
  pinMode(GoldMotorPWM, OUTPUT);
  pinMode(PurpleMotorPWM, OUTPUT);
  pinMode(BlackMotorPWM, OUTPUT);
    
  // Initialize motorExecBuffer to STOP VALUE
  memset(motorExecBuffer, MOTOR_STOP, MOTOR_COMM_WIDTH);
  // Initialize sensorExecBuffer to all 0
  memset(sensorExecBuffer, SENSOR_DISABLE, SENSOR_COMM_WIDTH);

  // Uncomment line based on robot type
//  initializeDrumMappings();
//    initializeDepoMappings();
    initializeLocoMappings();
}

void loop() {
  // main loop to switch between phases
  parsePhase();
  //  if (oPhase == READ_PHASE_NO) {
  //    oPhase = readPhase();
  //  }
  //  else if (oPhase == PARSE_PHASE_NO) {
  //    oPhase = parsePhase();
  //  }
  //  else if (oPhase == EXEC_PHASE_NO) {
  //    oPhase = execPhase();
  //  }
  //  else if (oPhase == WRITE_PHASE_NO) {
  //    oPhase = writePhase();
  //  }
}

// This phase reads data from the ESP into a buffer
// It ends once the buffer is full OR after a timeout
//int readPhase() {
//  int starttime = millis(); // when the phase started
//  int current = millis(); // current time of the phase
//
//  while (current - starttime < READ_TIMEOUT_MILLIS) {
//    // If there is a character to read
//    if (ESP.available() > 0) {
//      // Write to end of the readBuffer      
//      readBuffer[readBufferEnd] = ESP.read();
//      
//      // Increment end of read buffer
//      ++readBufferEnd;
//      // Reset the timeout
//      delay(10);
//      starttime = millis();
//    }
//    current = millis();
//    // Exit if the readbuffer is full
//    if (readBufferEnd >= READ_BUFFER_LEN) {
//        break;
//    }
//  }
//
//  // If the read buffer is still empty, repeat the current phase
//  if (readBufferEnd == 0) {
//    delay(10);
//    return READ_PHASE_NO;
//  }
//  
//  // Continue into the next phase
//  return PARSE_PHASE_NO;
//}

// This phase will take the received commands from the ESP and extracts the commands
// for each motor
int parsePhase() {
  int nextByteType = START_TYPE; // What is the next byte to expect?
  int motorIdentifier = 0; // What motor to expect a value for?
  int sensorIdentifier = 0; // What motor to expect a value for?
  // variables for recovering incomplete segments
  int endOfSegment = 0;
  int incompleteSegmentStart = 0;

  while (nextByteType != START_TYPE) {
    // For each byte in the read buffer
    while (ESP.available()) {
      int currentByte = ESP.read();
      
      if (nextByteType == START_TYPE && currentByte == START_BYTE_MOTOR ) {
        nextByteType = MOTOR_TYPE;
      }
      else if (nextByteType == START_TYPE && currentByte == START_BYTE_SENSOR) {
        nextByteType = SENSOR_TYPE;
      }
      else if (nextByteType == MOTOR_TYPE) {
        motorIdentifier = currentByte;
        nextByteType = VALUE_TYPE;
      }
      else if (nextByteType == SENSOR_TYPE) {
        // Enable sensor read
        sensorIdentifier = currentByte;
        sensorExecBuffer[sensorIdentifier] = SENSOR_ENABLE;
        nextByteType = STOP_TYPE;
      }
      else if (nextByteType == VALUE_TYPE) {
        // Set motor speed
        motorExecBuffer[motorIdentifier] = currentByte;
        nextByteType = STOP_TYPE;
      }
      else if (nextByteType == STOP_TYPE && currentByte == STOP_BYTE){
        nextByteType = START_TYPE;
        commit(motorIdentifier, sensorIdentifier, motorExecBuffer[motorIdentifier]);
        motorIdentifier = 0;
        sensorIdentifier = 0;
      }
      else {
        nextByteType = START_TYPE;
      }
    }
    // Continue to exec_phase
    return EXEC_PHASE_NO;
  }
}

// Execute phase writes commands to actual electronics
int execPhase() {
  int dataflag = 0; // Whether or not data was read from the sensors
  // Update the motors
  for (int motorIndex = 0; motorIndex < numMotors; ++motorIndex) {
    int i = 1 << motorIndex;
    int motor = *(int *)motorPinlookup[i];
    int power = (int)motorExecBuffer[i];
    unsigned int signalPWM = (unsigned long)power * 205 / 255 + 204; // Equation to map from 0-255 ---> 204-409 (12-bit resolution)
    int signalSerial = (power != 0) * ((power - 128) * 3937)
    
    if ((HardwareSerial *)motorPinlookup[i] == &BlueMotor || (HardwareSerial *)motorPinlookup[i] == &GoldMotor) {
      String chan = String((motorIndex % motorsPerChannel) + 1);
      String power = String(signalSerial);
      (*(HardwareSerial *)motorPinlookup[i]).println("!G " + chan + " " + power + "\r");
      continue;
    }
    // Write to the serial port
    analogWrite(motor, signalPWM);
  }
  // Update the sensors
  for (int sensorIndex = 0; sensorIndex < numSensors; ++sensorIndex) {
    int i = 1 << sensorIndex;
    if (sensorExecBuffer[i] == SENSOR_ENABLE) {
      if (*(int *)sensorPinlookup[i] > A0) { // Analog pin
        sensorValueBuffer[i] = analogRead(*(int *)sensorPinlookup[i]);
      }
      else if (*(HardwareSerial *)sensorPinlookup[i] == GoldMotor || *(HardwareSerial *)sensorPinlookup[i] == BlueMotor) {
        // Serial port, gotta do this
      }
      else {
        sensorValueBuffer[i] = digitalRead(*(int *)sensorPinlookup[i]);
      }
    }
  }

  // If no data was read from the sensors, no need to write back
  if (dataflag == 0) {
    return READ_PHASE_NO;
  }

  return WRITE_PHASE_NO;
}

// This phase write the collected sensor values to the ESP module
int writePhase() {
  int unscaled;
  
  for (int sensorIndex = 0; sensorIndex < numSensors; ++sensorIndex) {
    int i = 1 << sensorIndex;
    if (sensorExecBuffer[i] == SENSOR_ENABLE) {
      unscaled = sensorValueBuffer[i]; // TODO: unscale value from 10-bits to 8-bits
      
      Serial1.write(RESPONSE_BYTE_SENSOR); // Start byte indicating sensor value
      Serial1.write(i); // The sensor identifier
      Serial1.write(unscaled); // The value read from the sensor
      Serial1.write(STOP_BYTE); // End of packet

      sensorExecBuffer[i] = SENSOR_DISABLE;
    }
  }

  return READ_PHASE_NO;
}


// ROVER DEFINITIONS
void initializeDrumMappings() {
  numMotors = 5;
  numSensors = 8;
  
  motorPinlookup[DRUM_MOTOR_1_COMM] = (void *)&BlueMotorPWM; // If using PWM, replace with BlueMotorPWM
  motorPinlookup[DRUM_MOTOR_2_COMM] = (void *)&GoldMotorPWM;
  motorPinlookup[DRUM_LINEAR_ACTUATOR_COMM] = (void *)&GreenMotorPWM;
  motorPinlookup[VIBRATION_MOTOR_COMM] = (void *)&VibrationMotorEnable;
  motorPinlookup[DOOR_ACTUATOR_COMM] = (void *)&ServoPWM;
  
  sensorPinlookup[LINEAR_POT_COMM] = (void *)&LinearPot;
  sensorPinlookup[MAGNETIC_1_COMM] = (void *)&Pullup1;
  sensorPinlookup[MAGNETIC_2_COMM] = (void *)&Pullup2;
  sensorPinlookup[WEIGHT_COMM] = (void *)&MiscSensor;
  sensorPinlookup[DRUMENC_1_COMM] = (void *)&BlueMotor;
  sensorPinlookup[DRUMENC_2_COMM] = (void *)&GoldMotor;
  sensorPinlookup[TOP_LIMIT_COMM] = (void *)&Pullup4;
  sensorPinlookup[BOTTOM_LIMIT_COMM] = (void *)&Pullup5;
}

void initializeDepoMappings() {
  numMotors = 3;
  numSensors = 6;
  
  motorPinlookup[DEPO_MOTOR_1_COMM] = (void *)&BlueMotor; // If using PWM, replace with BlueMotorPWM
  motorPinlookup[DEPO_MOTOR_2_COMM] = (void *)&GoldMotor;
  motorPinlookup[DEPO_LINEAR_ACTUATOR_COMM] = (void *)&GreenMotorPWM;
  
  sensorPinlookup[POS_LIMIT_1_COMM] = (void *)&Pullup1;
  sensorPinlookup[POS_LIMIT_2_COMM] = (void *)&Pullup2;
  sensorPinlookup[RACK_LIMIT_L_1_COMM] = (void *)&Pullup3;
  sensorPinlookup[RACK_LIMIT_L_2_COMM] = (void *)&Pullup4;
  sensorPinlookup[RACK_LIMIT_R_1_COMM] = (void *)&Pullup5;
  sensorPinlookup[RACK_LIMIT_R_2_COMM] = (void *)&Pullup6;
}

void initializeLocoMappings() {
  numMotors = 4;
  numSensors = 2;
  motorsPerChannel = 2; // Locomotion uses 2 motors per channel
  
  motorPinlookup[LOCO_MOTOR_LF_COMM] = (void *)&BlueMotorPWM; // If using PWM, replace with BlueMotorPWM
  motorPinlookup[LOCO_MOTOR_LB_COMM] = (void *)&BlueMotor;
  motorPinlookup[LOCO_MOTOR_RF_COMM] = (void *)&GoldMotor;
  motorPinlookup[LOCO_MOTOR_RB_COMM] = (void *)&GoldMotor;
  
  sensorPinlookup[FORKLIFT_LIMIT_1_COMM] = (void *)&Pullup1;
  sensorPinlookup[FORKLIFT_LIMIT_2_COMM] = (void *)&Pullup2;
}
