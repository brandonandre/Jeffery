#include "robot.h"

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

void close()
{
    mc.controllerReset(DC);
    mc.controllerReset(SERVO);

    status = MyRio_Close();
}

void wait()
{
    int leftBusy;
    int rightBusy;

    do
    {
        rightBusy = readMotorBusy(DC_ADDRESS, 1);
        leftBusy = readMotorBusy(DC_ADDRESS, 2);
        Utils::waitFor(10);
    } while (leftBusy || rightBusy);

    resetEncoders(DC_ADDRESS);
}

void moveDistance(long mm, int motorSpeed = SPEED, int direction = FORWARDS)
{
    long motorOneDegrees;
    long motorTwoDegrees;

    /*If direction is true move forwards, if not move backwards*/
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

void tightTurn(long degrees)
{
    // simplified math for distance each wheel should turn for specified degrees
    long wheelTurn = (M_PI * JEFF_WIDTH * - degrees) / WHEEL_CIRC;

    mc.setMotorDegrees(DC_ADDRESS, motorSpeed, motorOneDegrees, motorSpeed, motorTwoDegrees);
}