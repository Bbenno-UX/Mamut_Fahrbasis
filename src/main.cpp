#include <Arduino.h>
#include <Stepper.h>
#include <Wire.h> // Wire Bibliothek einbinden
#include <LiquidCrystal_I2C.h> // Vorher hinzugefügte LiquidCrystal_I2C Bibliothek einbinden
#include "stepperControlCombined9Pizzakurzcopsaf.h"
#include "0_Structs.h"
#include "Timer.h"
#include "MotorDriver.h"
#include "PWM_Functions.h"
#include "DriveSystem.h"
#include "Y_DistSensors.h"
#include "funcs_fuer_Statue.h"
#include "X_Programs.h"
#include "klassn.h"
#include "Ser_proz.h"
#include "ServoTimer2.h" //github-library für timer2-Servo, weil wir den ersten schpon nutzen
//Serv Servo2(7,START2);

void setup() {
  
  Serial.begin(9600);
  LiquidCrystal_I2C lcd(0x27, 20, 4); 
  Schrittmot Step1(SPR,SPEED);
  Serv Servo1(5,START1);
  //lcd.init();
  //lcd.backlight(); 

  
  aktorenini();  
  pinMode(LEFT_DIRPIN, OUTPUT);
  pinMode(LEFT_PULSEPIN, OUTPUT);
  pinMode(LEFT_ENABLEPIN, OUTPUT);
  pinMode(RIGHT_DIRPIN, OUTPUT);
  pinMode(RIGHT_PULSEPIN, OUTPUT);
  pinMode(RIGHT_ENABLEPIN, OUTPUT);

  // Setup-Pins mit internem Pullup
  pinMode(PROGRAM_PIN_0, INPUT_PULLUP);
  pinMode(PROGRAM_PIN_1, INPUT_PULLUP);
  
  // Kommunikations-Pins mit internem Pullup
//  pinMode(COMM_PIN_OUT, OUTPUT);
//  pinMode(COMM_PIN_IN, INPUT_PULLUP);
  
  digitalWrite(LEFT_DIRPIN, LOW);
  digitalWrite(RIGHT_DIRPIN, LOW);

  digitalWrite(LEFT_ENABLEPIN, HIGH);
  digitalWrite(RIGHT_ENABLEPIN, HIGH);
  pinMode(STP1,OUTPUT);
  pinMode(STP2,OUTPUT);
  pinMode(STP3,OUTPUT);
  pinMode(STP4,OUTPUT);
  // Abstandssensoren
  for(int i=0; i<NUMBEROFSENORS; i++) { //auf 4 runtergesetzt, zu wenig pins. Brauchen Register oder anderen Controller
    OldValue[i] = 0;
  }
  //pinMode(13, OUTPUT);
  //digitalWrite(13, LOW);

  //COMM_PIN_OUT_LOW; // Comm Pin Out HIGH

  program = (!digitalRead(PROGRAM_PIN_1) << 1) | !digitalRead(PROGRAM_PIN_0);

// den ganzen kram hab ich einfach mit reinkopiert
  initMotorDriverHardware(p_leftMd, LEFT_DIRPIN, LEFT_PULSEPIN, LEFT_ENABLEPIN, LEFT_FALSE_DIRECTION, UNIT, LEFT_CIRCUMFERENCE, LEFT_TICKSPERROTATION);
  initMotorDriverSoftware(p_leftMd, getChannel(), LEFT_MIN_ANGULARVELOCITY, LEFT_MIN_ANGULARACCELERATION, LEFT_MAX_ANGULARVELOCITY, LEFT_MAX_ANGULARACCELERATION, LEFT_VELOCITY_CONTROLINTERVALL, LEFT_DUTY_CYCLE);
  p_leftMd->whichTemp = 0; // Identifizierung
  setAngVelo(p_leftMd, LEFT_ACTUAL_ANGULARVELOCITY);
  setAngAccel(p_leftMd, LEFT_ACTUAL_ANGULARACCELERATION);
  initChannel(p_leftMd->ch, p_leftMd->highTime, p_leftMd->lowTime, timerControlRisingEdgeLeft, timerControlFallingEdgeLeft, p_leftMd, p_leftMd);

// Motor Driver Right
  initMotorDriverHardware(p_rightMd, RIGHT_DIRPIN, RIGHT_PULSEPIN, RIGHT_ENABLEPIN, RIGHT_FALSE_DIRECTION, UNIT, RIGHT_CIRCUMFERENCE, RIGHT_TICKSPERROTATION);
  initMotorDriverSoftware(p_rightMd, getChannel(), RIGHT_MIN_ANGULARVELOCITY, RIGHT_MIN_ANGULARACCELERATION, RIGHT_MAX_ANGULARVELOCITY, RIGHT_MAX_ANGULARACCELERATION, RIGHT_VELOCITY_CONTROLINTERVALL, RIGHT_DUTY_CYCLE);
  p_rightMd->whichTemp = 1;
  setAngVelo(p_rightMd, RIGHT_ACTUAL_ANGULARVELOCITY);
  setAngAccel(p_rightMd, RIGHT_ACTUAL_ANGULARACCELERATION);
  initChannel(p_rightMd->ch, p_rightMd->highTime, p_rightMd->lowTime, timerControlRisingEdgeRight, timerControlFallingEdgeRight, p_rightMd, p_rightMd);

  distToCenter = DISTANCE_TO_CENTER; // in cm -> In Zukunft Variable von DriveSystem

  timerSetup(); // Timer jetzt schon aktiv -> Unschön: Lieber erst aktivieren, sobald der erste Channel aktiv
unsigned long serles=0;
char buffers[2][6]; //array zum einlesen von Serieller ausgabe
  for(int i=0;i<2;i++){
  memset(buffers[i],0,6);
  }
int globalstatus=0;//initialisierung mit 0 ist in PlatformIO wichtig. Ich glaub, die Arduino IDE macht das automatisch
int strecke=0;
int j=0;
int k=0;
bool availprev=false;
unsigned long uper=0;
int status=0;
bool start=false;
int zut;
int dur=0;
unsigned long t_akt=0;
int status_lcd;
bool setd;
while(1){

    if(ser_prozed(serles,availprev,buffers,j,k,status,dur)){ //tritt auf, wenn eine Serielle Nachricht voll gelesen wurde
    //wichtig: Befehl= Numer,Dauer, also 5,100 für 100 schritte mit dem Schrittmotor
      setd=false;
      start=true;
setState(Step1,Servo1,status,dur); //für die befehle bzgl. Schritte und Servos
status_lcd=status;
    }

    
    //lcd_ausgabe(lcd,status_lcd,dur); //für display. zu debugging-zwecken. Auskommentieren, wenn nicht benötigt
    
    //Servo2.turner();
 if(!Step1.schritter()){ //wenn der Schrittmotor die richtige position hat

  if (micros()-t_akt>20000){ 
    t_akt=micros();
  if(!Servo1.servo_akt()){ //wenn der Servo die richtige Position hat
    if(status>4){ //status>4 bedeutet momentan, dass ein Befehl bezüglich der Schrittmotoren oder der Servos gegeben wurde 
      status=0;
      go=true;
    }
  }
 }
 }

    if (p_leftMd->calcNextAngVelo) {
    calcInterruptTime(p_leftMd);
  }
  if (p_rightMd->calcNextAngVelo) {
    calcInterruptTime(p_rightMd);
  }
  // Ereignis zwingt zum Stoppen
  if (false) {
    earlyStopNow(p_leftMd, p_rightMd);
    // Auslöser wieder auf false/deaktivieren etc.
  }

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= 100)
  {
//    Serial.begin(9600);
//    Serial.println(sensorFeedback());
//    Serial.end();
    previousMillis = currentMillis;
    // Verarbeitung von sensorFeedback();

serielle_statemachine(status,dur,start); //für die Fahrbasisbefehle
  }
  
  
  if(go){
    go=false;
    status=0;
    Serial.println("1"); // das hier ist der "fertig" befhel für den rpi
  }
}
}




void loop() {


  //  switch (program) {
  //   case 0: // Homologation
  //     notfall(true);
  //   break;
  //   case 1: // Links zuerst hinfahren
  //     notfall(true);
  //   break;
  //   case 2: // Rechts zuerst hinfahren
  //     notfall(false);
  //   break;
  //   case 3:
  //     notfall(false);
  //   break;
  // }
}
