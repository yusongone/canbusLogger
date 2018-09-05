#include "read.h"

QueueArray <unsigned char> canbusQueue;
char ecu_req(unsigned char pid);

receiveHandler r;

void CanQueue::request(unsigned char pid){
 // QueneAreay
 Serial.println(canbusQueue.count());
 if(canbusQueue.isEmpty()){
  canbusQueue.enqueue(pid);
  ecu_req(canbusQueue.front());
 }else{
  canbusQueue.enqueue(pid);
 }
}

void CanQueue::receiveData(receiveHandler f){
  r = f;
}


void getDataSuccess(tCAN *msg){
  r(msg);
  canbusQueue.dequeue();
  if(!canbusQueue.isEmpty()){
    // delay(90);
    Serial.print("sss");
    ecu_req(canbusQueue.front());
  }
}
char ecu_req(unsigned char pid){
  tCAN message;
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

  Serial.print("-");
  Serial.print(pid);
  Serial.print("-");
  mcp2515_bit_modify(CANCTRL, (1<<REQOP2)|(1<<REQOP1)|(1<<REQOP0), 0);
  Serial.print("2");
  if (mcp2515_send_message(&message)) { }
  Serial.print("3");
  while(mcp2515_check_message()) {
    Serial.print("&");
    if (mcp2515_get_message(&message)) {
     getDataSuccess(&message);
    }
  }

}

CanQueue CQ;