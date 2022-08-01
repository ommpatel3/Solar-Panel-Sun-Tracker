#include <Servo.h>

int VRx = A4;
int VRy = A5;
int SW = 2;

int PR_TR = A0;
int PR_TL = A1;
int PR_BR = A2;
int PR_BL = A3;

int xPosition = 0;
int yPosition = 0;
int SW_state = 0;
int Prev_SW = 0;
int mapX = 0;
int mapY = 0;

int TopRight = 0;
int TopLeft = 0;
int BottomRight = 0;
int BottomLeft = 0;
int topAvg = 0;
int botAvg = 0;
int rAvg = 0;
int lAvg = 0;

String mode = "freeze";

Servo azimuth;
Servo elevation;
int azAngle = 90;
int elAngle = 90;

bool home = false;

void setup() {
  Serial.begin(9600); 
  
  pinMode(VRx, INPUT);
  pinMode(VRy, INPUT);
  pinMode(SW, INPUT_PULLUP); 

  azimuth.attach(10);
  elevation.attach(9);

}

void loop() {

  Prev_SW = SW_state;
  SW_state = ! digitalRead(SW);

  if (Prev_SW == 1 && SW_state == 0){
    if (mode == "auto"){
      mode = "manual";
    }
    else if (mode == "manual"){
      mode = "freeze";
    }
    else if (mode == "freeze"){
      mode = "auto";
    }
    Serial.println(mode);
  }

  if (mode == "manual")
  {
    xPosition = analogRead(VRx);
    yPosition = analogRead(VRy);
    
    mapX = map(xPosition, 0, 1023, -5, 5);
    mapY = map(yPosition, 0, 1023, -5, 5);

    rotateManual(elevation, elAngle, mapY);
    rotateManual(azimuth, azAngle, mapX); 
  }
  else if (mode == "auto")
  {
    TopRight = analogRead(PR_TR);
    TopLeft = analogRead(PR_TL);
    BottomRight = analogRead(PR_BR);
    BottomLeft = analogRead(PR_BL);

    topAvg = (TopRight + TopLeft)/2;
    botAvg = (BottomRight + BottomLeft)/2;
    rAvg = (TopRight + BottomRight)/2;
    lAvg = (TopLeft + BottomLeft)/2;

    rotateAuto(elevation, elAngle, topAvg, botAvg);
    rotateAuto(azimuth, azAngle, rAvg, lAvg);

//// ------------for debugging purposes:
////    Serial.print("Top: ");
////    Serial.println(topAvg);
////
////    Serial.print("Left: ");
////    Serial.print(lAvg);
////    Serial.print("  Right: ");
////    Serial.println(rAvg);
////    
////    Serial.print("Bottom: ");
////    Serial.println(botAvg);
////
////    Serial.println("");
////
////    
////    Serial.print(" TL: ");
////    Serial.print(TopLeft);
////    Serial.print("  TR: ");
////    Serial.print(TopRight);
////    Serial.println("");
////    Serial.print(" BL: ");
////    Serial.print(BottomLeft);
////    Serial.print("  BR: ");
////    Serial.print(BottomRight);
////
////    Serial.println("");
  }
  delay(20);
}  

void rotateManual (Servo motor, int &angle, int input)
{
    if (input == -4 || input == -5){
      angle -= 1;
    }
    else if (input == -2 || input == -3){
      //angle -=0.5;
    }
    else if (input == 3 || input == 2){
      //angle +=0.5;
    }
    else if (input == 5 || input == 4){
      angle += 1;
    }
  
    if (angle >= 175){
      angle = 175;
    }
    else if (angle <=5){
      angle = 5;
    }
    motor.write(angle);
}

void rotateAuto (Servo motor, int &angle, int positive, int negative){
  //net gives us the direction to move in
  int net = positive - negative;

  if (net > 5)
  {
    angle += 1;
  }
  if (net < -5)
  {
    angle -= 1;
  }
  //Serial.println(angle);
  motor.write(angle);
  delay(15);
}
