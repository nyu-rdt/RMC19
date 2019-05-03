#ifndef __TIMERCONFIG_H
#define __TIMERCONFIG_H

#ifndef GLOBALTICK
#define GLOBALTICK 10
#endif

#include <avr/interrupt.h>
#include <avr/io.h>
#include <Arduino.h>

class SysTick{
public:
  void init();
  void start();
  void stop();
  unsigned int getMillisecond();
  void setTimer2OCR(unsigned int value);
};


#endif
