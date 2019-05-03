#ifndef __ATLASPARSER_H
#define __ATLASPARSER_H
#include "serialISR.h"

class AtlasComm{
public:
  void parse(unsigned char* p);
  void handleReturnRequest();
  unsigned char getMotor(unsigned char channel);
  void setMotor(unsigned char channel, unsigned char value);
  unsigned char getServo(unsigned char channel);
  void setServo(unsigned char channel, unsigned char value);
  unsigned int getVoltage(unsigned char channel);
  void setVoltage(unsigned char channel, unsigned int value);
  unsigned int getCurrent(unsigned char channel);
  void setCurrent(unsigned char channel, unsigned int value);
  unsigned char getSwitch(unsigned char channel);
  void setSwitch(unsigned char channel, unsigned char value);
  unsigned char getAllSwitch();
  void setAllSwitch(unsigned char value);
  unsigned char getSingleSwitch();
  void setSingleSwitch(unsigned char value);
private:
  unsigned char dataArr[40];
  unsigned char returnArr[40];
  unsigned int voltages[8];
  unsigned int currents[8];
};

#endif

