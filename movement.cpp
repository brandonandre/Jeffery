void wait() 
{
    int leftBusy;
    int rightBusy;

    do {
        rightBusy = readMotorBusy(DC_ADDRESS,1);
        leftBusy = readMotorBusy(DC_ADDRESS,2);
        Utils::waitFor(10);
    } while (leftBusy || rightBusy);

    resetEncoders(DC_ADDRESS);
}