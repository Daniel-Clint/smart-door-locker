#include <Adafruit_Fingerprint.h>
#include <Servo.h>

int pos=0;
Servo myservo; 
volatile bool buttonPressed=false;

SoftwareSerial mySerial(10, 11);//SoftwareSerial mySerial(rxPin, txPin)；
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

void buttonInterrupt()
{
  buttonPressed=true;
}

void setup()  
{
//触摸模块供电
pinMode(9,OUTPUT);
digitalWrite(9,HIGH);


  
  Serial.begin(115200);
  while (!Serial);
  delay(100);

  pinMode(12,OUTPUT);
  digitalWrite(12,LOW);
  // set the data rate for the sensor serial port
  finger.begin(57600);
  
  delay(5);
  if (finger.verifyPassword()) {
    digitalWrite(12,HIGH);
    delay(500);
    digitalWrite(12,LOW);
    delay(200);
    digitalWrite(12,HIGH);
    delay(200);
    digitalWrite(12,LOW);
    Serial.println("Found fingerprint sensor!");
    finger.LEDcontrol(0x04,0,0,0);
    finger.setPassword(839);
  }
  else {
    Serial.println("Did not find fingerprint sensor :(");
    wrong();
  }
  
  pinMode(LED_BUILTIN,OUTPUT);//13引脚
  digitalWrite(LED_BUILTIN,LOW);
  myservo.attach(5);  
  digitalWrite(5,LOW);

  attachInterrupt(digitalPinToInterrupt(3), buttonInterrupt, RISING);
  myservo.write(180);
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
//use
void openDoor(){
  Serial.println("open");
  //蜂鸣
  digitalWrite(12,HIGH);
  delay(500);
  digitalWrite(12,LOW);
  delay(200);
  digitalWrite(12,HIGH);
  delay(200);
  digitalWrite(12,LOW);
  
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


bool state=true;
void loop()
{
  if(buttonPressed)
  {
    openDoor();
    buttonPressed=false;
  }
  else
  {
    if(state){
      //蓝色
      Serial.println("prese!");
      finger.LEDcontrol(0x03,0,0x04,0);
      if(getFingerprintIDez() > -1) 
      {
        buttonPressed=false;
      }
      delay(500);
      //关闭
      finger.LEDcontrol(0x04,0,0,0);
      state=false;
     }
    else
    {
      state=true;
    }
  }
  delay(100);            //don't ned to run this at full speed.
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
// returns -1 if failed, otherwise returns ID #
int getFingerprintIDez() {
  Serial.println("getFingerprint");
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  {
    finger.LEDcontrol(0x02,0,0x01,0);
  return -1;
  }
  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  {
    finger.LEDcontrol(0x02,0,0x01,0);
  return -1;
  }
  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  {
    finger.LEDcontrol(0x02,0,0x01,0);
  return -1;
  }
  finger.LEDcontrol(0x02,0,0x02,0);
  // found a match!
  openDoor();
  
  Serial.print("Found ID #"); Serial.print(finger.fingerID); 
  Serial.print(" with confidence of "); Serial.println(finger.confidence);
  return finger.fingerID; 
}
