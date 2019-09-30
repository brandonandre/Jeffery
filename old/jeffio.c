
/*
*    File name: jeffio.c
*    Purpose:   contains the function definitions of the JeffIO API
*    Date:      04/02/19
*    Authors:
*               Brandon Andre
*               Chris Ikongo
*               Crystal Luangpakham
*               Justin Angus
*               Marissa Law
*               Michael McRae
*    Functions:  <list of functions>
*    https://gitlab.com/6bits/wro-robot/wikis/home#jeffio-library
*/
#include "jeffio.h"

/*
*    Purpose: reset and initialize
*    Date: February 4th, 2019
*    Authors: Chris Ikongo
*    Parameters: None
*    Return value: void
*    Algorithm: None
*/
void begin()
{
    // Setup onboard light
    pinMode(ONBOARD_LIGHT, OUTPUT);
    digitalWrite(ONBOARD_LIGHT, HIGH);

    // Set up Serial Communications reciever
    Serial.begin(9600);

    Wire.begin();

    Wire.beginTransmission(DC_ADDRESS);
    Wire.write(RESET_CONTROLLERS);
    Wire.endTransmission();
    delay(10);
    Wire.beginTransmission(SERVO_ADDRESS);
    Wire.write(RESET_CONTROLLERS);
    Wire.endTransmission();
    delay(10);

    delay(3000);

    Wire.beginTransmission(DC_ADDRESS);
    Wire.write(ENABLE_CONTROLLERS);
    Wire.endTransmission();
    delay(10);
    Wire.beginTransmission(SERVO_ADDRESS);
    Wire.write(ENABLE_CONTROLLERS);
    Wire.endTransmission();
    delay(10);

    // Recalibrate Servo Motors
    moveHead(100);

    // Recalibrate clamp
    unclamp();

    digitalWrite(ONBOARD_LIGHT, LOW);
}

/*
*    Purpose: To move Jeffery a specific amount of milimeters
*    Date: February 4th, 2019
*    Authors: Justin Angus
*    Parameters: int direction, int milimeters
*    Return value: void
*    Algorithm: The circumference of the drive wheels is a foot so to move the robot 
*    a specific amount of milimeters the math is (milimeters * 360).
*/
void moveDistance(int address, int motorSpeed, int direction, long millimeters)
{
    int lobyte1;
    int hibyte1;
    int lobyte2;
    int hibyte2;
    long motorOneDegrees;
    long motorTwoDegrees;

    /*If direction is true move forwards, if not move backwards*/
    if (direction)
    {
        motorOneDegrees = (millimeters/WHEEL_CIRC * 360);      //forward
        motorTwoDegrees = (millimeters/WHEEL_CIRC * 360) * -1; //backwards
    }
    else
    {
        motorOneDegrees = (millimeters/WHEEL_CIRC * 360) * -1; //backwards
        motorTwoDegrees = (millimeters/WHEEL_CIRC * 360);      //forwards
    }

    lobyte1 = lowByte(motorSpeed);
    hibyte1 = highByte(motorSpeed);

    lobyte2 = lowByte(motorSpeed);
    hibyte2 = highByte(motorSpeed);

    byte four1 = (motorOneDegrees);
    byte three1 = (motorOneDegrees >> 8);
    byte two1 = (motorOneDegrees >> 16);
    byte one1 = (motorOneDegrees >> 24);

    byte four2 = (motorTwoDegrees);
    byte three2 = (motorTwoDegrees >> 8);
    byte two2 = (motorTwoDegrees >> 16);
    byte one2 = (motorTwoDegrees >> 24);

    Wire.beginTransmission(address);
    Wire.write(MOVE_MOTORS_DEGREES);
    Wire.write(hibyte1);
    Wire.write(lobyte1);
    Wire.write(one1);
    Wire.write(two1);
    Wire.write(three1);
    Wire.write(four1);
    Wire.write(hibyte2);
    Wire.write(lobyte2);
    Wire.write(one2);
    Wire.write(two2);
    Wire.write(three2);
    Wire.write(four2);
    Wire.endTransmission();

    delay((millimeters / 50) * 1000);

    Wire.beginTransmission(address);
    Wire.write(RESET_ENCODERS);
    Wire.endTransmission();
    delay(10);  // make sure the encoders have time to reset
}

