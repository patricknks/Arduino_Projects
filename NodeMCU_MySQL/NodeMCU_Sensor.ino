#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#define sensorPin1 0
char ssid[] = ""; //Network Name
char pass[] = ""; //Network Password
byte mac[6];
int datasensor[10];
unsigned long tim;
WiFiServer server(80);
WiFiClient client; 
MySQL_Connection conn((Client*)&client);
char INSERT_SQL[] = "INSERT INTO sql6421109.esp8266(sensor1, sensor2, sensor3)VALUES (%d,%d,%d)";
char query[128];
IPAddress server_addr(52,74,77,8); // MySQL server IP
char user[] ="sql6421109"; // MySQL user
char password[] ="cdYTE6irHl"; // MySQL password


void setup() {
 Serial.begin(9600);
 pinMode(sensorPin1, INPUT);
 //pinMode(sensorPin2, INPUT);
 Serial.println("Initialising connection");
 Serial.print(F("Setting static ip to : "));
 Serial.println(""); 
 Serial.println("");
 Serial.print("Connecting to ");
 Serial.println(ssid);
 WiFi.begin(ssid, pass);
 while (WiFi.status() != WL_CONNECTED)
  {
   delay(200);
   Serial.print(".");
  }
 Serial.println("");
 Serial.println("WiFi Connected");
 WiFi.macAddress(mac);
 Serial.print("MAC: ");
 Serial.print(mac[5],HEX);
 Serial.print(":");
 Serial.print(mac[4],HEX);
 Serial.print(":");
 Serial.print(mac[3],HEX);
 Serial.print(":");
 Serial.print(mac[2],HEX);
 Serial.print(":"); 
 Serial.print(mac[1],HEX);
 Serial.print(":");
 Serial.println(mac[0],HEX);
 Serial.println("");
 Serial.print("Assigned IP: ");
 Serial.print(WiFi.localIP());
 Serial.println("");
 Serial.println("Connecting to database");
 while (conn.connect(server_addr, 3306 ,user, password) != true) {
   delay(1000);
   Serial.print ( "." );
   }
 Serial.println("");
 Serial.println("Connected to SQL Server!");
 datasensor[0]=103;
 datasensor[1]=20;
 datasensor[2]=103;
} 



void loop() {
 datasensor[0]++;
 datasensor[1]++;
 //datasensor[2]++;
 tim = millis();
 MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
 for(int b=0;b<1;b++)
   {
   sprintf(query, INSERT_SQL,datasensor[0],datasensor[1],datasensor[3]);
   cur_mem->execute(query);
   }
 delete cur_mem;
 tim = millis()-tim;
 Serial.println(tim);
 delay(1000);
}
