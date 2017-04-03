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


int ball_loc = 0;  

//Initial servo positioning
Servo pan_servo, tilt_servo, grip_servo;
int pan_init = 90, tilt_init = 62, grip_init = 0;

//Servo positioning of basket
int goal_height = 110, grip_pos = 0; 

int irSearch = 0;

//IR
#define IRCOMM 11
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

//Bluetooth
byte a;

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
  //Serial.begin(9600); 

  //Bluetooth setup;
  Serial.begin(115200);
}

void loop() {  
  ball_loc = -1;

  Pivot(RIGHT, 4800);
  
  //Bluetooth loop
  while (ball_loc == -1){
  if(Serial.available()){
  ball_loc = Serial.read();
  Serial.print(ball_loc);
  }
  delay(50);
  }

  int rightVal, leftVal;
  /*
  //IR
  ball_loc = 0;
  irSearch = 0;
  //Continuously reads IR values until the value is not 0,-1, or -2
  while (ball_loc == 0 || ball_loc == -1 || ball_loc == -2){
    if (irSearch == 210){
      irSearch = 0;
      delay(500);
    }
    
    pan_servo.write(irSearch);
    
    ball_loc = IRserial.receive(200); //Reads the ball's location
    Serial.println(ball_loc);
    delay(300);
    irSearch += 30;
  }
  */
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
        Pivot(RIGHT,7);
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
        Pivot(LEFT,15);
        leftVal = analogRead(LTL);
        delay(1);     
      }
      delay(500);
  }  
  
  

  
  //IR Range
  //Moves forward in ball location along the black line until it gets stopped by IR range indicator
  //int range = analogRead(IRANGE);
  //Serial.print("Range Reading => ");
  //Serial.println(range);
  delay(250);


  //Stops the robot when it is near the end of the play fielD
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
        Pivot(RIGHT,20);
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
        Pivot(LEFT,7);
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

//Read the following code carefully 
//Very important to your final project
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
   analogWrite(E2,130);  //Please to use the lowest speed that can make the robot move!!!
   }
   
   //Veering right, move power to right motorp;3 
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
   
    else if (leftVal<LTHRES && centreVal<CTHRES && rightVal<RTHRES){ //for other cases, stop driving first.
      analogWrite(E1,130);
      analogWrite(E2,160);
    }
    else if (leftVal>LTHRES && centreVal>CTHRES && rightVal>RTHRES){  
      Backup();
      delay(20);
      Stop();
      return false;
    }
    return true;
}









