	/*
	ELEC299 - Winter2017
	
	CodeGroup2.ino
	
	Group Number: 2
	
	Group Members:
	Andus Yu (10192081)
	Darian Lio (10181505)
	Sean Lee (10199658)
	*/

//Include Libraries
#include <Servo.h>
#include <QSerial.h>

//Motor Pins
#define M1 7
#define E1 6
#define E2 5
#define M2 4

//Analog Pins
#define LTL A2
#define LTC A0
#define LTR A1

//Linetracking Threshold
#define LTHRES 900
#define CTHRES 850
#define RTHRES 900

//Servo
#define PAN 8
#define TILT 9
#define GRIP 10

int ball_loc;  

//Initial servo positioning
Servo pan_servo, tilt_servo, grip_servo;
int pan_init = 90, tilt_init = 63, grip_init = 0;

//Servo positioning of basket
int goal_height = 110, grip_pos = 0; 

int irSearch = 0;

//IR
#define IRCOMM 3
QSerial IRserial;

//IR Range Sensor
#define IRANGE A5

//Constant
#define RDRIVESPEED 170
#define LDRIVESPEED 191.5
#define PIVOTRIGHT90 440
#define PIVOTLEFT90 515
#define PIVOT180 1020
#define LEFT 0
#define RIGHT 1

void setup() {

  //Pin modes
  //Motor Pins
  pinMode(E1, OUTPUT);
  pinMode(M1, OUTPUT);
  pinMode(E2, OUTPUT);
  pinMode(M2, OUTPUT);

  //Threshold Pins
  pinMode(LTL, INPUT);
  pinMode(LTC, INPUT);
  pinMode(LTR, INPUT);

  //IR Range pin
  pinMode(IRANGE,INPUT);  

  //IR
  pinMode(IRCOMM,INPUT);
  IRserial.attach(IRCOMM,-1); 

  //Servo pins
  pan_servo.attach(PAN);
  tilt_servo.attach(TILT);
  grip_servo.attach(GRIP);
  
  //Set default servo positions
  pan_servo.write(pan_init);
  tilt_servo.write(tilt_init);
  grip_servo.write(grip_init);
  
  //Serial setup
  Serial.begin(9600); 

}

void loop() {  
  int rightVal, leftVal;
  
  //IR
  ball_loc = 0;
  irSearch = 0;

  tilt_servo.write(90);
  
  //Continuously reads IR values until the value is not 0,-1, and -2
  while (ball_loc != 48 && ball_loc != 49 && ball_loc != 50){
      ball_loc = IRserial.receive(200); //Reads the ball's location
  }

  //Set default servo positions
  pan_servo.write(pan_init);
  tilt_servo.write(tilt_init);
  grip_servo.write(grip_init);
 
  //Turns towards the ball's location
  if (ball_loc == 48){ //Right location
      Pivot(RIGHT,200);
      delay(50);
      rightVal = analogRead(LTR);
      while (rightVal < RTHRES) {    
        Pivot(RIGHT,4);
        rightVal = analogRead(LTR);
        delay(1);
        
      }
      delay(500);
  } else if (ball_loc == 50){ //Left location
      //Rotates 180 
      Pivot(LEFT,200);
      delay(50);
      leftVal = analogRead(LTL);
      while (leftVal < LTHRES) {    
        Pivot(LEFT,4);
        leftVal = analogRead(LTL);
        delay(1);     
      }
      delay(500);
  }  //No straight location because the robot is facing '1' by default
  
  delay(250);

  //Stops the robot when it is near the end of the play field
  while(LineTracker()){
  }

  Backup();
  delay(50);
  Stop();
  
  //Grips the ball and changes the tilt to be the goal
  grip_servo.write(150);
  delay(1000);
  tilt_servo.write(goal_height);
  delay(500);

  Backup();
  delay(100);
  Stop();
  
  //Rotates 180 
  Pivot(RIGHT,200);
  delay(50);
  rightVal = analogRead(LTR);
  while (rightVal < RTHRES) {    
    Pivot(RIGHT,3);
    rightVal = analogRead(LTR);
    delay(1);
    
  }
  delay(500);
  
  //Follows the line back until all three sensors are on black
  while (LineTracker()){
  }
  //Overshoots the middle line after returning from goal to adjust for rotation
  Forward();
  delay(150);

  //Turns in direction of basket
  if (ball_loc == 50){ //Left location
      Pivot(RIGHT,200);
      delay(50);
      rightVal = analogRead(LTR);
      while (rightVal < RTHRES) {    
        Pivot(RIGHT,4);
        rightVal = analogRead(LTR);
        delay(1);
        
      }
      delay(500);     
    } else if (ball_loc == 48){ //Right location
       //Rotates 180 
      Pivot(LEFT,200);
      delay(50);
      leftVal = analogRead(LTL);
      while (leftVal < LTHRES) {    
        Pivot(LEFT,4);
        leftVal = analogRead(LTL);
        delay(1);     
      }
      delay(500);
  } 

  //Follows the line to the basket
  delay(250);

  //Stops the robot when it is near the end of the play fielD
  while(LineTracker()){
  }
  
  //Opens the grip/drops ball into basket
  grip_servo.write(grip_init);
  delay(1000);

  Backup();
  delay(200);

  //Rotates 180 
  Pivot(RIGHT,200);
  delay(50);
  rightVal = analogRead(LTR);
  while (rightVal < RTHRES) {    
    Pivot(RIGHT,3);
    rightVal = analogRead(LTR);
    delay(1);
    
  }
  delay(500);
  
  //Follows the line back until all three sensors are on black
  while (LineTracker()){
  }
  //Overshoots the middle line after returning from goal to adjust for rotation
  Forward();
  delay(150);

  Stop();
  delay(100);
}

