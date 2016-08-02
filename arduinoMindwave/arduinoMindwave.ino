#include "MeOrion.h"

//attachments
MeRGBLed led(PORT_3);
MeDCMotor Motor1(M1);
MeDCMotor Motor2(M2);
MeDCMotor Hand(PORT_1);
MeDCMotor Arm(PORT_2);
MeTemperature myTemp(PORT_8, SLOT2);
MeUltrasonicSensor ultraSensor(PORT_7); /* Ultrasonic module can ONLY be connected to port 3, 4, 6, 7, 8 of base shield. */

//DECLARE VARIALBLES

//messages from node
String input;

//robot arm
boolean handPosition = false;
int handSpeed = 200;

/*
//temperature
int tempTimer = 0;
int lastTempTimer = 0;
int tempDelta = 0;
*/

//ultrasonic sensor
int distanceTimer = 0;

int lastDistanceTimer = 0;
int distanceDelta = 0;

boolean distanceTooShort = false;

void setup()
{
Serial.begin(9600);
Serial.setTimeout(10);
}

void loop() 
{
  //checkTemp(10000);
  checkDistance(1000);
  
  input = "";
  
  if(distanceTooShort)
  {
    //prevent the robot from hitting a wall!
    stopRobot();
    blinkLights(20, 20, 20);
    turnLeft();
    return;
  }
  
  if(Serial.available() > 0)
  {
      input = Serial.readStringUntil('\n');
  }

  //Serial.println(input); //for debugging only
  
  if(input == "stop") 
  {
    blinkLights(20, 0, 0);
    stopRobot();
  }
  
  if(input == "circle")
  {
    blinkLights(20, 0, 20);
    circle();
  }
  
  if(input == "forward")
  {
    blinkLights(0, 20, 0);
    driveForward();
  }
  
  if(input == "backward")
  {
    blinkLights(20, 20, 0);
    driveBackward();
  }
  
  if(input == "left")
  {
    blinkLights(0, 20, 20);
    turnLeft();
  }

  if(input == "right")
  {
    blinkLights(0, 20, 20);
    turnRight();
  }
  
  if(input == "openHand")
  {
    blinkLights(20, 50, 20);
    openHand();
  }
  
  if(input == "closeHand")
  {
    blinkLights(50, 20, 20);
    closeHand();
  }
  
  if(input == "raiseArm")
  {
    blinkLights(40, 30, 20);
    raiseArm();
  }
  
  if(input == "lowerArm")
  {
    blinkLights(20, 30, 40);
    lowerArm();
  }
  
  if(input == "toggleHand")
  {
    blinkLights(30, 40, 20);
    //toggleHand();
    openHand();
    lowerArm();
    closeHand();
    raiseArm();
  }
  
  /* //this works but we can only receive/process so much data at once over the port *
  if(input.startsWith("attention="))
  {
    String withoutPrefix = input.substring(10);
    int attentionValue = withoutPrefix.toInt();
    led.setColorAt(0, attentionValue, 0, 0);
    led.show();
  }

  if(input.startsWith("meditation="))
  {
    String withoutPrefix = input.substring(11);
    int meditationValue = withoutPrefix.toInt();
    led.setColorAt(1, 0, 0, meditationValue);
    led.show();
  }
  /**/
  
  /* // this was testing with *
  if(input == "red") 
  {
    setAllLights(20, 0, 0);
    Motor1.run(50);
    Motor2.run(50);
    return;
  }
  
  if(input == "green")
  {
    setAllLights(0, 20, 0);
    //HandAngle.run(-100);
    delay(1000);
    //HandAngle.stop();
    toggleHand();
    return;
  }
  
  if(input == "blue")
  {
    setAllLights(0, 0, 20);
    Motor1.run(50);
    Motor2.run(0);
    return;
  }
  
  if(input == "purple")
  {
    setAllLights(20, 20, 20);
    Motor1.run(50);
    Motor2.run(20);
    return;
  }
  
  if(input == "teal")
  {
    setAllLights(0, 20, 20);
    Motor1.stop();
    Motor2.stop();
    return;
  }
  
  if(input == "yellow")
  {
    setAllLights(20, 20, 0);
    return;
  }
  
  if(input == "white")
  {
    setAllLights(10, 10, 10);
    Motor1.stop();
    Motor2.stop();
    return;
  } else {
    setAllLights(0, 0, 0);
    Motor1.stop();
    Motor2.stop();
  }
  /**/
}

/*
void checkTemp(int interval)
{
  //write temperature about every 2 seconds
  tempTimer = millis();
  tempDelta = tempTimer-lastTempTimer;
  if(tempDelta >= interval) {
    String prefix = "temp=";
    int sensorValue = myTemp.temperature();
    Serial.println(prefix + sensorValue);
    lastTempTimer = tempTimer;
  }
}
*/

void checkDistance(int interval)
{
  distanceTimer = millis();
  distanceDelta = distanceTimer-lastDistanceTimer;
  if(distanceDelta >= interval) {
    String prefix = "distance=";
    int sensorValue = ultraSensor.distanceCm();
    Serial.println(prefix + sensorValue);
    lastDistanceTimer = distanceTimer;
    distanceTooShort = sensorValue > 0 && sensorValue <= 20;
  }
}

void setAllLights(int r, int g, int b)
{
    led.setColorAt(0, r, g, b);
    led.setColorAt(1, r, g, b);
    led.setColorAt(2, r, g, b);
    led.setColorAt(3, r, g, b);
    led.show(); 
}

void blinkLights(int r, int g, int b)
{
  setAllLights(r, g, b);
  delay(400);
  setAllLights(0, 0, 0);
  delay(400);
  setAllLights(r, g, b);
  delay(400);
  setAllLights(0, 0, 0);
  delay(400);
  setAllLights(r, g, b);
  delay(400);
  setAllLights(0, 0, 0);
}

void driveForward()
{
    Motor1.run(200);
    Motor2.run(200);
}

void driveBackward()
{
    Motor1.run(-200);
    Motor2.run(-200);
}

void circle()
{
    Motor1.run(150);
    Motor2.run(250);
}

void turnLeft()
{
    Motor2.stop();
    Motor1.run(200);
    delay(2000);
    Motor1.stop();
}

void turnRight()
{
    Motor1.stop();
    Motor2.run(200);
    delay(2000);
    Motor2.stop();
}

void stopRobot()
{
  Motor1.stop();
  Motor2.stop();
  Hand.stop();
  Arm.stop();
}

void raiseArm() 
{
  Arm.run(-200);
  delay(2200);
  Arm.stop();
}

void lowerArm()
{
  Arm.run(200);
  delay(2000);
  Arm.stop();
}

void openHand()
{
  Hand.run(-handSpeed);
  delay(2000);
  Hand.stop();
}

void closeHand()
{
  Hand.run(handSpeed);
  delay(2000);
  Hand.stop();
}

void toggleHand()
{
  if(handPosition) 
  {
    Hand.run(-handSpeed);
    delay(2000);
    Hand.stop();
    handPosition = !handPosition;
  } else {
    Hand.run(handSpeed);
    delay(2000);
    Hand.stop();
    handPosition = !handPosition;
  }
}
