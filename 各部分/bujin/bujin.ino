#include <Stepper.h>
#define STEPS 200  // 步进电机旋转一圈是多少步
Stepper stepper(STEPS, 3, 4, A2, A3);  
void setup(){
  stepper.setSpeed(100); // 转速，每s多少步
}
void loop(){
  int val = STEPS;
  stepper.step(val);
  delay(1000);
}
