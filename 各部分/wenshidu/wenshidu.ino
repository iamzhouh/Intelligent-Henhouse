
#include "DHT.h"  
#define DHTPIN 8 
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
void setup() {
  Serial.begin(9600);
  pinMode(9,OUTPUT);
  dht.begin();
}
void loop() {
  float shidu = dht.readHumidity();
  float wendu = dht.readTemperature();
  if(int(wendu) >= 30)
  {
    digitalWrite(9, LOW);
   }
   else{
    digitalWrite(9, HIGH);
   }

}
