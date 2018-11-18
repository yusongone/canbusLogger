#include <Canbus.h>
#include <defaults.h>

#include <global.h>
#include <mcp2515.h>
#include <mcp2515_defs.h>
#include "config.h"


typedef void (*parseProtocol)(tCAN *message);

struct Msg{
  int id;
  tCAN values;
  long updateIntervalTime;
  unsigned long requestTime;
  unsigned long lastUpdateTime;
  parseProtocol parser;
} fule, speed,engine_rpm,throttle;


const int elementCount=4;

Msg element[elementCount];

void fule_parse(tCAN *msg){
  float t =  float(msg->data[3] * 100)/255;
  Serial.print("{\"fule\":");
  Serial.print(t);
  Serial.print("}");
}

void speed_parse(tCAN *msg){
  int t =  msg->data[3] ;
  Serial.print("{\"speed\":");
  Serial.print(t);
  Serial.print("}");
}

void engine_rpm_parse(tCAN *msg){
  int t =  (256*msg->data[3] + msg->data[4])/4 ;
  Serial.print("{\"rpm\":");
  Serial.print(t);
  Serial.print("}");
}

void throttle_rpm_parse(tCAN *msg){
  float t = float(msg->data[3]*100)/255;
  Serial.print("{\"throttle\":");
  Serial.print(t);
  Serial.print("}");
}


void setup(){
  // dataFile = SD.open("datalog.txt", FILE_WRITE);

  element[0]=fule;
  element[0].id=FUEL_TANK_LEVEL_INPUT;
  element[0].updateIntervalTime=1000*30;
  element[0].parser=fule_parse;

  element[1]=speed;
  element[1].id=CAR_SPEED;
  element[1].updateIntervalTime=500;
  element[1].parser=speed_parse;

  element[2]=engine_rpm;
  element[2].id=ENGINE_RPM;
  element[2].updateIntervalTime=500;
  element[2].parser=engine_rpm_parse;

  element[3]=throttle;
  element[3].id=THROTTLE;
  element[3].updateIntervalTime=500;
  element[3].parser=throttle_rpm_parse;



  Serial.begin(9600); // For debug use
  Serial.println("CAN Read - Testing receival of CAN Bus message");  
  delay(1000);
  
  if(Canbus.init(CANSPEED_500))  //Initialise MCP2515 CAN controller at the specified speed
    Serial.println("CAN Init ok");
  else
    Serial.println("Can't init CAN");
  delay(1000);
}


char my_ecu_req(unsigned char pid){
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

  mcp2515_bit_modify(CANCTRL, (1<<REQOP2)|(1<<REQOP1)|(1<<REQOP0), 0);
  uint8_t sendResult = mcp2515_send_message(&message);
}

unsigned long time;

void sender(){
  unsigned long temp = millis();
  if( (temp - time) > 125){ // sender interval must > 100ms
    for(int i =0; i<elementCount;i++){
      if(temp - element[i].lastUpdateTime>element[i].updateIntervalTime){
        element[i].lastUpdateTime=temp;
        my_ecu_req(element[i].id);
        break;
      }else if(temp<element[i].lastUpdateTime){
        element[i].lastUpdateTime=temp;
      }
    }
    time = temp;
  }else if(temp<time){
    time = temp;
  }
}


unsigned long getterTime;
void getter(){
  tCAN mes;
  unsigned long temp = millis();
  if( (temp - getterTime) > 125){ // sender interval must > 100ms
    getterTime=temp;
    if(mcp2515_check_message()) {
      if (mcp2515_get_message(&mes)) {
          if(252 == mes.data[2]){
            /*
            Serial.print("car");
            Serial.println(mes.data[0]);
            */
          }else{
            for(int i =0; i<elementCount;i++){
                // element[i].values= &message;
                if(element[i].id == mes.data[2]){
                  element[i].parser(&mes);
                  break;
                }
            }
          }
      }
    }
  }
}

void loop(){
  /*
  getter();
  sender();
  */
  Serial.print("{\"throttle\":");
  Serial.print(12.34);
  Serial.print("}");
 delay(125);
}
