#include "serialISR.h"
#define BAUD_9600 103
#define HEARTBEAT 0 
#define VALIDDATA 1


unsigned char data;
unsigned char heartBeat[] = {0x4E, 0x52, 0x00};

unsigned char recvPackageBegin = 0;
unsigned char recvPackageComplete = 0;
unsigned char recvHeaderCounter = 0;
unsigned char recvBufferIndex = 0;
unsigned char recvBuffer[256];

extern unsigned int failSafeCounter;
extern unsigned char failSafeFlag;
extern unsigned char failSafeRecovered;

void resetFailSafe(){
  failSafeCounter = 0;
  failSafeFlag = 0;
  failSafeRecovered = 1;
}

void returnHeartBeat(){
  sendBuffer(heartBeat,3);
}

void prepareReceivingPackage(){
  recvHeaderCounter = 0;
  recvBufferIndex = 0;
  recvPackageBegin = 1;
}

void completeReceivingPackage(){
  recvPackageBegin = 0;
  recvPackageComplete = 1;

}

void usartInit(){
  UBRR1H = (unsigned char) (BAUD_9600 >> 8);
  UBRR1L = (unsigned char) BAUD_9600;
  UCSR1C |= (1 << UCSZ10) | (1 << USBS1);
  UCSR1B |= (1 << RXEN1) | (1 << TXEN1) | (1 << RXCIE1);
}

void sendChar(unsigned char c){
  while(!(UCSR1A & 0x20));
  UDR1 = c;
}

void sendBuffer(unsigned char* p, unsigned int length){
  unsigned count = 0;
  for(count = 0; count < length; count++){
    sendChar(*p);
    p++;
  }
}

void sendStr(const char* p){
  while(*p){
    sendChar(*p);
    p++;
  }
}

void sendNum(unsigned int num){
  unsigned int t=10000;
  unsigned char data=0;
  data=num/t;
  if(num>0)
  {
    while(data==0) 
    {
      t=t/10;
      data=num/t;
    }
    do
    {
      sendChar(data+'0');
      num=num%t;
      t=t/10;
      data=num/t;
    }
    while(t!=0);
  }
  else sendChar('0');
}

void sendStrWithNewLine(const char* p){
  while(*p){
    sendChar(*p);
    p++;
  }
  sendChar('\r');
  sendChar('\n');
}

void sendNewLine(){
  sendChar('\r');
  sendChar('\n');
}

ISR(USART1_RX_vect){
  data = UDR1;
  if(!recvPackageComplete){
    if(!recvPackageBegin){
        if(recvHeaderCounter == 0 && data == 'N')
          recvHeaderCounter++;
        else if(recvHeaderCounter == 1 && data == 'R')
          recvHeaderCounter++;
        else if(recvHeaderCounter == 2){
          resetFailSafe();
          if(data == 0x00)
            returnHeartBeat();
          else if(data == 0x01)
            prepareReceivingPackage();
        }
        else
          recvHeaderCounter = 0;
    }
    else{
      recvBuffer[recvBufferIndex] = data;
      if(recvBufferIndex > 0 && recvBufferIndex == recvBuffer[0])
        completeReceivingPackage();
      recvBufferIndex++;
    }
  }
}