/*
*    Purpose:       Move a given direction for a given number of milliseconds
*    Date:          08/02/2019
*    Authors:       Michael McRae
*    Parameters:    Int representing milliseconds, Int representing direction (1 FORWARDS, 2 BACKWARDS)
*    Return value:  none
*/
void moveSeconds(int seconds, int direction) 
{
    /*Do math for the delay in mili's*/
    int run = seconds * 1000;

    switch (direction)
    {
    case FORWARDS:
        Wire.beginTransmission(DC_ADDRESS); //address used for the
        Wire.write(MOTORS_POWER);
        Wire.write(SPEED / 4);
        Wire.write(-SPEED / 4);
        Wire.endTransmission();
        break;
    case BACKWARDS:
        Wire.beginTransmission(DC_ADDRESS); //address used for the
        Wire.write(MOTORS_POWER);
        Wire.write(-SPEED / 4);
        Wire.write(SPEED / 4);
        Wire.endTransmission();
        break;
    }

    delay(run);

    Wire.beginTransmission(DC_ADDRESS); //address used for the
    Wire.write(MOTORS_POWER);
    Wire.write(0);
    Wire.write(0);
    Wire.endTransmission();
}

/*
*    Purpose:       Reset the head position to the top of the robot belt. This will recalibrate it.
*    Date:          05/02/2019
*    Authors:       Brandon Andre
*    Parameters:    None
*    Return value:  None
*/
void resetHead()
{
    // Begin transmission to servo motor controller.
    Wire.beginTransmission(SERVO_ADDRESS);
    Wire.write(HEAD_SERVO);
    Wire.write(COUNTER_CLOCKWISE);
    Wire.endTransmission();

    // Wait 5 seconds to reset the head to the top
    delay(3000);

    // Stop moving the servo motor.
    Wire.beginTransmission(SERVO_ADDRESS);
    Wire.write(HEAD_SERVO);
    Wire.write(SERVO_STOP);
    Wire.endTransmission();

    EEPROM.write(HEAD_POSITION_ADDRESS, 100);
}

/*
*    Purpose:       Move the robot head to a specific position on the belt.
*    Date:          05/02/2019
*    Authors:       Brandon Andre
*    Parameters:    position
                        The specific location to move the robot head. Please note this is an estimate.
                        0 being the bottom of the robot belt, 100 being the top of the robot belt.
                        You can also use the constants, HEAD_TOP and HEAD_BOTTOM.
*    Return value:  None
*/
void moveHead(int position) 
{
    int headPosition = EEPROM.read(HEAD_POSITION_ADDRESS);

    // Don't do anything if the current position is where it needs to be.
    if (position == headPosition)
        return;

    // Check if the position matches the limit constrains.
    if (position > 100 || position < 0)
    {
        // Reset the head location...
        EEPROM.write(HEAD_POSITION_ADDRESS, 0);
        return;
    }

    // Determine the direction to move the servo.
    Wire.beginTransmission(SERVO_ADDRESS);
    Wire.write(HEAD_SERVO);
    // current: 0, dersired: 100
    if (position < headPosition)
    {
        Wire.write(CLOCKWISE);
    }
    else
    {
        Wire.write(COUNTER_CLOCKWISE);
    }
    Wire.endTransmission();

    // Determine how long to move the servo so it reaches the dersired location.
    delay(30 * abs(headPosition - position));

    EEPROM.write(HEAD_POSITION_ADDRESS, position);

    // Stop the servo motor from spinning.
    Wire.beginTransmission(SERVO_ADDRESS);
    Wire.write(HEAD_SERVO);
    Wire.write(SERVO_STOP);
    Wire.endTransmission();
}

/*
*    Purpose:       Retrieve the head position from the robot.
*    Date:          05/02/2019
*    Authors:       Brandon Andre
*    Parameters:    None
*    Return value:  Int, range from 0 to 100. 100 Being the very top of the robot, 0 being the bottom.
*/
int getCurrentHeadPosition() 
{
    return EEPROM.read(HEAD_POSITION_ADDRESS);
}

