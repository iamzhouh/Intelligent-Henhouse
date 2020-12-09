double temp,data;
void setup() {
  Serial.begin(9600);
  pinMode(10,OUTPUT);
}
void loop() {
  // put your main code here, to run repeatedly:
  temp=(long)analogRead(1);
  data=(temp/650)*4;
  Serial.println(data);
  delay(100);
  if(data==0){
    digitalWrite(10, LOW);
    }
  if(data>=4){
    digitalWrite(10, HIGH);
    }
}
