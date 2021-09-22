#include <DS3231.h>
#include <Wire.h>  
#include <LiquidCrystal.h> 
#include <EEPROM.h>

LiquidCrystal lcd(2, 3, 4, 5, 6, 7);

#define bt_time   A0
#define bt_up     A1
#define bt_down   A2
#define bt_alarm  A3

#define buzzer 8

#define lampu1 10
#define lampu2 11

DS3231  rtc(SDA, SCL);

Time  t; 

int hh = 0, mm = 0, ss = 0, dd = 0, bb = 0, set_day;
int yy = 0;
String Day = "  ";

int AlarmHH  = 21, AlarmMM  = 22, AlarmSS  = 23, setMode = 0, setAlarm = 0, alarmMode=0;
int AlarmHH2  = 22, AlarmMM2  = 23, AlarmSS2  = 24;
int stop =0, mode=0, flag=0;
byte bell_symbol[8] = {
        B00100,
        B01110,
        B01110,
        B01110,
        B01110,
        B11111,
        B01000,
        B00100};
        
int alarm_flag = 0;
char data = 0;

void setup(){
  Serial.begin(9600);
  rtc.begin(); 
  pinMode(bt_time,  INPUT_PULLUP); 
  pinMode(bt_up,    INPUT_PULLUP); 
  pinMode(bt_down,  INPUT_PULLUP); 
  pinMode(bt_alarm, INPUT_PULLUP); 
  pinMode(buzzer, OUTPUT);
  pinMode(lampu1, OUTPUT);
  pinMode(lampu2, OUTPUT);
  digitalWrite(lampu1, HIGH); 
  digitalWrite(lampu2, HIGH);
  lcd.createChar(1, thermometer_symbol);
  lcd.createChar(2, bell_symbol); 
  lcd.begin(16, 2); 
  lcd.setCursor(0,0);  
  lcd.setCursor (0,0);
  lcd.print("PRAKTIKUM SET");
  lcd.setCursor (0,1);
  lcd.print("Home Automation");
  delay (2000);
  lcd.clear();
  stop=EEPROM.read(50);
  if(stop==0){            
  }
  else{WriteEeprom ();}    
  EEPROM.write(50,0);       
  ReadEeprom();            
  //rtc.setDOW(2);     // Set Day-of-Week to SUNDAY
  //rtc.setTime (00, 9, 50); 
  //rtc.setDate(17, 07, 2021);  
  delay(1000);
}
//........................................................................................................
void loop(){  
  data = Serial.read();
  t = rtc.getTime();
  Day = rtc.getDOWStr(1); //(1) menyingkat Thursday -> Thu
  
  if (setMode == 0){
  hh = t.hour,DEC;
  mm = t.min,DEC;
  ss = t.sec,DEC;
  dd = t.date,DEC;
  bb = t.mon,DEC;
  yy = t.year,DEC;
  }  

  if(setAlarm==0){
  lcd.setCursor(4,0); 
  lcd.print((hh/10)%10);
  lcd.print(hh % 10); 
  lcd.print(":");
  lcd.print((mm/10)%10);
  lcd.print(mm % 10);
  lcd.print(":");
  lcd.print((ss/10)%10);
  lcd.print(ss % 10);
  lcd.print(" ");
  if(mode==1){lcd.write(2);}
  else{lcd.print(" ");}   
  lcd.print(" ");  

  lcd.setCursor(1,1);
  lcd.print(Day);
  lcd.print(" ");
  lcd.print((dd/10)%10);
  lcd.print(dd % 10); 
  lcd.print("/");
  lcd.print((bb/10)%10);
  lcd.print(bb % 10);
  lcd.print("/"); 
  lcd.print((yy/1000)%10);
  lcd.print((yy/100)%10);
  lcd.print((yy/10)%10);
  lcd.print(yy % 10);  
  }

  setupClock();
  setTimer();
  delay (100);
  blinking();
  //Serial.print("timer |");
  Serial.print(AlarmHH); Serial.print("|"); Serial.print(AlarmMM); Serial.print("|"); Serial.print(AlarmSS); Serial.print("|");
  Serial.print(AlarmHH2); Serial.print("|"); Serial.print(AlarmMM2); Serial.print("|"); Serial.print(AlarmSS2); Serial.print("|");
  relay1();
  relay2();
  delay (100);
}
//........................................................................................................
void blinking (){
//BLINKING SCREEN
if (setAlarm <2 && setMode == 1){lcd.setCursor(4,0);  lcd.print("  ");}
if (setAlarm <2 && setMode == 2){lcd.setCursor(7,0);  lcd.print("  ");}
if (setAlarm <2 && setMode == 3){lcd.setCursor(10,0);  lcd.print("  ");}
if (setAlarm <2 && setMode == 4){lcd.setCursor(1,1);  lcd.print("   ");}
if (setAlarm <2 && setMode == 5){lcd.setCursor(5,1);  lcd.print("  ");}
if (setAlarm <2 && setMode == 6){lcd.setCursor(8,1);  lcd.print("  ");}
if (setAlarm <2 && setMode == 7){lcd.setCursor(11,1); lcd.print("    "); }
//Alarm
if (setMode == 0 && setAlarm == 1){lcd.setCursor(6,0); lcd.print("           "); }
if (setMode == 0 && setAlarm == 2){lcd.setCursor(7,1); lcd.print("  "); }           
if (setMode == 0 && setAlarm == 3){lcd.setCursor(10,1); lcd.print("  "); }           
if (setMode == 0 && setAlarm == 4){lcd.setCursor(13,1);lcd.print("  "); }           

if (setMode == 0 && setAlarm == 5){lcd.setCursor(7,1); lcd.print("  "); }           
if (setMode == 0 && setAlarm == 6){lcd.setCursor(10,1); lcd.print("  "); }           
if (setMode == 0 && setAlarm == 7){lcd.setCursor(13,1);lcd.print("  "); }           
}

