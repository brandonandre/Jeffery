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
        printf("\t3. Tight Turn\n");
        printf("\t4. Exit\n");

        int response;
        scanf("%d", &response);

        switch (response) {
        case 1:
            moveDistance(20, SPEED, FORWARDS);
            break;
        case 2:
            moveDistance(20, SPEED, BACKWARDS);
            break;
        case 3:
            break;
        case 4:
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
