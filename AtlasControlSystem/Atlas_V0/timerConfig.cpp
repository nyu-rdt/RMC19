#include "timerConfig.h"

unsigned int globalRefreshTime = 0;
unsigned char globalRefreshReady = 0;
unsigned int failSafeCounter = 0;
unsigned int failSafeThreshold = 300;
unsigned char failSafeFlag = 0;
unsigned char failSafeHandled = 0;
unsigned char failSafeRecovered = 0;
unsigned long milliseconds = 0;

void SysTick::init(){
  TCCR2A = 0x00;
  TCCR2B = 0x00;
  TCCR2A |= (1 << WGM21) | (0 << WGM20);
  TCCR2B |= (0 << WGM22);
  TIMSK2 |= (1 << OCIE2A);
  OCR2A = 250 - 1;
}

void SysTick::start(){
  TCCR2B |= (1 << CS22) | (0 << CS21) | (0 << CS20);
}

void SysTick::stop(){
  TCCR2B &= !((1 << CS12) | (1 << CS11) | (1 << CS10));
}

void SysTick::setTimer2OCR(unsigned int value){
  OCR2A = value;
}

unsigned int SysTick::getMillisecond(){
  return milliseconds;
}

ISR(TIMER2_COMPA_vect){
  milliseconds++;
  if(++globalRefreshTime > GLOBALTICK){
    globalRefreshTime = 0;
    globalRefreshReady = 1;
  }
  if(++failSafeCounter > failSafeThreshold){
    failSafeCounter = failSafeThreshold;
    failSafeFlag = 1;
  }
}


