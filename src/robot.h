#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <math.h>

#include "MyRio.h"
#include "I2C.h"
#include "Motor_Controller.h"
#include "Utils.h"

using namespace std;

/* Constants */
#define DC_ADDRESS 1                /* I2C address for the DC Motor */
#define SERVO_ADDRESS 2             /* I2C address for the Servo Motor */

#define HEAD_POSITION_ADDRESS 0     /* EEPROM storage that remembers that last location. */ 

#define FORWARDS 0                  /* Move the robot in the forward direction. */
#define BACKWARDS 1                 /* Move the robot in the backward direction. */

#define CLOCKWISE 100               /* Turn the servo in the clockwise direction. */
#define COUNTER_CLOCKWISE -100      /* Turn the servo in the counter clockwise direction. */
#define SERVO_STOP 0                /* Stop turning the servo motor. */

#define SPEED 100                   /* The default speed for all of the motor functions. */
#define HEAD_TOP 100                /* Move the robots head towards the top of the rail. */
#define HEAD_BOTTOM 0               /* Move the robots head towards the bottom of the rail. */

#define JEFF_WIDTH 270             /* Width of robot from wheel to wheel in mm */
#define WHEEL_CIRC 319.19             /* Circumference of each wheel on robot in mm */

/* Servo Addresses*/
#define HEAD_SERVO 0x28 /* Address for the head servo 1 */
#define CLAW_SERVO 0x29 /* Address for claw servo 2 */

/* Claw Constants */
#define CLAW_OPEN 180
#define CLAW_CLOSED 0

/* Function Prototypes */
void begin();
void moveDistance(long mm, int motorSpeed = SPEED, int direction = FORWARDS);
void moveSeconds(int seconds, int direction);
void moveHead(int position);
void tightTurn(long degrees);
int getCurrentHeadPosition();
void resetHead();
void clamp();
void unclamp();
void wait();
void close();

/* Global Variables */
int headPosition = HEAD_TOP;