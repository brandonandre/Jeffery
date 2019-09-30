/*
*    File name: jeffio.h
*    Purpose:   contains the function and constant declarations of the JeffIO API
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

#ifndef __JEFFIO_H__
#define __JEFFIO_H__

#include <Wire.h>
#include <EEPROM.h>

/* Macros */
#define lowByte(w) ((uint8_t) ((w) & 0xff))
#define highByte(w) ((uint8_t) ((w) >> 8))
#define turnDelayTime(w) (w/360*1000) /* this is the calculates the delay between completing a turn and other movement - w should be degrees turned */

/* Constants */
#define DC_ADDRESS 1                /* I2C address for the DC Motor */
#define SERVO_ADDRESS 2             /* I2C address for the Servo Motor */

#define ONBOARD_LIGHT 13            /* On board light pin. */

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


/* Controller Addresses */
#define RESET_CONTROLLERS 0x27 /* Address to reset controllers */
#define ENABLE_CONTROLLERS 0x25 /* Address to enable controllers */

/* Motor Addresses */
#define MOTORS_POWER 0x42

#define MOVE_LEFT_MOTOR_DEGREES 0x58    /* Command to move left DC motor by degrees */
#define MOVE_RIGHT_MOTOR_DEGREES 0x59    /* Command to move right DC motor by degrees */
#define MOVE_MOTORS_DEGREES 0x5A    /* Command to move both DC motors by degrees */

#define LEFT_MOTOR_BUSY 0x4F    /* Command to check if left DC motor is busy */
#define RIGHT_MOTOR_BUSY 0x50   /* Command to check if right DC motor is busy */

#define RESET_ENCODERS 0x4E    /* Address to reset both encoders */

/* Servo Addresses*/
#define HEAD_SERVO 0x36 /* Address for the head servo 1 */
#define CLAW_SERVO 0x30 /* Address for claw servo 2 */
#define DEATH_HAMMER 0x32 /* Address for the death hammer */

/* Claw Constants */
#define CLAW_OPEN 180
#define CLAW_CLOSED 0

/* Hammer Constants */
#define HAMMER_DOWN 0
#define HAMMER_UP 180

/* Playing field coordinate values (distance from origin, which is top left corner of field) */
#define A 0
#define B 115
#define C 230
#define D 345
#define E 460
#define F 575
#define G 690
#define H 805
#define I 920
#define J 1035
#define K 1150
#define L 1265
#define M 1380
#define N 1495
#define O 1610
#define P 1725
#define Q 1840
#define R 1955
#define S 2070
#define T 2185
#define U 2300

/* Function Prototypes */
void begin();
void moveDistance(int address, int motorSpeed, int direction, long mm);
void moveSeconds(int seconds, int direction);
void moveHead(int position);
void TightTurn(long degrees);
int getCurrentHeadPosition();
void resetHead();
void clamp();
void unclamp();
void wideTurnLeftWheel(int degrees);
void wideTurnRightWheel(int degrees);
int getNumberParam();
void wait();
float moveToCoordinates(int x1, int x2, int y1, int y2, float currentAngle);

#endif /* __JEFFIO_H__ */
