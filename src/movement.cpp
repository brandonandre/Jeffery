#include "robot.h"

/* Begin Variables */
extern NiFpga_Session myrio_session;
NiFpga_Status status;
Motor_Controller mc;
MyRio_I2c i2c;

/*
*    Purpose: reset and initialize the motor controllers and other starting processes.
*    Date: Septemeber 30, 2019
*    Authors: Michael McRae
*    Parameters: None
*    Return value: void
*    Algorithm: None
*/
void begin()
{
    MyRio_Open();

    Utils::setupI2CB(&myrio_session, &i2c);

    mc = Motor_Controller();
    mc.init(&myrio_session);
    mc.controllerEnable(DC);
    mc.controllerEnable(SERVO);

    mc.resetEncoders(DC_ADDRESS);

    int volt = mc.readBatteryVoltage(1);
    printf("Battery Voltage: %d\n\n", volt);

    if (volt > 3000) {
        printf("Connected to the DC charger!\n");
    }

    // Check if Jeffrey needs to be charged.
    if (volt < 1500) {
        printf("Please charge me!\n");
    }
}

/*
*    Purpose: Reset the controllers for future use. 
*    Date: Septemeber 30, 2019
*    Authors: Michael McRae
*    Parameters: None
*    Return value: void
*    Algorithm: None
*/
void close()
{
    mc.controllerReset(DC);
    mc.controllerReset(SERVO);

    status = MyRio_Close();
}

/*
 *    Purpose: Wait until the motor encoders are finished with a specific task.
 *    Date: Septemeber 30, 2019
 *    Authors: Michael McRae
 *    Parameters: None
 *    Return value: void
 *    Algorithm: None
 */
void wait() 
{
    int leftBusy;
    int rightBusy;

    do {
        rightBusy = mc.readMotorBusy(DC_ADDRESS, 0);
        leftBusy = mc.readMotorBusy(DC_ADDRESS, 1);
	    printf("L:%d R:%d\n",leftBusy, rightBusy);
    } while (leftBusy || rightBusy);

    mc.resetEncoders(DC_ADDRESS);
}

/*
*    Purpose: Move the robot forward or backwards a specific speed and distance.
*    Date: Septemeber 30, 2019
*    Authors: Brandon Andre
*    Parameters: mm (distance to travel)
                 motorSpeed (Default: 100%, the percentage of speed from 0 to 100)
                 direction (Default; FORWARDS, either forward or backwards.)
*    Return value: void
*    Algorithm: None
*/
void moveDistance(long mm, int motorSpeed, int direction) 
{
    long motorOneDegrees;
    long motorTwoDegrees;

    /* If direction is true move forwards, if not move backwards */
    if (direction)
    {
        motorOneDegrees = (mm / WHEEL_CIRC * 360);      //forward
        motorTwoDegrees = (mm / WHEEL_CIRC * 360) * -1; //backwards
    }
    else
    {
        motorOneDegrees = (mm / WHEEL_CIRC * 360) * -1; //backwards
        motorTwoDegrees = (mm / WHEEL_CIRC * 360);      //forwards
    }

    mc.setMotorDegrees(DC_ADDRESS, motorSpeed, motorOneDegrees, motorSpeed, motorTwoDegrees);
    Utils::waitFor(mm / 60);
    Utils::waitFor(1);
    mc.resetEncoders(DC_ADDRESS);
}

/*
*    Purpose:       Move a given direction for a given number of milliseconds
*    Date:          08/02/2019
*    Authors:       Michael McRae / Brandon Andre
*    Parameters:    Int representing milliseconds, Int representing direction 
                    (1 FORWARDS, 2 BACKWARDS)
*    Return value:  none
*/
void moveSeconds(int seconds, int direction)
{
    switch (direction) {
    case FORWARDS:
        mc.setMotorPowers(DC_ADDRESS, SPEED / 4, -SPEED / 4);
        break;
    case BACKWARDS:
        mc.setMotorPowers(DC_ADDRESS, -SPEED / 4, SPEED / 4);
        break;
    default:
        // TODO: Error message here...
        return;
    }

    // Wait for the specified amount of seconds.
    Utils::waitFor(seconds);

    // Stop the motor
    mc.setMotorPowers(DC_ADDRESS, 0, 0);
}

