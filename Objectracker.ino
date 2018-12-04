#include <Wire.h>
#include <QMC5883L.h>

QMC5883L compass;



//int pwm_a = 10; //PWM control for motor outputs 1 and 2 is on digital pin 10
int l1=A3;
int l2=A2;

int r1=A1;
int r2=A0;

int enl=11;
int enr=10;

int lowspeed = 120;
int highspeed = 140;

//Distance away
int distance;

//Sets the duration each keystroke captures the motors.
int keyDuration = 10;

int iComp;

void setup()
{
  Wire.begin();
  Serial.begin(9600);
  compass.init();
  


  pinMode(l1,OUTPUT);   // Left motor output
  pinMode(l2,OUTPUT);
  
  pinMode(r1,OUTPUT);  //Right motor output
  pinMode(r2,OUTPUT);

  pinMode(enl,OUTPUT);   //Enable pins of R and L motors
  pinMode(enr,OUTPUT);

  analogWrite(enl, 0);
  analogWrite(enr, 0);

}

void loop()
{

  int x,y,z;
  compass.read(&x,&y,&z);

  // Calculate heading when the magnetometer is level, then correct for signs of axis.
  // Atan2() automatically check the correct formula taking care of the quadrant you are in
  float heading = atan2(y, x);

  float declinationAngle = 0.0404;
  heading += declinationAngle;
  // Find yours here: http://www.magnetic-declination.com/

   // Correct for when signs are reversed.
  if(heading < 0)
    heading += 2*PI;

  // Check for wrap due to addition of declination.
  if(heading > 2*PI)
    heading -= 2*PI;

  // Convert radians to degrees for readability.
  float headingDegrees = heading * 180/M_PI; 

  // Normally we would delay the application by 66ms to allow the loop
  // to run at 15Hz (default bandwidth for the HMC5883L).
  // However since we have a long serial out (104ms at 9600) we will let
  // it run at its natural speed.
  // delay(66);

  //This throttles how much data is sent to Python code.  
  //Basically, it updates every second (10 microsecond delay X 100 iComps)
  if (iComp >= 30){

    int adjHeading = 0;
    //The "floor" part makes the float into an integer, rounds it up.
    headingDegrees = floor(headingDegrees);
    if (headingDegrees >= 280){
        adjHeading = map(headingDegrees, 280, 360, 0, 79);
    }
    else if (headingDegrees <= 279) {
        adjHeading = map(headingDegrees, 0, 279, 80, 360);
    }

    Serial.println(adjHeading);
    iComp=0;
  }
  iComp++;

  delay(10); //For serial stability.

 int val = Serial.read() - '0';

  if (val == 1)
  {
    Back();
  }

  else if (val == 2)
  {
    Right();
  }

  else if (val == 3)
  {
    Forward();
  }

  else if (val == 4)
  {
    Left();
  }

  else if (val == 5)
  {
    Stop();
  }

}

void Back(){
//Straight back
      analogWrite(enl, highspeed);
      analogWrite(enr, highspeed);

      digitalWrite(l1,HIGH);
      digitalWrite(l2,LOW);
  
      digitalWrite(r1,HIGH);
      digitalWrite(r2,LOW);

delay(keyDuration);
}

void Left(){
      //Left
      analogWrite(enl,lowspeed);
      analogWrite(enr,lowspeed);

      digitalWrite(l1,HIGH);
      digitalWrite(l2,LOW);
  
      digitalWrite(r1,LOW);
      digitalWrite(r2,HIGH);

delay(keyDuration);
}

void Right(){
      //Right
      analogWrite(enl,lowspeed);
      analogWrite(enr,lowspeed);

      digitalWrite(l1,LOW);
      digitalWrite(l2,HIGH);
  
      digitalWrite(r1,HIGH);
      digitalWrite(r2,LOW);

delay(keyDuration);
}

void Forward(){
  //set both motors to run at 100% duty cycle (fast)

  analogWrite(enl,highspeed);
  analogWrite(enr,highspeed);

  digitalWrite(l1,LOW);
  digitalWrite(l2,HIGH);
  
  digitalWrite(r1,LOW);
  digitalWrite(r2,HIGH);

  delay(keyDuration);
}

void Stop(){
  //set both motors to run at 100% duty cycle (fast)
  analogWrite(enl,0);
  analogWrite(enr,0);

  //Straight forward
  digitalWrite(l1,HIGH);
  digitalWrite(l2,HIGH);
  
  digitalWrite(r1,HIGH);
  digitalWrite(r2,HIGH);

  delay(keyDuration);
}
