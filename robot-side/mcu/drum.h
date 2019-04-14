// Drum protocol definitions
#define DRUM_MOTOR_1_COMM       0x01
#define DRUM_MOTOR_2_COMM       0x02
#define LINEAR_ACTUATOR_COMM    0x04
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


// Pin mappings
#define DRUM_MOTOR_1            BlueMotor
#define DRUM_MOTOR_2            GoldMotor
#define LINEAR_ACTUATOR         GreenMotorPWM
#define VIBRATION_MOTOR         VibrationMotorEnable
#define DOOR_ACTUATOR           ServoPWM

#define LINEAR_POT              LinearPot
#define MAGNETIC_1              Pullup1
#define MAGNETIC_2              Pullup2
#define WEIGHT                  MiscSensor
#define DRUMENC_1               BlueMotor
#define DRUMENC_2               GoldMotor
#define TOP_LIMIT               Pullup4
#define BOTTOM_LIMIT            Pullup5


// Motor lookup
#ifndef MOTOR_LOOKUP
#define MOTOR_LOOKUP

#define MOTOR_COMM_WIDTH        DOOR_ACTUATOR_COMM + 1

void *motorPinlookup[MOTOR_COMM_WIDTH];
motorPinLookup[DRUM_MOTOR_1_COMM] = &DRUM_MOTOR_1; 
motorPinLookup[DRUM_MOTOR_2_COMM] = &DRUM_MOTOR_2;
motorPinLookup[LINEAR_ACTUATOR_COMM] = &LINEAR_ACTUATOR; 
motorPinLookup[VIBRATION_MOTOR_COMM] = &VIBRATION_MOTOR_COMM;
motorPinlookup[DOOR_ACTUATOR_COMM] = &DOOR_ACTUATOR_COMM; 

#endif

// Sensor lookup
#ifndef SENSOR_LOOKUP
#define SENSOR_LOOKUP

#define SENSOR_COMM_WIDTH       BOTTOM_LIMIT_COMM + 1

void *sensorPinlookup[SENSOR_COMM_WIDTH];
motorPinLookup[LINEAR_POT_COMM] = &LINEAR_POT; 
motorPinLookup[MAGNETIC_1_COMM] = &MAGNETIC_1;
motorPinLookup[MAGNETIC_2_COMM] = &MAGNETIC_2; 
motorPinLookup[WEIGHT_COMM] = &WEIGHT;
motorPinlookup[DRUMENC_1_COMM] = &DRUMENC_1; 
motorPinlookup[DRUMENC_2_COMM] = &DRUMENC_2; 
motorPinlookup[TOP_LIMIT_COMM] = &TOP_LIMIT; 
motorPinlookup[BOTTOM_LIMIT_COMM] = &BOTTOM_LIMIT; 

#endif