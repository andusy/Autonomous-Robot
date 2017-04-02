//Include Libraries
#include <Servo.h>
#include <QSerial.h>

//Motor Pins
#define M1 7
#define E1 6
#define E2 5
#define M2 4

//Analog Pins
#define LTL A1
#define LTC A2
#define LTR A0

//Linetracking Threshold
#define LTHRES 900
#define CTHRES 900
#define RTHRES 900

//Servo
#define PAN 8
#define TILT 9
#define GRIP 10

//Initial servo positioning
Servo pan_servo, tilt_servo, grip_servo;
int pan_init = 90, tilt_init = 90, grip_init = 40;

//Servo positioning of basket
goal_height = 115, grip_pos = 40; 

//IR
#define IRCOMM 9
QSerial IRserial;

//IR Range Sensor
#define IRANGE A5

//Constant
#define DRIVESPEED 500
#define PIVOT45 200
#define PIVOT180 500
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
 /* 
  //Set default servo positions for each iteration which is the height of the ball's pickup location
  pan_servo.write(pan_init);
  tilt_servo.write(tilt_init);
  grip_servo.write(grip_init);

/*IR
  char ball_loc = '-1';
  
  //Continuously reads IR values until the value is not -1
  while (ball_loc == '-1'){
    ball_loc = IRserial.receive(200); //Reads the ball's location
    Serial.println(ball_loc);
    delay(200);
  }

  //Turns towards the ball's location
  if (ball_loc == '2'){ //Right location
    pivot(RIGHT,90);
  } else if (ball_loc == '0'){ //Left location
    pivot(LEFT,90);
  } 
  
  //IR Range
  //Moves forward in ball location along the black line until it gets stopped by IR range indicator
  int range = analogRead(IRANGE);
  Serial.print("Range Reading => ");
  Serial.println(range);
  delay(250);
  Forward();

  //Stops the robot when it is near the end of the play field
  if(range > 500)
  {
     Stop(); 
     delay(1000);
  }

  //Grips the ball and changes the tilt to be the goal
  grip_servo.write(150);
  tilt_servo.write(goal_height);

  //Rotates 180 
  pivot(RIGHT,180);

  //Follows the line back until all three sensors are on black

  //Turns in direction of basket
  if (ball_loc == '2'){ //Right location
    pivot(LEFT,90);
  } else if (ball_loc == '0'){ //Left location
    pivot(RIGHT,90);
  } 

  //Follows the line to the basket

  //Opens the grip/drops ball into basket
  grip_servo.write(40);

  //Rotates 180
  pivot(RIGHT, 180);

  //Follows the line back to original position

   */
 
  //Read linetracker sensor values
  int leftVal = analogRead(LTL);
  int centreVal = analogRead(LTC);
  int rightVal = analogRead(LTR);

   Serial.print("Left: ");
   Serial.print(leftVal);
   Serial.print("  ");
   Serial.print("Center: ");
   Serial.print(centreVal);
   Serial.print("  ");
   Serial.print("Right: ");
   Serial.print(rightVal);
   Serial.print("  \n");

   Pivot(LEFT,50);
   Pivot(RIGHT,50);
   
}

void Forward() {
  //Motors to drive forward
  digitalWrite(M1,HIGH);   
  digitalWrite(M2,HIGH);  //I make it spin in case you copy my code!
  
  //Motor to drive a speed xxx
  analogWrite(E1,DRIVESPEED);
  analogWrite(E2,DRIVESPEED); 
  
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
      analogWrite(E1,DRIVESPEED);
      analogWrite(E2,DRIVESPEED);
    }
    else if( whichWay == RIGHT )
    {//Pivot RIGHT
      //Motors to drive forward
      digitalWrite(M1,LOW);   
      digitalWrite(M2,HIGH);
  
     //Motor to drive a speed xxx
     analogWrite(E1,DRIVESPEED);
     analogWrite(E2,DRIVESPEED);  
    }  
    
    delay(angle);

    analogWrite(E1, 0);
    analogWrite(E2, 0);

    return;    
}

