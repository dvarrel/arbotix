#include <ax12.h>

const uint8_t SERVOCOUNT = 8;
const uint8_t broadcastID=254;
const uint16_t Home[]= {512, 380, 644, 340, 684, 561, 512, 512};
uint16_t AX12speed=100;
uint8_t id;

void setup(){
  pinMode(0,OUTPUT);  
  Serial.begin(1000000);
  delay(10);
  ax12Init(1000000);
  delay(5);
  InfosServos();
  ChangeSpeed(AX12speed);
  MenuOptions();
}

void loop(){
  uint32_t x;
  if (Serial.available() > 0) {
   uint8_t inByte = Serial.read();
   switch (inByte) {
    case '1':    
      InfosServos();
      break;
     case '2':    
      Serial.println("#### -> Center position ####");
      ax12SetRegister2(broadcastID, AX_GOAL_POSITION_L, 512);
      break;     
     case '3':    
       Serial.println("#### -> Home position #####");
       for (id=1;id<= SERVOCOUNT;id++) ax12SetRegister2(id, AX_GOAL_POSITION_L, Home[id-1]);
      break;     
    case '4':    
      MoveAndPrint();
      break;                                                                                                                                                                                      
    case '5':   
      Serial.println("##### Torque OFF ####");
      ax12SetRegister(broadcastID, AX_TORQUE_ENABLE, 0);
      break;                                                                                                                                                                                      
    case '6':    
      Serial.println("#### Torque ON ####");
      ax12SetRegister(broadcastID, AX_TORQUE_ENABLE, 1);
      break;                                                                                                                                                                                      
    case '7':    
      x=Serial.parseInt();
      if (x>0 && x<1024) ChangeSpeed(int(x));
      break;                                                                                                                                                                                      
 }
  if (inByte!=10) MenuOptions();
  }

}

void MenuOptions(){
  Serial.println("###########################"); 
  Serial.println("1) Servo Infos");        
  Serial.println("2) Move to Center");     
  Serial.println("3) Move to Home");     
  Serial.println("4) Move id,position and print millis load speed position");            
  Serial.println("5) Torque OFF Servos");  
  Serial.println("6) Torque ON Sevos");  
  Serial.println("7) Change Speed (1-1023)");  
  Serial.println("###########################"); 
}

void PrintInfos(String s,byte r,byte num) {
   Serial.print(s);
   for (id=1;id <= SERVOCOUNT;id++) {
    Serial.print(ax12GetRegister(id,r,num));
    if (id!=SERVOCOUNT) Serial.print(",");
    else Serial.println();
   }
}

void InfosServos(){
  Serial.println("#### Servo Infos ####");
  PrintInfos("temperature ",AX_PRESENT_TEMPERATURE, 1);
  PrintInfos("position ",AX_PRESENT_POSITION_L, 2);
  PrintInfos("speed ",AX_GOAL_SPEED_L, 2);
  PrintInfos("voltage x10°C ",AX_PRESENT_VOLTAGE, 1);
  PrintInfos("load ", AX_PRESENT_LOAD_L, 2);
  PrintInfos("torque limit ", AX_TORQUE_LIMIT_L, 2);
  PrintInfos("compliance margin CW ", AX_CW_COMPLIANCE_MARGIN, 1);
  PrintInfos("compliance margin CCW ", AX_CCW_COMPLIANCE_MARGIN, 1);
  PrintInfos("compliance slope CW ", AX_CW_COMPLIANCE_SLOPE, 1);
  PrintInfos("compliance slope CCW ", AX_CCW_COMPLIANCE_SLOPE, 1);
}

void ChangeSpeed(uint16_t x){
  AX12speed=x;
  ax12SetRegister2(broadcastID, AX_GOAL_SPEED_L, AX12speed);
  Serial.print("New Speed : ");
  Serial.println(AX12speed);
}

void MoveAndPrint() {
  id=Serial.parseInt();
  uint16_t y=Serial.parseInt();
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
  uint32_t t=millis();
  while (ax12GetRegister(id,AX_MOVING,1)!=0) {
    Serial.print(millis()-t);
    Serial.print("\t");
    int16_t l=ax12GetRegister(id,AX_PRESENT_LOAD_L, 2);
    if (bitRead(l,10)==1) l=1024-l;
    Serial.print(l);
    Serial.print("\t");
    int16_t v=ax12GetRegister(id,AX_PRESENT_SPEED_L, 2);
    if (bitRead(v,10)==1) v=1024-v;
    Serial.print(v);
    Serial.print("\t");
    Serial.println(ax12GetRegister(id,AX_PRESENT_POSITION_L, 2));
  }
}