/*
*    Purpose:       Move the clamp servo to hold on to an object.
*    Date:          19/03/2019
*    Authors:       Brandon Andre
*    Parameters:    None
*    Return value:  None
*/
void clamp() {
    Wire.beginTransmission(SERVO_ADDRESS);
    Wire.write(DEATH_HAMMER);
    Wire.write(HAMMER_DOWN);
    Wire.endTransmission();

    delay(500);

    Wire.beginTransmission(SERVO_ADDRESS);
    Wire.write(CLAW_SERVO);
    Wire.write(CLAW_CLOSED);
    Wire.endTransmission();

    delay(500);

    Wire.beginTransmission(SERVO_ADDRESS);
    Wire.write(DEATH_HAMMER);
    Wire.write(HAMMER_UP);
    Wire.endTransmission();
}

/*
*    Purpose:       Move the clamp servo to let go of an object.
*    Date:          19/03/2019
*    Authors:       Brandon Andre
*    Parameters:    None
*    Return value:  None
*/
void unclamp() {
    Wire.beginTransmission(SERVO_ADDRESS);
    Wire.write(CLAW_SERVO);
    Wire.write(CLAW_OPEN);
    Wire.endTransmission();
}


/*
*    Purpose: Allows both motors to turn
*    Date: February 11th, 2018
*    Authors: Crystal Luangpakham
*    Parameters: degrees(long) a negative number will cause it to turn to the left
                 and a positive number to the right
*    Return value: N/A
*    Algorithm: 2 * (pi) * (radius) for the radius and simplified circle math.
*/
void TightTurn(long degrees) 
{
    // simplified math for distance each wheel should turn for specified degrees
    long wheelTurn = (PI * JEFF_WIDTH * -degrees) / WHEEL_CIRC;

    // set the speed of the motors
    int hiBSpd1 = highByte(100);
    int loBSpd1 = lowByte(100);
    int hiBSpd2 = highByte(100);
    int loBSpd2 = lowByte(100);

    // convert wheel turning to bytes
    byte four1 = (wheelTurn);
    byte three1 = (wheelTurn >> 8);
    byte two1 = (wheelTurn >> 16);
    byte one1 = (wheelTurn >> 24);

    Wire.beginTransmission(DC_ADDRESS);
    Wire.write(MOVE_MOTORS_DEGREES);

    // configure left motor speed and degrees
    Wire.write(hiBSpd1);
    Wire.write(loBSpd1);
    Wire.write(one1);
    Wire.write(two1);
    Wire.write(three1);
    Wire.write(four1);

    // configure right motor speed and degrees
    Wire.write(hiBSpd2);
    Wire.write(loBSpd2);
    Wire.write(one1);
    Wire.write(two1);
    Wire.write(three1);
    Wire.write(four1);
    Wire.endTransmission();

    // dps of 100 estimated higher
    delay(abs(wheelTurn * 15));

    Wire.beginTransmission(DC_ADDRESS);
    Wire.write(RESET_ENCODERS);
    Wire.endTransmission();
    delay(10);  // make sure the encoders have time to reset
}

/*
*    Purpose: To rotate Jeffrey by degreesTurned using the left wheel
*    Date: February 10th, 2019
*    Authors: Marissa Law
*    Parameters: int degreesTurned (the target degrees that Jeffrey should turn, can be positive or negative)
*    Return value: void
*    Algorithm: The distance travelled by the outer wheel is calculated (circular path of motion),
*               then the corresponding degrees the motor should be turned is calculated
*               (see wiki for more information)
*/
void wideTurnLeftWheel(int degreesTurned) 
{
    int lobyte; //lower byte of motor speed
    int hibyte; //upper byte of motor speed
    int turnDistance; //the distance the robot travels on a circular path
    long motorTurnDeg; //the degrees the motor has to turn (360 is 1 full rotation)

    turnDistance = degreesTurned*PI*JEFF_WIDTH/180; //distance travelled by outer wheel
    motorTurnDeg = turnDistance*360/WHEEL_CIRC; //the amount of degrees the motor must turn
    //the motor speed is broken up into two bytes
    lobyte  = lowByte(SPEED);
    hibyte  = highByte(SPEED);

    //the degrees the motor must turn is broken up into four bytes
    byte four  = (motorTurnDeg);
    byte three = (motorTurnDeg>>8);
    byte two   = (motorTurnDeg>>16);
    byte one   = (motorTurnDeg>>24);

    Wire.beginTransmission(DC_ADDRESS); //connect to DC motors
    Wire.write(MOVE_LEFT_MOTOR_DEGREES); //command to move left wheel
    Wire.write(hibyte); //the first two bytes written are the motor speed (in degrees per second)
    Wire.write(lobyte);
    Wire.write(one); //the next 4 bytes written are the target degrees the motor should turn
    Wire.write(two);
    Wire.write(three);
    Wire.write(four);
    Wire.endTransmission();
    delay(10);
}

