class LiquidCrystal_I2C;
class Servo1;
class Schrittmot;
bool ser_prozed(unsigned long& serles,bool& availprev,char buffers[2][6],int& j,int& k,int& Status,int& dur){
  //serles:long für den abgleich mit micros()
  //availprev:die Funktion wird nach 4 millisekundne aufgerufen, dieser bool gibt an, ob beim letzten mal was gelesen wurde
  //buffers: 2 Strings, um Nachricht zu speichern
  //j:in welchen string soll der gelesene char rein?
  //k:in welche Position des Strings soll der char rein?
  //Status: Befehl
  //dur: Magnitude(entweder anzahl Schritte oder Position des Servos)
      if (Serial.available() && micros()-serles>4000){//Serial-buffer braucht 3 ms, um sich zu füllen, hab ich gehört
  char akt;
  availprev=true;
  serles=micros();
    akt=Serial.read();
            if(akt==','){
            j++;
            k=0;
        }
        else{
          buffers[j][k]=akt;
          k++;
        }
  }
      else if(Serial.available()==0 && availprev && micros()-serles>4000){
      
      k=0;
      j=0;
      serles=micros();
  availprev=false;
     Status=atoi(buffers[0]);
     dur=atoi(buffers[1]);

  for(int i=0;i<2;i++){
  memset(buffers[i],0,6);
  }
  return true;
  //Serial.print(z1);Serial.print("\t");Serial.print(z2);Serial.print("\t");Serial.println(schritte);
  }
  return false;
}

void setState(Schrittmot& Step1, Serv& Servo1,int Status, int& strecke){
  //gibt befehl weiter, 0-4 sind für die Fahrbasis
    switch(Status){
        case 5:
        
        Step1.schritte=strecke;
        break;
        case 6:
        Step1.schritte=-strecke;
        break;
        case 7:
        Servo1.z_soll=strecke;
        break;
        case 8:
        //Servo2.z_soll=strecke;
        break;
        default:
        strecke=strecke;
    }
}


void lcd_ausgabe(LiquidCrystal_I2C& lcd,int status,int dur){
  lcd.setCursor(0, 0);//Hier wird die Position des ersten Zeichens festgelegt. In diesem Fall bedeutet (0,0) das erste Zeichen in der ersten Zeile. 
lcd.print("Befehl:"); 
lcd.setCursor(0, 1);// In diesem Fall bedeutet (0,1) das erste Zeichen in der zweiten Zeile. 
lcd.print(status); 
lcd.setCursor(0, 2);// In diesem Fall bedeutet (0,1) das erste Zeichen in der zweiten Zeile. 
lcd.print(dur); 
}
