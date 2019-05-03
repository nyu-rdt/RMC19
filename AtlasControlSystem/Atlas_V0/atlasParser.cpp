#include "atlasParser.h"

unsigned char AtlasComm::getMotor(unsigned char channel){
  return dataArr[1 + channel];
}

void AtlasComm::setMotor(unsigned char channel, unsigned char value){
  dataArr[1 + channel] = value;
}

unsigned char AtlasComm::getServo(unsigned char channel){
  return dataArr[7 + channel];
}

void AtlasComm::setServo(unsigned char channel, unsigned char value){
  dataArr[7 + channel] = value;
}

unsigned int AtlasComm::getVoltage(unsigned char channel){
  return voltages[channel];
}

void AtlasComm::setVoltage(unsigned char channel, unsigned int value){
  voltages[channel] = value;
}

unsigned int AtlasComm::getCurrent(unsigned char channel){
  return currents[channel];
}

void AtlasComm::setCurrent(unsigned char channel, unsigned int value){
  currents[channel] = value;
}

unsigned char AtlasComm::getSwitch(unsigned char channel){
  int switchStatus = dataArr[0x0F] & (0x01 << channel);
  if(switchStatus)
    return 1;
  else
    return 0;
}

void AtlasComm::setSwitch(unsigned char channel, unsigned char value){
  dataArr[0x0F] |= value << channel;
}

unsigned char AtlasComm::getAllSwitch(){
  return dataArr[0x0F];
}

void AtlasComm::setAllSwitch(unsigned char value){
  dataArr[0x0F] = value;
}

unsigned char AtlasComm::getSingleSwitch(){
  return dataArr[0x13];
}

void AtlasComm::setSingleSwitch(unsigned char value){
  dataArr[0x13] = value;
}

void AtlasComm::parse(unsigned char* p){
  unsigned char length = 0;
  unsigned int i;
  unsigned char parsingCommand = 0;
  unsigned char commandIndex = 0;
  unsigned char readRequest = 0;
  returnArr[0] = 0;
  length = *p;
  p++;
  for(i = 0; i < length; i++){
    if(!parsingCommand){
      if(*(p + i) & 0x80){
        returnArr[0]++;
        returnArr[returnArr[0]] = *(p + i) & 0x7F;
        readRequest = 1;
        parsingCommand = 1;
        commandIndex = *(p + i) & 0x7F;
      }
      else{
        readRequest = 0;
        parsingCommand = 1;
        commandIndex = *(p + i);
      }   
    }
    else{
      parsingCommand = 0;
      if(!readRequest)
        dataArr[commandIndex] = *(p + i);
      if(readRequest && (commandIndex == 0x10 || commandIndex == 0x11)){
        returnArr[0]++;
        returnArr[returnArr[0]] = *(p + i);
      }
    }
  }
}

void AtlasComm::handleReturnRequest(){
  unsigned char i, length;
  unsigned char returnedData[100];
  unsigned char appendingContent = 0;
  unsigned char commandIndex = 0;
  unsigned char selectedChannel = 0;
  unsigned char multiChannelRead = 0;
  if(returnArr[0] == 0){
  returnHeartBeat();
    return;
  }
  length = returnArr[0];
  returnedData[0] = 'N';
  returnedData[1] = 'R';
  returnedData[2] = 0x02;
  returnedData[3] = 0;
  for(i = 0; i < length; i++){
    if(!appendingContent){
      if(multiChannelRead){
        selectedChannel = returnArr[1 + i];
        returnedData[3]++;
        returnedData[3 + returnedData[3]] = selectedChannel;
        multiChannelRead = 0;
        appendingContent = 1;
      }
      else{
        commandIndex = returnArr[1 + i];
        returnedData[3]++;
        returnedData[3 + returnedData[3]] = commandIndex;
        if(commandIndex == 0x10 || commandIndex == 0x11)
          multiChannelRead = 1;
        else
          appendingContent = 1;
      }
    }
    if(appendingContent){
      if(commandIndex == 0x10){
        returnedData[3]++;
        returnedData[3 + returnedData[3]] = voltages[selectedChannel] >> 8;
        returnedData[3]++;
        returnedData[3 + returnedData[3]] = voltages[selectedChannel];
      }
      else if(commandIndex == 0x11){
        returnedData[3]++;
        returnedData[3 + returnedData[3]] = currents[selectedChannel] >> 8;
        returnedData[3]++;
        returnedData[3 + returnedData[3]] = currents[selectedChannel];
      }
      else{
        returnedData[3]++;
        returnedData[3 + returnedData[3]] = dataArr[commandIndex];
      }
      appendingContent = 0;
    }
  }
  sendBuffer(returnedData, returnedData[3]+4);
}