/*
*    Purpose: To rotate Jeffrey by degreesTurned using the right wheel
*    Date: February 10th, 2019
*    Authors: Marissa Law
*    Parameters: int degreesTurned (the target degrees that Jeffrey should turn, can be postive or negative)
*    Return value: void
*    Algorithm: The distance travelled by the outer wheel is calculated (circular path of motion),
*               then the corresponding degrees the motor should be turned is calculated
*               (see wiki for more information)
*/
void wideTurnRightWheel(int degreesTurned) 
{
    int lobyte; //lower byte of motor speed
    int hibyte; //upper byte of motor speed
    int turnDistance; //the distance the robot travels on a circular path
    long motorTurnDeg; //the degrees the motor has to turn (360 is 1 full rotation)

    turnDistance = degreesTurned*PI*JEFF_WIDTH/180; //distance travelled by outer wheel
    motorTurnDeg = turnDistance*360/WHEEL_CIRC; //the amount of degrees the motor must turn

    //the motor speed is broken up into two bytes
    lobyte  = lowByte(SPEED);
    hibyte  = highByte(SPEED);

    //the degrees the motor must turn is broken up into four bytes
    byte four  = (motorTurnDeg);
    byte three = (motorTurnDeg>>8);
    byte two   = (motorTurnDeg>>16);
    byte one   = (motorTurnDeg>>24);

    Wire.beginTransmission(DC_ADDRESS); //connect to DC motors
    Wire.write(MOVE_RIGHT_MOTOR_DEGREES); //command to set right wheel encoder by degrees
    Wire.write(hibyte); //the first two bytes written are the motor speed (in degrees per second)
    Wire.write(lobyte);
    Wire.write(one); //the next 4 bytes written are the target degrees the motor should turn
    Wire.write(two);
    Wire.write(three);
    Wire.write(four);
    Wire.endTransmission();
    delay(10);
}

/*
*    Purpose: To get Parameters from the Raspberry Pi.
*    Date: February 27th, 2019
*    Authors: Brandon Andre
*    Parameters: None
*    Return value: The int found in the Serial buffer.
*/
int getNumberParam() 
{
  delay(500);
  // Check if the serial has another command waiting.
  if (Serial.available()) {
    // Return the number back.
    String param = Serial.readStringUntil(';');
    Serial.println(param);
    return param.toInt();
  } else {
    // Error!
    Serial.println("Error - Could not find the param in the Serial buffer.");
    return -1;
  }
}

/*
*    Purpose: Keeps checking if the mototrs are in use before ending the loop and resetting the encoders.
*    Date: February 23th, 2018
*    Authors: Crystal Luangpakham
*    Parameters: N/A
*    Return value: N/A
*/
void wait() 
{   
    int leftBusy;
    int rightBusy;

    // loop until both motors are done their previous task.
    do {
        // Get busy status from left motor
        Wire.beginTransmission(DC_ADDRESS);
        Wire.write(LEFT_MOTOR_BUSY);
        Wire.endTransmission();
        delay(10);
        
        // Request bytes from device, 1 if motor is busy, 0 if done
        Wire.requestFrom(LEFT_MOTOR_BUSY, 1);
        leftBusy = Wire.read();

        // Get busy status from right motor
        Wire.beginTransmission(DC_ADDRESS);
        Wire.write(LEFT_MOTOR_BUSY);
        Wire.endTransmission();
        delay(10);

        // Request bytes from device
        Wire.requestFrom(LEFT_MOTOR_BUSY, 1);
        rightBusy = Wire.read();

    } while (leftBusy || rightBusy);

    // Previous task might've used the encoders, so reset them
    Wire.beginTransmission(DC_ADDRESS);
    Wire.write(RESET_ENCODERS);
    Wire.endTransmission();
}