/*
 *    Purpose: Allows both motors to turn
 *    Date: Septemeber 30, 2019
 *    Authors: Chris Ikongo
 *    Parameters: degrees(long) a negative number will cause it to turn to the left
 *                  and a positive number to the right
 *    Return value: N/A
 *    Algorithm: 2 * (pi) * (radius) for the radius and simplified circle math.
 */
void tightTurn(long degrees)
{
    // simplified math for distance each wheel should turn for specified degrees
    long wheelTurn = (M_PI * JEFF_WIDTH * -degrees) / WHEEL_CIRC;

    printf("Wheel turn: %ld\n", wheelTurn);

    mc.setMotorDegrees(DC_ADDRESS, SPEED, wheelTurn, SPEED, wheelTurn);

    // Reset the encoders after the turn has taken place.
    Utils::waitFor(abs(degrees) / 10);
    mc.resetEncoders(DC_ADDRESS);
    Utils::waitFor(1);
}

/*
*    Purpose:       Retrieve the head position from the robot.
*    Date:          05/02/2019
*    Authors:       Brandon Andre
*    Parameters:    None
*    Return value:  Int, range from 0 to 100. 100 Being the very top of the robot, 0 being the bottom.
*/
int getCurrentHeadPosition() {
    return 0; 
}

/*
*    Purpose:       Reset the head position to the top of the robot belt. This will recalibrate it.
*    Date:          05/02/2019
*    Authors:       Brandon Andre
*    Parameters:    None
*    Return value:  None
*/
void resetHead() {
    // Begin transmission to servo motor controller.
    mc.setServoSpeed(SERVO_ADDRESS, HEAD_SERVO, COUNTER_CLOCKWISE);

    // Wait 5 seconds to reset the head to the top
    Utils::waitFor(5);

    // Stop moving the servo motor.
    mc.setServoSpeed(SERVO_ADDRESS, HEAD_SERVO, SERVO_STOP);

    //headPosition = HEAD_TOP;
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
void moveHead(int position) 
{
    // Don't do anything if the current position is where it needs to be.
    //if (position == headPosition)
    //    return;

    // Check if the position matches the limit constrains.
    if (position > 100 || position < 0)
    {
        // Reset the head location...
        resetHead();
        return;
    }

    // Determine the direction to move the servo.
    // current: 0, dersired: 100
    if (position < headPosition)
    {
        mc.setServoSpeed(SERVO_ADDRESS, HEAD_SERVO, CLOCKWISE);
    }
    else
    {
        mc.setServoSpeed(SERVO_ADDRESS, HEAD_SERVO, COUNTER_CLOCKWISE);
    }

    // Determine how long to move the servo so it reaches the dersired location.
    //Utils::waitFor(abs(headPosition - position));

    //headPosition = position;

    // Stop the servo motor from spinning.
    mc.setServoSpeed(SERVO_ADDRESS, HEAD_SERVO, SERVO_STOP);
}
*/

/*
*    Purpose:       Move the clamp servo to hold on to an object.
*    Date:          19/03/2019
*    Authors:       Brandon Andre
*    Parameters:    None
*    Return value:  None
*/
void clamp() {
    mc.setServoSpeed(SERVO_ADDRESS, CLAW_SERVO, CLAW_CLOSED);
}

/*
*    Purpose:       Move the clamp servo to let go of an object.
*    Date:          19/03/2019
*    Authors:       Brandon Andre
*    Parameters:    None
*    Return value:  None
*/
void unclamp() {
    mc.setServoSpeed(SERVO_ADDRESS, CLAW_SERVO, CLAW_OPEN);
}

/**
 * Purpose:         Wag the robots tail.
 * Date:            25/11/2019
 * Authors:         Brandon Andre
 * Parameters:      None
 * Return Value:    None
 */
void wagTail() {
    mc.setServoPosition(SERVO_ADDRESS, 1, 30);
    Utils::waitFor(1);
    mc.setServoPosition(SERVO_ADDRESS, 1, 150);
    Utils::waitFor(1);
    mc.setServoPosition(SERVO_ADDRESS, 1, 30);
    Utils::waitFor(1);
    mc.setServoPosition(SERVO_ADDRESS, 1, 150);
    Utils::waitFor(1);
    mc.setServoSpeed(SERVO_ADDRESS, 1, 0);
}

/**
 * Purpose:         Shake the whole robot very quickly.
 * Date:            25/11/2019
 * Authors:         Brandon Andre
 * Parameters:      None
 * Return Value:    None
 */
void shake() {
    tightTurn(20);
    tightTurn(-40);
    tightTurn(40);
    tightTurn(-20);
}