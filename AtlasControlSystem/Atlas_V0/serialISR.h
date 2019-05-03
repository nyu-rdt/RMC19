#ifndef __SERIALISR_H
#define __SERIALISR_H
#include <avr/interrupt.h>
#include <avr/io.h>
#include <Arduino.h>


void resetFailSafe();
void returnHeartBeat();
void prepareReceivingPackage();
void completeReceivingPackage();
void usartInit();
void sendChar(unsigned char c);
void sendBuffer(unsigned char* p, unsigned int length);
void sendStr(const char* p);
void sendStrWithNewLine(const char* p);
void sendNewLine();
void sendNum(unsigned int num);

#endif
