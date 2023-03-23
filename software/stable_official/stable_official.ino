#include "sfm.hpp"
#include <Servo.h>

#define SFM_RX 10
#define SFM_TX 11
#define SFM_IRQ 2
#define SFM_VCC 5

SFM_Module SFM(SFM_VCC, SFM_IRQ, SFM_TX, SFM_RX);

bool lastTouchState = 0;
int pos = 0;
Servo myservo;
volatile bool buttonPressed = false;

void sfmPinInt1() {
  SFM.pinInterrupt();
}

void setup() {
  SFM.setPinInterrupt(sfmPinInt1); // must perform this step in setup() to attach the inner interrupt.
  Serial.begin(115200); // not affiliated with module, just print unlock result.
  SFM.setRingColor(SFM_RING_GREEN,SFM_RING_BLUE,1500);
  Serial.print("innitial");

  //蜂鸣器
  pinMode(12, OUTPUT);
  digitalWrite(12, LOW);

  //舵机
  myservo.attach(5);
  digitalWrite(5, LOW);

  //触摸模块供电
  pinMode(9, OUTPUT);
  digitalWrite(9, HIGH);

  //断点
  attachInterrupt(digitalPinToInterrupt(3), buttonInterrupt, RISING);
  myservo.write(180);
  OK();
}

void buttonInterrupt()
{
  buttonPressed=true;
}

//舵机pwm函数
void servoControl(int pul){
  for(int i=0;i<=5;i++){
    digitalWrite(5,HIGH);
    delayMicroseconds(pul);
    digitalWrite(5,LOW);
    delayMicroseconds(20000-pul);
  }
}

void openDoor(){
  Serial.println("open");
  //蜂鸣
 OK();
  
 for (pos = 180; pos >= 0; pos --) { 
    myservo.write(pos);            
    delay(6);                      
  }    
  delay(2000);
 for (pos = 0; pos <= 180; pos ++) { 
    myservo.write(pos);           
    delay(6);              
  }
}

void OK()
{
  digitalWrite(12,HIGH);
  delay(500);
  digitalWrite(12,LOW);
  delay(200);
  digitalWrite(12,HIGH);
  delay(200);
  digitalWrite(12,LOW);
}

void wrong()
{
  for(int i=1;i<4;i++) 
    { 
      digitalWrite(12,HIGH);
      delay(200);
      digitalWrite(12,LOW);
      delay(400);
      delay(1); 
    }
}

uint8_t temp = 0; // used to get recognition return
uint16_t tempUid = 0; // used to get recognized uid

void loop() {
  if(buttonPressed)
  {
    openDoor();
    buttonPressed=false;
  }
  if(SFM.isTouched() != lastTouchState) 
  { // Make sure the action just performed once when touch state changes
    lastTouchState = SFM.isTouched();
    if(SFM.isTouched()) 
    {
      SFM.setRingColor(SFM_RING_RED, SFM_RING_OFF); // Ring fade from red to black at default period (500ms), creating a breathe effect
      // Here we are going to start recognition, if unlocked, we will change the ring color to green and send a message
      temp = SFM.recognition_1vN(tempUid);
      if(tempUid != 0) 
      {
        SFM.setRingColor(SFM_RING_GREEN);
        Serial.print("Successfully matched with UID: %d\n");
        Serial.print(tempUid);
        openDoor();
        SFM.setRingColor(SFM_RING_GREEN,SFM_RING_BLUE,1500);
      }
      else
      {
        wrong();
      }
    }
  }
}