//---------------------------------------------------------------------------------------------------------
//Seting Jam ,Tanggal,Alarm/Timer
void setupClock (void) {
    if (setMode == 8){
    lcd.clear();
    lcd.setCursor (0,0);
   // lcd.print (F("Set Date Finish "));
    lcd.setCursor (0,1);
    lcd.print (F("Set Time Finish "));
    delay (1000);
    rtc.setTime (hh, mm, ss);
    rtc.setDate (dd, bb, yy);  
    lcd.clear();
    setMode = 0;
    }

    if (setAlarm == 8){                                       
    lcd.setCursor (0,0);
    lcd.clear();
    lcd.print (F("Setting Lampu "));
    lcd.setCursor (0,1);
    lcd.print (F("Finish"));
    WriteEeprom();      /
    delay (2000); 
    lcd.clear();
    setAlarm=0;
    alarmMode=1;
    }
    
    if (setAlarm >0){ alarmMode=0;}   
     if(digitalRead (bt_time) == 0 && flag==0) 
     {
        flag=1;
        if(setAlarm>0){setAlarm=8;}                           
     else{setMode = setMode+1;}
     }
      
     if(digitalRead (bt_alarm) == 0 && flag==0)
     {
        flag=1;
        if(setMode>0){setMode=8;}
     else{setAlarm = setAlarm+1;} 
        lcd.clear();
      }      

    if(digitalRead (bt_time) == 1 && digitalRead (bt_alarm) == 1){flag=0;}

    if(digitalRead (bt_up) == 0){                          
        if (setAlarm<2 && setMode==1)hh=hh+1; 
        if (setAlarm<2 && setMode==2)mm=mm+1;
        if (setAlarm<2 && setMode==3)ss=ss+1;
        if (setAlarm<2 && setMode==4)set_day=set_day+1;
        if (setAlarm<2 && setMode==5)dd=dd+1;
        if (setAlarm<2 && setMode==6)bb=bb+1;
        if (setAlarm<2 && setMode==7)yy=yy+1;
        //Alarm
        if (setMode==0 && setAlarm==1)mode=1;
        if (setMode==0 && setAlarm==2 )AlarmHH=AlarmHH+1;   
        if (setMode==0 && setAlarm==3 )AlarmMM=AlarmMM+1;   
        if (setMode==0 && setAlarm==4 )AlarmSS=AlarmSS+1;    
        if (setMode==0 && setAlarm==5 )AlarmHH2=AlarmHH2+1;   
        if (setMode==0 && setAlarm==6 )AlarmMM2=AlarmMM2+1;   
        if (setMode==0 && setAlarm==7 )AlarmSS2=AlarmSS2+1;   
    
    
        if(hh>23)hh=0;
        if(mm>59)mm=0;
        if(ss>59)ss=0;
        if(set_day>7)set_day=0;
        if(dd>31)dd=0;
        if(bb>12)bb=0;
        if(yy>2030)yy=2000;
        rtc.setDOW(set_day);
        if(AlarmHH>23)AlarmHH=0;                                            
        if(AlarmMM>59)AlarmMM=0;
        if(AlarmSS>59)AlarmSS=0;        
        if(AlarmHH2>23)AlarmHH2=0;
        if(AlarmMM2>59)AlarmMM2=0;
        if(AlarmSS2>59)AlarmSS2=0; 
    }        

    if(digitalRead (bt_down) == 0){                                      
        if (setAlarm<2 && setMode==1)hh=hh-1; 
        if (setAlarm<2 && setMode==2)mm=mm-1;
        if (setAlarm<2 && setMode==3)ss=ss-1;
        if (setAlarm<2 && setMode==4)set_day=set_day-1;
        if (setAlarm<2 && setMode==5)dd=dd-1;
        if (setAlarm<2 && setMode==6)bb=bb-1;
        if (setAlarm<2 && setMode==7)yy=yy-1;
        //Alarm
        if (setMode==0 && setAlarm==1 )mode=0;
        if (setMode==0 && setAlarm==2 )AlarmHH=AlarmHH-1;   
        if (setMode==0 && setAlarm==3 )AlarmMM=AlarmMM-1;  
        if (setMode==0 && setAlarm==4 )AlarmSS=AlarmSS-1;   

        if (setMode==0 && setAlarm==5 )AlarmHH2=AlarmHH2-1;    
        if (setMode==0 && setAlarm==6 )AlarmMM2=AlarmMM2-1;    
        if (setMode==0 && setAlarm==7 )AlarmSS2=AlarmSS2-1;    


                
        if(hh<0)hh=23;
        if(mm<0)mm=59;
        if(ss<0)ss=59;
        if(set_day<0)set_day=7;
        if(dd<0)dd=31;
        if(bb<0)bb=12;
        if(yy<0)yy=2030;
        rtc.setDOW(set_day);
        if(AlarmHH  <0)AlarmHH=23;
        if(AlarmMM  <0)AlarmMM=59;
        if(AlarmSS  <0)AlarmSS=59;
        if(AlarmHH2 <0)AlarmHH2=23;
        if(AlarmMM2 <0)AlarmMM2=59;
        if(AlarmSS2 <0)AlarmSS2=59;
     }
}


