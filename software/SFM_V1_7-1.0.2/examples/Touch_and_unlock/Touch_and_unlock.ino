#include "sfm.hpp"

#define SFM_RX 10
#define SFM_TX 11
#define SFM_IRQ 2
#define SFM_VCC 5

SFM_Module SFM(SFM_VCC, SFM_IRQ, SFM_TX, SFM_RX);

bool lastTouchState = 0;

void sfmPinInt1() {
  SFM.pinInterrupt();
}

void setup() {
  SFM.setPinInterrupt(sfmPinInt1); // must perform this step in setup() to attach the inner interrupt.
  Serial.begin(115200); // not affiliated with module, just print unlock result.
  SFM.setRingColor(SFM_RING_GREEN,SFM_RING_BLUE,1500);
  Serial.print("innitial");
}

uint8_t temp = 0; // used to get recognition return
uint16_t tempUid = 0; // used to get recognized uid

void loop() {
  if(SFM.isTouched() != lastTouchState) {Serial.print("touch"); // Make sure the action just performed once when touch state changes
    lastTouchState = SFM.isTouched();
    if(SFM.isTouched()) {
      SFM.setRingColor(SFM_RING_RED, SFM_RING_OFF); // Ring fade from red to black at default period (500ms), creating a breathe effect
      // Here we are going to start recognition, if unlocked, we will change the ring color to green and send a message
      temp = SFM.recognition_1vN(tempUid);
      if(tempUid != 0) {
        SFM.setRingColor(SFM_RING_GREEN);
        Serial.print("Successfully matched with UID: %d\n");
        Serial.print(tempUid);
      }
    }
    else {
      SFM.setRingColor(SFM_RING_GREEN,SFM_RING_BLUE,1500);
    }
  }
}
