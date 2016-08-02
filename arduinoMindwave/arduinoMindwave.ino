#include "MeOrion.h"

//attachments
MeRGBLed led(PORT_3);
MeDCMotor Motor1(M1);
MeDCMotor Motor2(M2);
MeDCMotor Hand(PORT_1);
MeDCMotor HandAngle(PORT_2);
MeTemperature myTemp(PORT_8, SLOT2);
MeUltrasonicSensor ultraSensor(PORT_7); /* Ultrasonic module can ONLY be connected to port 3, 4, 6, 7, 8 of base shield. */

//DECLARE VARIALBLES

//messages from node
String input;

//robot arm
boolean handPosition = false;
int handSpeed = 200;

//temperature
int tempTimer = 0;
int lastTempTimer = 0;
int tempDelta = 0;

//ultrasonic sensor
int distanceTimer = 0;

int lastDistanceTimer = 0;
int distanceDelta = 0;

void setup()
{
Serial.begin(9600);
Serial.setTimeout(10);
}

void loop() 
{
  //checkTemp(10000);
  checkDistance(1000);
  
  if(Serial.available() > 0)
  {
      input = Serial.readStringUntil('\n');
  }

  //Serial.println(input); //for debugging only
  
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
    HandAngle.run(100);
    delay(1000);
    HandAngle.stop();
    //toggleHand();
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
}

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

void checkDistance(int interval)
{
  distanceTimer = millis();
  distanceDelta = distanceTimer-lastDistanceTimer;
  if(distanceDelta >= interval) {
    String prefix = "distance=";
    int sensorValue = ultraSensor.distanceCm();
    Serial.println(prefix + sensorValue);
    lastDistanceTimer = distanceTimer;
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

void toggleHand()
{
  if(handPosition) 
  {
    Hand.run(-handSpeed);
    HandAngle.run(handSpeed);
    delay(2000);
    Hand.stop();
    HandAngle.stop();
    handPosition = !handPosition;
  } else {
    Hand.run(handSpeed);
    HandAngle.run(-handSpeed);
    delay(2000);
    Hand.stop();
    HandAngle.stop();
    handPosition = !handPosition;
  }
}