void setTimer (){
  //Alarm
  if (setMode == 0 && setAlarm >0 && setAlarm <5){                     
    lcd.setCursor (0,0);
    lcd.print("Lampu ");
    if(mode==0){lcd.print("Deactivate");}
      else{lcd.print("Activated ");}
    lcd.setCursor (0,1);                                                
    lcd.print("ON : ");                                            
    lcd.setCursor (7,1);                                                
    lcd.print((AlarmHH/10)%10);
    lcd.print(AlarmHH % 10);
    lcd.print(":");
    lcd.print((AlarmMM/10)%10);
    lcd.print(AlarmMM % 10);
    lcd.print(":");
    lcd.print((AlarmSS/10)%10);
    lcd.print(AlarmSS % 10);
 }
 if (setMode == 0 && setAlarm >4 && setAlarm <8){
    lcd.setCursor (0,0);
    lcd.print("Lampu ");
    if(mode==0){lcd.print("Deactivate");}
      else{lcd.print("Activated ");}
    lcd.setCursor (0,1);
    lcd.print("OFF : ");
    lcd.setCursor (7,1);
    lcd.print((AlarmHH2/10)%10);
    lcd.print(AlarmHH2 % 10);
    lcd.print(":");
    lcd.print((AlarmMM2/10)%10);
    lcd.print(AlarmMM2 % 10);
    lcd.print(":");
    lcd.print((AlarmSS2/10)%10);
    lcd.print(AlarmSS2 % 10);
 }
}

void relay1(){
  if (alarmMode==1 && mode==1 && hh==AlarmHH && mm==AlarmMM && ss==AlarmSS) {
  lcd.clear();
  lcd.setCursor (0,0);
  lcd.print("Lampu Akan On : ");
  lcd.setCursor (7,1);
  lcd.print("3");
  delay(1000);
  lcd.setCursor (7,1);
  lcd.print("2");
  delay(1000);
  lcd.setCursor (7,1);
  lcd.print("1");
  delay(1000);  
  digitalWrite(lampu1, LOW);
  lcd.clear();
  }
  else if (alarmMode==1 && mode==1 && hh==AlarmHH2 && mm==AlarmMM2 && ss==AlarmSS2) { 
  lcd.clear();
  lcd.setCursor (0,0);
  lcd.print("Lampu Akan OFF : ");
  lcd.setCursor (7,1);
  lcd.print("3");
  delay(1000);
  lcd.setCursor (7,1);
  lcd.print("2");
  delay(1000);
  lcd.setCursor (7,1);
  lcd.print("1");
  delay(1000);  
  digitalWrite(lampu1, HIGH);
  lcd.clear();
  }
  else if (data == '3') {digitalWrite(lampu1, LOW);}
  else if (data == '4') {digitalWrite(lampu1, HIGH);}
  
}
void relay2() {
  if (data == '1') {digitalWrite(lampu2, LOW);}
  else if (data == '2') {digitalWrite(lampu2, HIGH);} 
}



void ReadEeprom () {
  AlarmHH=EEPROM.read(1);
  AlarmMM=EEPROM.read(2);
  AlarmSS=EEPROM.read(3);
  
  mode=EEPROM.read(4); 
  AlarmHH2=EEPROM.read(5);
  AlarmMM2=EEPROM.read(6);
  AlarmSS2=EEPROM.read(7);
}

void WriteEeprom () {
  EEPROM.write(1,AlarmHH);
  EEPROM.write(2,AlarmMM);
  EEPROM.write(3,AlarmSS);
  
  EEPROM.write(4,mode);
  EEPROM.write(5,AlarmHH2);
  EEPROM.write(6,AlarmMM2);
  EEPROM.write(7,AlarmSS2);
}
