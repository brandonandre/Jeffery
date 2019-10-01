#include "robot.h"

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
    status = MyRio_Open();
    if (MyRio_IsNotSuccess(status))
    {
        return status;
    }

    status = Utils::setupI2CB(&myrio_session, &i2c);

    mc = Motor_Controller(&i2c);
    mc.controllerEnable(DC);
    mc.controllerEnable(SERVO);

    int volt = mc.readBatteryVoltage(1);
    printf("%d\n\n", volt);
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

void wait()
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

    do
    {
        rightBusy = readMotorBusy(DC_ADDRESS, 1);
        leftBusy = readMotorBusy(DC_ADDRESS, 2);
    do {
        rightBusy = mc.readMotorBusy(DC_ADDRESS,1);
        leftBusy = mc.readMotorBusy(DC_ADDRESS,2);
        Utils::waitFor(10);
    } while (leftBusy || rightBusy);

    mc.resetEncoders(DC_ADDRESS);
}

void moveDistance(long mm, int motorSpeed = SPEED, int direction = FORWARDS)
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
void moveDistance(long mm, int motorSpeed = SPEED, int direction = FORWARDS) 
{
    long motorOneDegrees;
    long motorTwoDegrees;

    /* If direction is true move forwards, if not move backwards */
    if (direction)
    {
        motorOneDegrees = (millimeters / WHEEL_CIRC * 360);      //forward
        motorTwoDegrees = (millimeters / WHEEL_CIRC * 360) * -1; //backwards
    }
    else
    {
        motorOneDegrees = (millimeters / WHEEL_CIRC * 360) * -1; //backwards
        motorTwoDegrees = (millimeters / WHEEL_CIRC * 360);      //forwards
    }

    mc.setMotorDegrees(DC_ADDRESS, motorSpeed, motorOneDegrees, motorSpeed, motorTwoDegrees);
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
        return void;
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

    mc.setMotorDegrees(DC_ADDRESS, SPEED, wheelTurn, SPEED, wheelTurn);

    // Reset the encoders after the turn has taken place.
    Utils::waitFor(wheelTurn * 15);
    mc.resetEncoders();
}