void Forward() {
  //Motors to drive forward
  digitalWrite(M1,HIGH);   
  digitalWrite(M2,HIGH); 
  
  //Motor to drive a speed xxx
  analogWrite(E1,RDRIVESPEED);
  analogWrite(E2,LDRIVESPEED); 
  
  return;
}

void Backup() {
  //Motors to drive forward
  digitalWrite(M1,LOW);   
  digitalWrite(M2,LOW); 
  
  //Motor to drive a speed xxx
  analogWrite(E1,RDRIVESPEED);
  analogWrite(E2,LDRIVESPEED); 
  
  return;
}

void Stop()
{
  //Motor to drive a speed xxx
  analogWrite(E1,0);
  analogWrite(E2,0);
}

//Code to pivot the robot
void Pivot(int whichWay, int angle){ 
    if( whichWay == LEFT )
    {//Pivot LEFT
      //Motors to drive forward
      digitalWrite(M1,HIGH);   
      digitalWrite(M2,LOW);
  
      //Motor to drive a speed xxx
      analogWrite(E1,RDRIVESPEED);
      analogWrite(E2,LDRIVESPEED);
    }
    else if( whichWay == RIGHT )
    {//Pivot RIGHT
      //Motors to drive forward
      digitalWrite(M1,LOW);   
      digitalWrite(M2,HIGH);
  
     //Motor to drive a speed xxx
     analogWrite(E1,RDRIVESPEED);
     analogWrite(E2,LDRIVESPEED);  
    }  
    
    delay(angle);

    analogWrite(E1, 0);
    analogWrite(E2, 0);

    return;   
}

//Function that the robot uses to follow the line
//Returns false if all 3 sensors are completely off of the line
boolean LineTracker(){

  //Motors to drive straight
  digitalWrite(M1,HIGH);
  digitalWrite(M2,HIGH);
 
  //Read linetracker sensor values
  int leftVal = analogRead(LTL);
  int centreVal = analogRead(LTC);
  int rightVal = analogRead(LTR);
  //Optional: reflect the values to the monitor
  Serial.print("Left: ");
  Serial.print(leftVal);
  Serial.print("  ");
  Serial.print("Center: ");
  Serial.print(centreVal);
  Serial.print("  ");
  Serial.print("Right: ");
  Serial.print(rightVal);
  Serial.print("  \n");
  
   if(leftVal<LTHRES && centreVal>CTHRES && rightVal<RTHRES){
   analogWrite(E1,130);
   analogWrite(E2,130);  
   }
   
   //Veering right, move power to right motor
   else if(leftVal>LTHRES && centreVal<CTHRES & rightVal<RTHRES){
   analogWrite(E1,160);
   analogWrite(E2,130);
   }
   
   else if(leftVal>LTHRES && centreVal>CTHRES && rightVal<RTHRES){
   analogWrite(E1,160);
   analogWrite(E2,130);
   }
   
   //Veering left, move power to left motor
   else if(leftVal<LTHRES && centreVal<CTHRES && rightVal>RTHRES){
   analogWrite(E1,130);
   analogWrite(E2,160);
   }
   
   else if(leftVal<LTHRES && centreVal>CTHRES && rightVal>RTHRES){
   analogWrite(E1,130);
   analogWrite(E2,160);
   }
   
    else if (leftVal<LTHRES && centreVal<CTHRES && rightVal<RTHRES){ //Slow down and turn right
      analogWrite(E1,100);
      analogWrite(E2,80);
    }
    else if (leftVal>LTHRES && centreVal>CTHRES && rightVal>RTHRES){  
      Backup();
      delay(20);
      Stop();
      return false;
    }
    return true;
}









