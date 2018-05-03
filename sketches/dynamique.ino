/*
to install drivers, libraries and hardware to arduino ide
go to https://github.com/dvarrel/dynamixel

TP dynamique
accélération-décélération sur 180°
Torque : 1.5N.m (at 12.0V, 1.5A)
No load speed : 59rpm (at 12V)
Running Degree 0° ~ 300°
*/

//#include <Uno_ax12.h>   //with arduino Uno
#include <ax12.h>

const uint8_t SERVOCOUNT = 8;
const uint8_t broadcastID=254;
const uint16_t AX12speed=100;
const uint8_t id=1;

void setup(){
  pinMode(0,OUTPUT);  
  Serial.begin(115200);
  delay(10);
  ax12Init(1000000);
  delay(5);
  ax12SetRegister2(broadcastID, AX_GOAL_SPEED_L, AX12speed);
  ax12SetRegister(broadcastID,AX_RETURN_DELAY_TIME,5);
  MenuOptions();
}

void loop(){
  uint32_t x;
  if (Serial.available() > 0) {
   uint8_t inByte = Serial.read();
   switch (inByte) {
     case '2':  
      ax12SetRegister2(broadcastID, AX_GOAL_SPEED_L, AX12speed);  
      ax12SetRegister2(broadcastID, AX_GOAL_POSITION_L, 512);
      break;     
    case '3':    
      MoveAndPrint(100);
      break;                                                                                                                                                                                      
    case '4':    
      MoveAndPrint(280);
      break;                                                                                                                                                                                      
    case 'o':    
      Serial.println("#### Open Hand ####");
      ax12SetRegister2(8, AX_GOAL_POSITION_L,512);
      break;                                                                                                                                                                                      
    case 'c':    
      Serial.println("#### Close Hand ####");
      ax12SetRegister2(8, AX_GOAL_POSITION_L,200);
      break;                                                                                                                                                                                      
 }
  if (inByte!=10) MenuOptions();
  }

}

void MenuOptions(){
  Serial.println("\n##TP dynamique - Shoulder(Epaule)##"); 
  Serial.println("2 -> Move to Center");     
  Serial.println("3 -> Move shoulder to 100° and print");     
  Serial.println("4 -> Move shoulder to 280° and print");            
  Serial.println("o -> Open Hand");  
  Serial.println("c -> Close Hand");  
  Serial.println("###########################"); 
}

void MoveAndPrint(uint16_t degre) {
  Serial.print("position ");
  int32_t current=(int32_t)ax12GetRegister(id,AX_PRESENT_POSITION_L, 2)*300/1023;
  Serial.print(current);
  Serial.print("° -> ");
  Serial.print(degre);Serial.println("°");
  
  uint32_t goal=(uint32_t)degre*1023/300; // 0-1023
  const float duree=2.0; //s 
  uint32_t alpha=4*abs(current-degre)/(duree*duree);
  //nmax=59rpm=354°/s
  //nmax=114rpm=684°/s   114*360/60=114*6
  #define nmaxrpm 105
  uint32_t wmax=alpha*duree/2; //0-1023
  uint32_t n=5;


  Serial.println("duree\talpha\twmax");
  Serial.print(duree);Serial.print("\t");
  Serial.print(alpha);Serial.print("\t");
  Serial.println(wmax);
    
  //accélération à nmax puis décélération;
  ax12SetRegister2(id, AX_GOAL_SPEED_L, n);
  ax12SetRegister2(id, AX_GOAL_POSITION_L, goal);
  
  Serial.println("time\tangle\tw_simul\tw_mesure");
  Serial.println("ms\t°\t°/s\t°/s");
  
  uint32_t lastpos,lasttx;
  uint32_t posdegre=(uint32_t)ax12GetRegister(id,AX_PRESENT_POSITION_L, 2)*300/1023;
  uint32_t t0=millis();
  delay(1);
  uint32_t tx=millis()-t0;
  delay(1);
  while (tx<duree*1000) {
    lasttx=tx;
    tx=millis()-t0;
    if (tx<duree*500) n=max(2,alpha*tx*1023/nmaxrpm/6000);
    else n=max(2,alpha*(duree*1000-tx)*1023/nmaxrpm/6000);
    /*if (tx<duree*500) n=max(2,nmax*tx/1000);
    else n=max(2,nmax-min(nmax,nmax*(tx-duree*500)/1000));*/
    ax12SetRegister2(id, AX_GOAL_SPEED_L, n);
        
    Serial.print(tx);
    Serial.print("\t");

    lastpos=posdegre;
    posdegre=(uint32_t)ax12GetRegister(id,AX_PRESENT_POSITION_L, 2)*300/1023;
    if (posdegre>310) posdegre=lastpos;
    Serial.print(posdegre);
    Serial.print("\t");

    uint32_t w_calcul=n*nmaxrpm*6/1023;
    Serial.print(w_calcul);
    Serial.print("\t");
    
    uint32_t w_mesure=abs((int)posdegre-(int)lastpos)*1000/(tx-lasttx);
    Serial.println(w_mesure);
    
    delay(20);
    if (ax12GetRegister(id,AX_MOVING,1)==0) return 0;
  }
}
