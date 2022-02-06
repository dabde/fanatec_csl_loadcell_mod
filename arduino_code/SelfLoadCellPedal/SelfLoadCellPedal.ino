#include "Joystick.h"
#include "MultiMap.h"

struct PedelsMap {
  int inAccMM[9];
  int outAccMM[9];
  int inBreakMM[9];
  int outBreakMM[9];
};

// added but not yet used to make non linear curves
// tested hall sensor linearity, only for the first 5° and the last 3° (23°) has a small non linear behavior of 3%
PedelsMap MyPedals  = {
  {0, 128, 256, 384, 512, 640, 768, 896, 1024}, // input mapping acc
  {0, 128, 256, 384, 512, 640, 768, 896, 1024}, // output mapping acc
  {520, 576, 640, 704, 768, 832, 896, 960, 1024}, // input mapping break (OpAmp is going from 2.5V to 5V, but also possible to do via windows calibration)
  {0, 128, 256, 384, 512, 640, 768, 896, 1024}, // output mapping break
};
float breakAvg = 0.0;
Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID, 
  JOYSTICK_TYPE_MULTI_AXIS,
  0, // buttons
  0, // hatSwitchCount
  false, // XAxis
  false, // YAxis
  false, // ZAxis
  false, // RxAxis
  false, // RyAxis
  false, // RzAxis
  false, // Ruder
  true, // Throttle (used for acc)
  false, // Accelerator
  true, // Break
  false); // Streering

void setup() {

  Serial.begin(115200);

  Joystick.setThrottleRange(0, 1024);
  Joystick.setBrakeRange(0, 1024);
  Joystick.begin(false);
  
  pinMode(A0, INPUT_PULLUP);
  pinMode(A3, INPUT_PULLUP);
}

void loop() {
  int accVal = multiMap<int>(1024-analogRead(A0),MyPedals.inAccMM,MyPedals.outAccMM, sizeof(MyPedals.inAccMM));

  int breakRead = analogRead(A3);

  // smooth low changes, keep speed for fast action
  if ((abs(breakRead - breakAvg)>40))
  {
    breakAvg = (breakRead + breakAvg*2) / 3.0;
  } else {
    breakAvg = (breakAvg*49.0 + breakRead) / 50.0;
  }
  int breakVal = multiMap<int>(round(breakAvg),MyPedals.inBreakMM,MyPedals.outBreakMM, sizeof(MyPedals.inBreakMM));

  Serial.print(0);
  Serial.print(" ");
  Serial.print(1024);
  Serial.print(" ");  
  Serial.println(breakVal);
  
  Joystick.setThrottle(accVal);
  Joystick.setBrake(breakVal);
  Joystick.sendState();
}
