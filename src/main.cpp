#include "robot.h"

int main(int argc, char const *argv[])
{
    begin();

    bool running = true;
    while(running) {
        printf("Jeffery Control Panel\n");
        printf("Enter a command:\n");
        printf("\t1. Move Forwards\n");
        printf("\t2. Move Backwards\n");
        printf("\t3. Tight Turn Left\n");
        printf("\t4. Tight Turn Right\n");
        printf("\t5. Exit\n");

        int response;
        scanf("%d", &response);

        switch (response) {
        case 1:
            printf("MOVE FORWARDS!\n");
            moveDistance(100, SPEED, FORWARDS);
            break;
        case 2:
            printf("MOVE BACKWARDS!\n");
            moveDistance(100, SPEED, BACKWARDS);
            break;
        case 3:
            printf("TIGHT TURN LEFT!\n");
            tightTurn(-90);
            break;
        case 4:
            printf("TIGHT TURN RIGHT!\n");
            tightTurn(90);
            break;
        case 5:
            running = false;
            break;
        
        default:
            printf("\nWrong command...\n");
            break;
        }
    }

    close();
    return 0;
}
