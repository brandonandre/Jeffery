#include "jeffio.h"
#include <math.h>

void setup() {
  // Robot startup initalization
  begin();
  unclamp();
  float beta = moveToCoordinates(A, A, A, E, 0);
  moveDistance(DC_ADDRESS, SPEED, FORWARDS, C);
  clamp();
  moveDistance(DC_ADDRESS, SPEED, BACKWARDS, D);
  moveToCoordinates(E, D, A, D, 90);
  moveDistance(DC_ADDRESS, SPEED, FORWARDS, D);
  unclamp();
  delay(500);
  moveDistance(DC_ADDRESS, SPEED, BACKWARDS, C);
  clamp();
  TightTurn(360);
}

void loop() {
  // Check if the serial buffer has instructions awaiting in it.
  
  if (Serial.available()) {

    String commandToParse = Serial.readStringUntil(';');
    Serial.println(commandToParse);

    // Move head command was given...
    if (commandToParse.startsWith("movehead")) {
      // Get the head position param. 
      int headPosition = getNumberParam();
      moveHead(headPosition);
      return;
    }

    // Move distance command was given...
    if (commandToParse.startsWith("movedistance")) {
      // Get the speed param. 
      int speed = getNumberParam();

      // Get direction.
      int directionConverted;
      delay(500);
      if (Serial.available()) {
        String direction = Serial.readStringUntil(';');

        // Parse the direction.
        if (direction.equals("F")) {
          directionConverted = FORWARDS;
        } else if (direction.equals("B")) {
          directionConverted = BACKWARDS;
        } else {
          Serial.println("Error - Unknown direction given. Must be F or B.");
        }

      }

      // Get the distance.
      int distance = getNumberParam();

      // Run the commmand.
      moveDistance(1, speed, directionConverted, distance);
      return;
    }

    // Tight turn command was given...
    if (commandToParse.startsWith("tightturn")) {
      int degrees = getNumberParam();
      TightTurn(degrees);
      wait();
      return;
    }

    // Clamp functionality.
    if (commandToParse.startsWith("clamp")) {
      clamp();
      return;
    }

    // Unclamp functionality.
    if (commandToParse.startsWith("unclamp")) {
      unclamp();
      return;
    }
  }
}