/*
*    Purpose: To move Jeffery from (x1, y1) to (x2, y2)
*    Date: February 27th, 2019
*    Authors: Marissa Law
*    Parameters: int x1 (A-U, see jeffio.h for macro definitions)
*                int y1 (A-U, see jeffio.h for macro definitions)
*                int x2 (A-U, see jeffio.h for macro definitions)
*                int y2 (A-U, see jeffio.h for macro definitions)
*                float currentAngle (the angle the robot is turned from facing up - measured from y-axis)
*    Return value: float beta (the angle the robot has to turn)
*    Algorithm: The distance from the initial point and target point is calculated,
*               then the angle the robot must rotate is calculated, then tightTurn()
*               and moveDistance() are called
*               Note: negative xDistance indicates right-to-left path of motion
*                     negative yDistance indicates bottom-to-top path of motion
*/
float moveToCoordinates(int x1, int y1, int x2, int y2, float currentAngle){
    float xDistance; /* the horizontal distance the robot must move from point to point */
    float yDistance; /* the vertical distance the robot must move from point to point */
    float distance; /* the final distance the robot must move from point to point */
    float alpha; /* angle of path of motion (measured from y-axis) */
    float beta; /* the angle the robot must turn */

    xDistance = x2 - x1;
    yDistance = y2 - y1;

    /* these cases handle motion parallel or perpendicular to the y plane */
    if(xDistance == 0 && yDistance == 0)
        alpha = 0;
    else if(xDistance > 0 && yDistance == 0)
        //alpha = 90;
        alpha = 0;
    else if(xDistance < 0 && yDistance == 0)
        //alpha = -90;
        alpha = 180;
    else if(xDistance == 0 && yDistance > 0)
        //alpha = 180;
        alpha = 90;
    else if(xDistance == 0 && yDistance < 0)
        //alpha = 0;
        alpha = -90;
   else
        /*
            take inverse tangent of inverse of slope (run over rise).
            this will be the angle measured from the y-axis to the path of motion.
            handles all other cases.
        */
        alpha = /* 180 - */(atan(yDistance/xDistance) * 180.0 / PI);

    beta = alpha - currentAngle;
    //might want to make this more effecient, ie. if angle > 180 turn in other direction at smaller angle
    //should maybe sanitize distance and beta before calling TightTurn

  /* if(beta > 180)
        beta = 180 - beta; */
   // if(xDistance == 0 || yDistance == 0)
        distance = sqrt(xDistance * xDistance + yDistance * yDistance) - 40*(beta/90) - 400*(beta/180);
   // else
   //     distance = sqrt(xDistance * xDistance + yDistance * yDistance) - 40*(beta/90) - 400*(beta/180) - 100*(yDistance/xDistance);
    
    //float finalDistance = sqrt(distance * distance + (0.5*JEFF_WIDTH + 90) * (0.5*JEFF_WIDTH + 90) - 2 * distance * (0.5*JEFF_WIDTH + 90) * cos(beta));

    /* move robot */
    
    if(beta > 0)
        moveDistance(DC_ADDRESS, SPEED, FORWARDS, 0.5*JEFF_WIDTH + 90); // adjusts movement to account for turn, extra length is 40 mm for now
    else
        moveDistance(DC_ADDRESS, SPEED, BACKWARDS, 0.5*JEFF_WIDTH + 90);
    
    TightTurn(beta);
    moveDistance(DC_ADDRESS, SPEED, FORWARDS, distance);

    Serial.print("distance in function: ");
    Serial.println(distance);


    Serial.print("angle in function: ");
    Serial.println(beta);
    
    return beta;
}