#include "robot.h"

int main(int argc, char const *argv[])
{
    begin();

    moveDistance(300, SPEED, FORWARDS);
    tightTurn(90);
    close();
    return 0;
}
