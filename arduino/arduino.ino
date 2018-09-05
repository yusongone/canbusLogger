#include "read.h"
#include "simpleTimer.h"

#include <mcp2515.h>
#include <mcp2515_defs.h>
#include "config.h"

SimpleTimer timer;

void handler_100ms(){
  CQ.request(ENGINE_LOAD); // engine load
  CQ.request(ENGINE_RPM); // RPM
  delay(100);
  CQ.request(CAR_SPEED); // speed
  delay(100);
  CQ.request(THROTTLE); // throttle
  delay(100);
  /*
  /*
  */
}

void handler_5_000ms(){
  CQ.request(INTAKE_AIR_TEMP); 
}

void handler_15_000ms(){
  CQ.request(ENGINE_OIL_TEMP); 
}

void handler_60_000ms(){
  CQ.request(FUEL_TANK_LEVEL_INPUT);
}

void handler_300_000ms(){
  CQ.request(DISTANCE_TRAVELED_SINCE_CODE_CLEARED);
}

void receiveDataHandler(tCAN *message){
  switch(message -> data[2]){
    case ENGINE_LOAD: // Calculated engine load
    Serial.println("x1");
    break;
    case ABSOLUTE_LOAD:// Absolute load value
    Serial.println("x2");
    break;
    case ENGINE_RPM: // RPM
    Serial.println("x3");
    break;
    case CAR_SPEED:// speed
    Serial.println("x4");
    break;
    case THROTTLE:// throttle
    Serial.println("x5");
    break;
    case INTAKE_AIR_TEMP: //Intake air temperature;
    Serial.println("x6");
    break;
    case ENGINE_OIL_TEMP://Engine oil temperature
    Serial.println("x7");
    break;
    case FUEL_TANK_LEVEL_INPUT://Fuel Tank Level Input
    Serial.println("x8");
    break;
    case DISTANCE_TRAVELED_SINCE_CODE_CLEARED://Distance traveled since codes cleared
    Serial.println("x9");
    break;
  }
  Serial.print("*");
  Serial.print(message->data[2]);
  Serial.println("*");
}


void setup(){
  Serial.begin(9600); // For debug use
  Serial.println("CAN Read - Testing receival of CAN Bus message");  
  delay(1000);
  
  if(Canbus.init(CANSPEED_500))  //Initialise MCP2515 CAN controller at the specified speed
    Serial.println("CAN Init ok");
  else
    Serial.println("Can't init CAN");
  delay(1000);
  /*
  timer.setInterval(300000,handler_300_000ms);
  timer.setInterval(60000,handler_60_000ms);
  timer.setInterval(151000,handler_15_000ms);
  timer.setInterval(5000,handler_5_000ms);
  */
  timer.setInterval(500,handler_100ms);
  CQ.receiveData(receiveDataHandler);
}

void loop(){
  timer.run();
}