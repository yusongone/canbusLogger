#include "read.h"

QueueArray <char> canbusQueue;
char ecu_req();

receiveHandler r;

void CanQueue::request(unsigned char pid){
 // QueneAreay
 if(canbusQueue.isEmpty()){
  canbusQueue.enqueue(pid);
  ecu_req();
 }else{
  canbusQueue.enqueue(pid);
 }
}

void CanQueue::receiveData(receiveHandler f){
  r = f;
}


char ecu_req(){
  char pid = canbusQueue.front();
  tCAN message;
  float engine_data;
  int timeout = 0;
  char message_ok = 0;
  // Prepair message
  message.id = PID_REQUEST;
  message.header.rtr = 0;
  message.header.length = 8;
  message.data[0] = 0x02;
  message.data[1] = 0x01;
  message.data[2] = pid;
  message.data[3] = 0x00;
  message.data[4] = 0x00;
  message.data[5] = 0x00;
  message.data[6] = 0x00;
  message.data[7] = 0x00;           
  

  mcp2515_bit_modify(CANCTRL, (1<<REQOP2)|(1<<REQOP1)|(1<<REQOP0), 0);
  
  if (mcp2515_send_message(&message)) { }
  if (mcp2515_check_message()) {
    if (mcp2515_get_message(&message)) {
      r(&message);
      canbusQueue.dequeue();
      if(!canbusQueue.isEmpty()){
        ecu_req();
      }
    }
  }
}

CanQueue CQ;