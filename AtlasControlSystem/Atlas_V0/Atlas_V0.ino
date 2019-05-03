/*************************************************************************
 * Code by Troy Wang
 * Version: V0.2
 * Update Time: 01/10/2017
 * CAUTIONS:
 * 1. Any servo libray cannot be used based on my code.
 * 2. Any serial port library cannot be used based on my code.
 * IMPORTANT COMMENTS:
 * 1. Timer3, Timer4 and Timer5 are used for ESCs control and servo signal generation. 
 *    Desire freq: 50Hz (20ms)
 * 2. Timer2 COMPA is used for system status update: e.g. Motor speed refresh,
 *    servo position refresh, battery voltage and current read, etc. Desire 
 *    freq: 100Hz (10ms)
 * 3. Timer2 COMPA is also used for failsafe.
 * 4. Timer0 is still avaliable for Arduino time functions(e.g. delay()).
 * 5. Motor0 : Pin  -> PWM3A  -> pin 5  back right
 *    Motor1 : Pin  -> PWM3B  -> pin 2  front right
 *    Motor2 : Pin  -> PWM3C  -> pin 3  Front left
 *    Motor3 : Pin  -> PWM4A  -> pin 6  back left
 *    Servo0 : Pin  -> PWM4B  -> pin 7  scissor lift
 *    Servo1 : Pin  -> PWM4C  -> pin 8  sweep motor
 *    Servo2 : Pin  -> PWM5A  -> pin 46
 *    Servo3 : Pin  -> PWM5B  -> pin 45
 *    Servo4 : Pin  -> PWM5C  -> pin 44
 *    limitswitch pin 29-30
 *    SW0 : pin 31 belt motor
 *    SW1 : pin 32
 *    SW2 : pin 33
 *    SW3 : pin 34
 *    SW4 : pin 35
 *    SW5 : pin 36
 *    SW6 : pin 37
 *    SW7 : pin 38
*************************************************************************/

#include "atlasParser.h"
#include "serialISR.h"
#include "timerConfig.h"
#include "pwm.h"
#include "indicator.h"
#include "limit_switch.h"
#include "switches.h"

extern unsigned char recvPackageComplete;
extern unsigned char recvBuffer[256];
extern unsigned char globalRefreshReady;
extern unsigned char failSafeFlag;
extern unsigned char failSafeHandled;
extern unsigned char failSafeRecovered;

AtlasComm comm1;
AtlasPWM myPWM;
SysTick mySysTick;
Indicator myIndicator;
LimitSwitch myls;
Switches mySwitch;
void emergencyStop(){
  int i;
  for(i = 0; i < 4; i++){
    myPWM.setMotor(i, 127);
    comm1.setMotor(i, 127);
  }
  for(i = 0; i < 2; i++){
    myPWM.setServo(i, 127);
    comm1.setServo(i, 127);
  }
  mySwitch.setSwitch(0, 0);
  mySwitch.setSwitch(1, 0);
}

void initDataArr(){
  int i;
  for(i = 0; i < 4; i++)
    comm1.setMotor(i, 127);
  for(i = 0; i < 2; i++)
    comm1.setServo(i, 127);
}

void setup() {
  // put your setup code here, to run once:
  usartInit();
  comm1.setVoltage(0, 10000);
  myPWM.init();
  myPWM.start();
  mySysTick.init();
  mySysTick.start();
  initDataArr();
  emergencyStop();
  interrupts();
}

void checkLimitSwitch(){
  unsigned char ls_status = 0;
  unsigned char value = 0;
  ls_status = myls.checkLimits();
  if(ls_status == 3){
    myPWM.setServo(0, 127);
  }
  else if(ls_status == 0){
    myPWM.setServo(0, comm1.getServo(0));
  }
  else if(ls_status == 1){
    value = comm1.getServo(0);
    if(value >= 0 && value <= 127)
      myPWM.setServo(0, value);
    else
      myPWM.setServo(0, 127);
  }
  else if(ls_status == 2){
    value = comm1.getServo(0);
    if(value >= 127 && value < 256)
      myPWM.setServo(0, value);
    else
      myPWM.setServo(0, 127);
  }
}

void refreshMotors(){
  unsigned char i;
  for(i = 0; i < 4; i++)
    myPWM.setMotor(i, comm1.getMotor(i));
}

void refreshServos(){
  unsigned char i;
  for(i = 0; i < 5; i++)
    myPWM.setServo(i, comm1.getServo(i));
  checkLimitSwitch();
}

void refreshSwitches(){
  // mySwitch.setAllSwitch(comm1.getAllSwitch());
  mySwitch.setSingleSwitch(comm1.getSingleSwitch());
}

void checkFailSafe(){
  if(failSafeFlag){
    if(!failSafeHandled){
      emergencyStop();
      myIndicator.setConnectionBreak();
      failSafeHandled = 1;
    }
  }
  if(failSafeRecovered){
    myIndicator.resetConnectionBreak();
    failSafeHandled = 0;
    failSafeRecovered = 0;
  }
}


void loop() {
  // put your main code here, to run repeatedly:
  if(globalRefreshReady){
    if(recvPackageComplete){
      comm1.parse(recvBuffer);
      comm1.handleReturnRequest();
      refreshMotors();
      refreshServos();
      refreshSwitches();
      recvPackageComplete = 0;
    }
    checkFailSafe();
    checkLimitSwitch();
    myIndicator.refreshIndicator();
    globalRefreshReady = 0;
  }
}
