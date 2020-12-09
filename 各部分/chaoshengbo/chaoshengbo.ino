#include <DFRobot_URM10.h>

DFRobot_URM10 urm10(11,12);

void setup() {
  Serial.begin(9600);
  pinMode(13,OUTPUT);
}

void loop() {
  Serial.println((urm10.getDistanceCM()));
  if (((36 > (urm10.getDistanceCM())))) {
    digitalWrite(13, HIGH);
  }
  else {
    digitalWrite(13, LOW);
  }
}
