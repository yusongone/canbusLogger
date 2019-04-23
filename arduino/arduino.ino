#include <Canbus.h>
#include <defaults.h>

#include <global.h>
#include <mcp2515.h>
#include <mcp2515_defs.h>
#include "config.h"


typedef void (*parseProtocol)(tCAN *message,String &t);

struct Msg{
  int id;
  tCAN values;
  long updateIntervalTime;
  unsigned long requestTime;
  unsigned long lastUpdateTime;
  parseProtocol parser;
} fule, speed,engine_rpm,throttle,dis;


const int elementCount=5;
bool pin13=false;

Msg element[elementCount];

void fule_parse(tCAN *msg,String &str){
  float t =  float(msg->data[3] * 100)/255;
  str+="{\"fule\":"+String(t)+"}";
}

void speed_parse(tCAN *msg,String &str){
  int t =  msg->data[3] ;
  str+="{\"speed\":"+String(t)+"}";
}

void engine_rpm_parse(tCAN *msg,String &str){
  int t =  (256*msg->data[3] + msg->data[4])/4 ;
  str+="{\"rpm\":"+String(t)+"}";
}

void throttle_rpm_parse(tCAN *msg, String &str){
  float t = float(msg->data[3]*100)/255;
  str+="{\"thr\":"+String(t)+"}";
}

void dis_parse(tCAN *msg, String &str){
  float t = float(msg->data[3]*256) + msg->data[4];
  str+="{\"dis\":"+String(t)+"}";
}


void setup(){
  // dataFile = SD.open("datalog.txt", FILE_WRITE);

  element[0]=fule;
  element[0].id=FUEL_TANK_LEVEL_INPUT;
  element[0].updateIntervalTime=1000*30;
  element[0].parser=fule_parse;

  element[1]=dis;
  element[1].id=DISTANCE_TRAVELED_SINCE_CODE_CLEARED;
  element[1].updateIntervalTime=1000*30;
  element[1].parser=dis_parse;

  element[2]=speed;
  element[2].id=CAR_SPEED;
  element[2].updateIntervalTime=500;
  element[2].parser=speed_parse;

  element[3]=engine_rpm;
  element[3].id=ENGINE_RPM;
  element[3].updateIntervalTime=500;
  element[3].parser=engine_rpm_parse;

  element[4]=throttle;
  element[4].id=THROTTLE;
  element[4].updateIntervalTime=500;
  element[4].parser=throttle_rpm_parse;



//  Serial.begin(115200); // For debug use
  Serial.begin(9600); // For debug use
  Serial.println("CAN Read - Testing receival of CAN Bus message");  
  delay(1000);
  pinMode(8, OUTPUT);
  
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
          pin13=!pin13;
          String t ="";
          if(252 == mes.data[2]){
            /*
            Serial.print("car");
            Serial.println(mes.data[0]);
            */
            t ="{\"pow\":"+String(mes.data[0])+"}";
          }else{
            for(int i =0; i<elementCount;i++){
                // element[i].values= &message;
                if(element[i].id == mes.data[2]){
                  element[i].parser(&mes, t);
                  break;
                }
            }
          }
          Serial.println(t);
      }
    }
  }
}

unsigned long pingTime;
void SerialPing(){
  unsigned long temp = millis();
  if( (temp - pingTime) > 1000 ){ 
    pingTime=temp;
    Serial.println("{\"ping\":1000}");
  }
}

void loop(){
  getter();
  sender();
  SerialPing();
  digitalWrite(8, pin13);
}
