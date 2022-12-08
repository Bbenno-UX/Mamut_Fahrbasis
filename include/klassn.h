
#include "ServoTimer2.h"
class Schrittmot{
  public:
    Schrittmot(int spr, int speed){
        usecs=60000000/(spr*speed);

        //myStepper=new Stepper(spr, STP1, STP2, STP3, STP4);
        schritte=0;
        stepkram=0;
    }
    void sprak(int st){
          switch (st) {
      case 0:  // 1010
        digitalWrite(STP1, HIGH);
        digitalWrite(STP2, LOW);
        digitalWrite(STP3, HIGH);
        digitalWrite(STP4, LOW);
      break;
      case 1:  // 0110
        digitalWrite(STP1, LOW);
        digitalWrite(STP2, HIGH);
        digitalWrite(STP3, HIGH);
        digitalWrite(STP4, LOW);
      break;
      case 2:  //0101
        digitalWrite(STP1, LOW);
        digitalWrite(STP2, HIGH);
        digitalWrite(STP3, LOW);
        digitalWrite(STP4, HIGH);
      break;
      case 3:  //1001
        digitalWrite(STP1, HIGH);
        digitalWrite(STP2, LOW);
        digitalWrite(STP3, LOW);
        digitalWrite(STP4, HIGH);
      break;
    }
    }
    bool schritter(){//funktion für schrittmotoren adafruit benutzt dummerweise "delay" bei der step funktion, darum nur 1 schritt auf einmal
  if(abs(schritte)>0){
    static long k=0;
    //Serial.println(schritte);
  if(micros()-stepkram>usecs){
    stepkram=micros();
    if(schritte>0){
    if(k%4==0){
    schritte--;
    }
    sprak(k%4);
    k++;
    }
       if(schritte<0){
    if(k%4==0){
    schritte++;
    }
    
    sprak(k%4);
    k++;
  }
  }
  return true; 
  }
  return false;
}

    int schritte;
    long usecs;
    static long aktschritt;
    unsigned long stepkram;
    //Stepper* myStepper;
};

class Serv{
    public:
    Serv(int PIN, int z_bas){
      pinMode(PIN,OUTPUT);
        t=0;
        del=0;
        aktiv=false;
        z_soll=z_bas;
        pin=PIN;
        Servo2.attach(PIN);
        
    }
//     void turner(){ //momentan legacy, soll raus
// if(aktiv && micros()-t>20000-ztemp){
//   t=micros();
//   digitalWrite(pin,HIGH);
//   aktiv=false;
  
// }
// else if (!aktiv && micros()-t>ztemp){
//   t=micros();
//   digitalWrite(pin,LOW);
//   aktiv=true;
// }
// }

bool servo_akt(){ //langsame anpassung der Servoposition
  if(abs(ztemp-z_soll)>2){
    //Serial.println("was");
  ztemp=ztemp+(z_soll-ztemp)*pow(sin(del*2*CONST_PI/150),2);
  Servo2.write(ztemp);
  del++;
  //Serial.print(pin);Serial.print(":\t");Serial.print(ztemp);Serial.print("\t");Serial.println(z);
  return true;
}
del=0;
return false;
}
    bool aktiv;
    int z_soll;
    int ztemp;
    int del;
    unsigned long t;
    int pin;
    ServoTimer2 Servo2;
};

// Serv Servo1(6,START1);
// //Serv Servo2(7,START2);
// Schrittmot Step1(SPR,SPEED);