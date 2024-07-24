#include <ezButton.h>

#include <AccelStepper.h>
#include <MultiStepper.h>


const int stepPin = 7;
const int directionPin = 8;

//AccelStepper stepper(AccelStepper::FULL4WIRE, 10, 9, 8, 7);
AccelStepper stepper(AccelStepper::DRIVER, stepPin, directionPin);

const int limitSwitchLeft (A0);
const int limitSwitchRight (A1);
ezButton button(A2);  // create ezButton object that attach to pin 6;

#define DIRECTION_CCW -1
#define DIRECTION_CW   1

#define MAX_POSITION 0x7FFFFFFF // maximum of position we can set (long type)

int direction  = DIRECTION_CCW;
long targetPos = 0;
bool isStopped = true;

bool buttonPressed = false;  // Flag to track button press

void setup() {

  // put your setup code here, to run once
  stepper.setPinsInverted (true,false,false);

  
stepper.setMaxSpeed(0.0);   // set the maximum speed
         stepper.setAcceleration(0.0); // set acceleration
         stepper.setSpeed(0);         // set initial speed
         stepper.setCurrentPosition(0); // set position

 
    Serial.begin(9600);
    Serial.println("Modix Conveyor Program");
    Serial.println("Program Version 1.1.1");
  //assuming motor is already one one junction
  Serial.println("Conveyor Starting. Motor is now at rest");
}

enum states { STATE_IDLE, STATE_MOVING_RIGHT, STATE_MOVING_LEFT,};

int currentState = STATE_IDLE;
int leftlimit = 0; 
int rightlimit = 0; 
void loop() {
  // put your main code here, to run repeatedly:
leftlimit = digitalRead(limitSwitchLeft);
rightlimit = digitalRead(limitSwitchRight);
  button.loop();

  stepper.run(); // MUST be called in loop() function


  switch (currentState) {
    case STATE_IDLE:
      // motor is off, waiting for trigger button to be pressed
      if (button.isPressed())
      {
        Serial.println("button to trigger start motor has been pressed");
        isStopped = false;
       {
         stepper.setMaxSpeed(10000.0);   // set the maximum speed
         stepper.setAcceleration(1000.0); // set acceleration
         stepper.setSpeed(1000);         // set initial speed
         stepper.setCurrentPosition(0); // set position
         targetPos = direction * MAX_POSITION;
         stepper.moveTo(targetPos);
         Serial.println("Moving Left");
         currentState = STATE_MOVING_LEFT;
        }
        }
      break;
case STATE_MOVING_LEFT:
      // moving left so only check left limit switch

      if (leftlimit == HIGH) 
      { 
        Serial.println("Sled has reached operator. Delaying for 1 Second"); 
        delay (1000);     
        Serial.println("Motor has reached left. Returning to Home");
        
    Serial.print(F("The direction -> "));
    if (direction == DIRECTION_CW)
      Serial.println(F("CLOCKWISE"));
    else
      Serial.println(F("ANTI-CLOCKWISE"));
direction *= -1; // change direction
    targetPos = direction * MAX_POSITION;
    stepper.setCurrentPosition(0); // set position
    stepper.moveTo(targetPos);
    currentState = STATE_MOVING_RIGHT;
  }

  // without this part, the move will stop after reaching maximum position
  if (stepper.distanceToGo() == 0) { // if motor moved to the maximum position
    stepper.setCurrentPosition(0);   // reset position to 0
    stepper.moveTo(targetPos);       // move the motor to maximum position again
  }

  
  break;

    case STATE_MOVING_RIGHT:
      // moving right so only check right limit switch

      if (rightlimit == HIGH) 
      {
        Serial.println("Motor reached right. motor stopping");
            stepper.setMaxSpeed(0.0);   // set the maximum speed
         stepper.setAcceleration(0.0); // set acceleration
         stepper.setSpeed(0);         // set initial speed
         stepper.setCurrentPosition(0); // set position
         direction *= -1; // change direction
         isStopped == true;


        currentState = STATE_IDLE;
      }
      break;

    
  }
}