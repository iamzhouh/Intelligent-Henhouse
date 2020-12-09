int ledPin = 13; 
int val;
void setup()
{
  Serial.begin(9600); // open serial port, set the baud rate to 9600 bps
  pinMode(ledPin, OUTPUT);
}

void loop()
{
  val=analogRead(0); 
  if (600>val>0)  
  {
    digitalWrite(ledPin,HIGH);
    Serial.print("guangqiang:");
    Serial.println(val);
  }
  else
  {
    digitalWrite(ledPin,LOW);
    Serial.print("guangqiang:");
    Serial.println(val);
  }       
  delay(100);
}
