#include <ax12.h>

const int SERVOCOUNT = 8;
const byte broadcastID=254;
const uint16_t Home[]= {512, 380, 644, 340, 684, 561, 512, 512};

boolean VoltageOK=false;
uint16_t AX12speed=100;

void setup(){
  pinMode(0,OUTPUT);  
  Serial.begin(1000000);
  delay(10);
  Serial.println("ax12Init...");
  ax12Init(1000000);
  CheckVoltage();
  ChangeSpeed(AX12speed);
  MenuOptions();
}

void loop(){
  long x;
  if (Serial.available() > 0) {
   byte inByte = Serial.read();
   switch (inByte) {
    case '1':    
      InfosServos();
      break;
     case '2':    
      MoveCenter();
      break;     
     case '3':    
      MoveHome();
      break;     
    case '4':    
      MoveAndPrint();
      break;                                                                                                                                                                                      
    case '6':    
      CheckVoltage();
      break;                                                                                                                                                                                      
    case '7':    
      x=Serial.parseInt();
      if (x>=0 && x<1024) ChangeSpeed(int(x));
      break;                                                                                                                                                                                      
 }
  if (inByte!=10)  MenuOptions();
  }

}

void MenuOptions(){
  Serial.println("###########################"); 
    Serial.println("1) Servo Scanning Test");        
    Serial.println("2) Move Servos to Center");     
    Serial.println("3) Move Servos to Home");     
    Serial.println("4) Move id,position print millis load speed position");            
    Serial.println("6) Check System Voltage");  
    Serial.println("7) Change Speed (0-1023)");  
    Serial.println("###########################"); 
}

void InfosServos(){
  Serial.println("###########################");
  Serial.println("Starting Servo Scanning Test.");
  Serial.println("###########################");
  Serial.print("position{");    
  for (int id=1;id <= SERVOCOUNT;id++){
  Serial.print(  ax12GetRegister(id, AX_PRESENT_POSITION_L, 2));
  if (id!=SERVOCOUNT) Serial.print(",");   
  }
  Serial.println("}");
  Serial.print("speed{");    
  for (int id=1;id<= SERVOCOUNT;id++){
  Serial.print( ax12GetRegister(id, AX_PRESENT_SPEED_L, 2));
  if (id!=SERVOCOUNT) Serial.print(","); 
  }
  Serial.println("}");
  Serial.print("voltage{");    
  for (int id=1;id<= SERVOCOUNT;id++){
  Serial.print( ax12GetRegister(id, AX_PRESENT_VOLTAGE, 1));
  if (id!=SERVOCOUNT) Serial.print(","); 
  }
  Serial.println("}");
  Serial.print("load{");    
  for (int id=1;id<= SERVOCOUNT;id++){
  Serial.print( ax12GetRegister(id, AX_PRESENT_LOAD_L, 2));
  if (id!=SERVOCOUNT) Serial.print(","); 
  }
  Serial.println("}");  
  Serial.print("torque{");    
  for (int id=1;id<= SERVOCOUNT;id++){
  Serial.print( ax12GetRegister(id, AX_TORQUE_LIMIT_L, 2));
  if (id!=SERVOCOUNT) Serial.print(","); 
  }
  Serial.println("}");  
  Serial.print("compliance margin{");    
  for (int id=1;id<= SERVOCOUNT;id++){
  Serial.print( ax12GetRegister(id, AX_CW_COMPLIANCE_MARGIN, 2));
  if (id!=SERVOCOUNT) Serial.print(","); 
  }
  Serial.println("}");  
}

void CheckVoltage(){  
  float voltage = (ax12GetRegister (1, AX_PRESENT_VOLTAGE, 1)) / 10.0;
  Serial.println("###########################");   
  Serial.print ("System Voltage: ");
  Serial.print (voltage);
  Serial.println (" volts.");
  if (voltage < 10.0){
    Serial.println("Voltage levels below 10v, please check Power.");
    VoltageOK=false;
  }  
  else VoltageOK=true;
  Serial.println("###########################"); 
}

void MoveCenter(){
  Serial.println("###########################");
  Serial.println("Moving servos to Center position");
  Serial.println("###########################");
  ax12SetRegister2(broadcastID, AX_GOAL_POSITION_L, 512);
}

void MoveHome(){
  Serial.println("###########################");
  Serial.println("Moving servos to Home position");
  Serial.println("###########################");
  for (byte id=1;id<= SERVOCOUNT;id++){
    ax12SetRegister2(id, AX_GOAL_POSITION_L, Home[id-1]);
  }
}

void ChangeSpeed(int x){
  AX12speed=x;
  ax12SetRegister2(broadcastID, AX_GOAL_SPEED_L, AX12speed);
  Serial.print("New Speed : ");
  Serial.println(AX12speed);
}

void MoveAndPrint() {
  byte id=Serial.parseInt();
  int y=Serial.parseInt();
  if (id<0 || id>SERVOCOUNT || y<0 || y>=1024) {
    Serial.println("error !");
    return 0;
  }
  Serial.print("position ");
  Serial.print(ax12GetRegister(id,AX_PRESENT_POSITION_L, 2));
  Serial.print(" -> ");
  Serial.println(y);
  Serial.print("speed:");
  Serial.println(ax12GetRegister(id,AX_GOAL_SPEED_L, 2));
  ax12SetRegister2(id, AX_GOAL_POSITION_L, y);
  Serial.println("time\tload\tspeed\tposition");
  unsigned long t=millis();
  while (ax12GetRegister(id,AX_MOVING,1)!=0) {
    Serial.print(millis()-t);
    Serial.print("\t");
    int l=ax12GetRegister(id,AX_PRESENT_LOAD_L, 2);
    if (bitRead(l,10)==1) l=1024-l;
    Serial.print(l);
    Serial.print("\t");
    int v=ax12GetRegister(id,AX_PRESENT_SPEED_L, 2);
    if (bitRead(v,10)==1) v=1024-v;
    Serial.print(v);
    Serial.print("\t");
    Serial.println(ax12GetRegister(id,AX_PRESENT_POSITION_L, 2));
  }
}

