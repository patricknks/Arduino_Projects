#include <Wire.h>
#include<Servo.h>
Servo servo;
#define n 100
#define n1 10
#define w1 0.9998
#define w2 0.0002

const int ledpin =  LED_BUILTIN;

int val, s_angle;
long aX0,aY0,aZ0, aX1=0,aY1=0,aZ1=0, aX2=0,aY2=0,aZ2=0;
long gX0,gY0,gZ0, gX1=0,gY1=0,gZ1=0, gX2=0,gY2=0,gZ2=0;

float suhu;
float aX,aY,aZ;
float gX,gY,gZ;
float accRoll, accPitch; //sudut dari accel
float gyrRoll, gyrPitch; //sudut dari gyro
float roll, pitch; //value after filter

float dt; //interval waktu (timer)
unsigned long waktu;
void setup() {
  // put your setup code here, to run once:
  pinMode(ledpin, OUTPUT);
  digitalWrite(ledpin, LOW);
  servo.attach(6);
  servo.write(90);
  Wire.begin();
  Serial.begin(115200);

//reset acc, gyro,temp
  Wire.beginTransmission(0x68);
  Wire.write(0x68); 
  Wire.write(0b00000111); 
  Wire.endTransmission();
//reset acc, gyro,temp
  Wire.beginTransmission(0x68);
  Wire.write(0x68); 
  Wire.write(0b00000000); 
  Wire.endTransmission();

  Wire.beginTransmission(0x68);
  Wire.write(0x6B); //power management
  Wire.write(0x00);
  Wire.endTransmission();

//DLPF ,1, 184hz -> acc   188hz ->gyr
  Wire.beginTransmission(0x68);
  Wire.write(0x1A); 
  Wire.write(0b00000000); 
  Wire.endTransmission();

  Wire.beginTransmission(0x68);
  Wire.write(0x1B); //setting gyro
  Wire.write(0b00);
  Wire.endTransmission();

  Wire.beginTransmission(0x68);
  Wire.write(0x1C); //setting accel
  Wire.write(0b00000000); // accel dan DHPF
  Wire.endTransmission();
  Serial.println("  MULAI KALIBRASI ");
  digitalWrite(ledpin, HIGH);
  
  
//////////////////////KALIBRASI//////////////////////////
  for(int i=0; i<n; i++) 
  {
    Serial.print(" Kalbrasi : "); Serial.println(i);
    Wire.beginTransmission(0x68);
    Wire.write(0x3B); 
    Wire.endTransmission();
    while(Wire.available() < 14) //ambil semua dari accel,suhu,gyro
    Wire.requestFrom(0x68,14);
    aX0 = Wire.read() << 8 | Wire.read();
    aY0 = Wire.read() << 8 | Wire.read();
    aZ0 = Wire.read() << 8 | Wire.read();
    suhu = Wire.read() << 8 | Wire.read();
    gX0 = Wire.read() << 8 | Wire.read();
    gY0 = Wire.read() << 8 | Wire.read();
    gZ0 = Wire.read() << 8 | Wire.read();

    aX1 +=aX0;
    aY1 +=aY0;
    aZ1 +=aZ0;
    gX1 += gX0;
    gY1 += gY0;
    gZ1 += gZ0;
    delay(20);
    if(i==n-1) {
      aX1 /=n;
      aY1 /=n;
      aZ1 /=n;
      gX1 /=n;
      gY1 /=n;
      gZ1 /=n;
      break;
    } 
  }
  delay(2000);
  digitalWrite(ledpin,LOW);
  Serial.println("  NILAI rata rata : ");
  Serial.print("aX : ");  Serial.print(aX1);
  Serial.print("\t aY : "); Serial.print(aY1);
  Serial.print("\t aZ : "); Serial.print(aZ1);
  Serial.print("\t gY : "); Serial.print(gX1);
  Serial.print("\t gY : "); Serial.print(gY1);
  Serial.print("\t gY : "); Serial.println(gZ1);
  delay(3000);
  
  waktu = millis();
}

void loop() {
  // put your main code here, to run repeatedly:
  Wire.beginTransmission(0x68);
  Wire.write(0x3B); 
  Wire.endTransmission();
  while(Wire.available() < 8) //ambil semua dari accel,suhu,gyro
  Wire.requestFrom(0x68,8);
  aX2 = Wire.read() << 8 | Wire.read();
  aY2 = Wire.read() << 8 | Wire.read();
  aZ2 = Wire.read() << 8 | Wire.read();
  suhu = Wire.read() << 8 | Wire.read();

  aX = (float) (aX2 + (-1*aX1)) / 16384.00;
  aY = (float) (aY2 + (-1*aY1)) / 16384.00;
  aZ = (float) (aZ2 + (16384 - aZ1)) / 16384.00;

  accRoll = (atan2(aX,aZ))*57.2958;
  accPitch = (atan2(aY,aZ))*57.2958;

  for (int i = 0; i<n1 ; i++)
  {
     dt = (millis() - waktu)/1000.00;
     Wire.beginTransmission(0x68);
     Wire.write(0x43); 
     Wire.endTransmission();
     while(Wire.available() < 6) //ambil semua dari accel,suhu,gyro
     Wire.requestFrom(0x68,6);
     gX2 = Wire.read() << 8 | Wire.read();
     gY2 = Wire.read() << 8 | Wire.read();
     gZ2 = Wire.read() << 8 | Wire.read();

     gX = (float) (gX2 + (-1*gX1)) / 131;
     gY = (float) (gY2 + (-1*gY1)) / 131;
     gZ = (float) (gZ2 + (-1*gZ1)) / 131;

     gyrRoll += gY*dt;
     gyrPitch += gX*dt;
     if(i==n-1)
     {
        gyrRoll = (gyrRoll/n)*57.2958;
        gyrPitch = (gyrPitch/n)*57.2958;
        break;
     }
  }

  //COMPLEMENTARY FILTER
  roll = (accRoll * w1 + gyrRoll * w2) / (w1+w2);
  pitch = (accPitch * w1 + gyrPitch * w2) / (w1+w2);
  
  Serial.print(accRoll);
  Serial.print(",   ");
  Serial.print(accPitch);
  Serial.print(",               ");
  Serial.print(gyrRoll);
  Serial.print(",   ");
  Serial.print(gyrPitch);
  Serial.print(",               ");
  Serial.print(roll);
  Serial.print(",   ");
  Serial.println(pitch);

  //angle   = map(pot_val,0,1023,0,180);
  //delay(200);  
  val   = -1 * (pitch - 90);
  servo.write(val);
  delay(100);
  waktu = millis();
}
