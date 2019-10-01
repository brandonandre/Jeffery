#include "robot.h"

int main(int argc, char const *argv[])
{
    begin();

    moveDistance(100, SPEED, FORWARDS);
    
    close();
    return 0;
}
