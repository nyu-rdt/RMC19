#ifndef __PWM_H
#define __PWM_H

#include <avr/interrupt.h>
#include <avr/io.h>
#include <Arduino.h>

class AtlasPWM{
public:
  void init(){
  unsigned char i;
  TCCR3A = 0x00;
  TCCR3B = 0x00;
  TCCR4A = 0x00;
  TCCR4B = 0x00;
  TCCR5A = 0x00;
  TCCR5B = 0x00;
  DDRE = 0x38;
  DDRH = 0x38;
  DDRL = 0x38;
  ICR3 = 40000 - 1;
  ICR4 = 40000 - 1;
  ICR5 = 40000 - 1;
  for(i = 0; i < 9; i++)
    setPwmRaw(i, 3000);
    TCCR3A |= (1 << COM3A1)| (1 << COM3B1) | (1 << COM3C1) | (1 << WGM31);
    TCCR3B |= (1 << WGM32) | (1 << WGM33);
    TCCR4A |= (1 << COM4A1)| (1 << COM4B1) | (1 << COM4C1) | (1 << WGM41);
    TCCR4B |= (1 << WGM42) | (1 << WGM43);
    TCCR5A |= (1 << COM5A1)| (1 << COM5B1) | (1 << COM5C1) | (1 << WGM51);
    TCCR5B |= (1 << WGM52) | (1 << WGM53);
  }

  void start(){
    TCCR3B |= (0 << CS32) | (1 << CS31) | (0 << CS30);
    TCCR4B |= (0 << CS42) | (1 << CS41) | (0 << CS40);
    TCCR5B |= (0 << CS52) | (1 << CS51) | (0 << CS50);
  }

  void stop(){
    TCCR3B &= !((1 << CS32) | (1 << CS31) | (1 << CS30));
    TCCR4B &= !((1 << CS42) | (1 << CS41) | (1 << CS40));
    TCCR5B &= !((1 << CS52) | (1 << CS51) | (1 << CS50));
  }

  void setPwmRaw(unsigned char channel, unsigned int value){
    switch(channel){
      case 0 : OCR3A = value; break;
      case 1 : OCR3B = value; break;
      case 2 : OCR3C = value; break;
      case 3 : OCR4A = value; break;
      case 4 : OCR4B = value; break;
      case 5 : OCR4C = value; break;
      case 6 : OCR5A = value; break;
      case 7 : OCR5B = value; break;
      case 8 : OCR5C = value; break;
      default : break;
    }
  }

  void setMotor(unsigned char motor, int level){
    unsigned int pulseWidth = (unsigned long)level * 2000 / 255 + 2000;
    if(motor >= 0 && motor <= 3){
      setPwmRaw(motor, pulseWidth);
      
    }
  }

  void setServo(unsigned char servo, unsigned char level){
    unsigned int pulseWidth = (unsigned long)level * 2000 / 255 + 2000;
    if(servo >= 0 && servo <= 4){
      setPwmRaw(servo + 4, pulseWidth);
    }
  }
};

#endif
