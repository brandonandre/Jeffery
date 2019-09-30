import movement
import math

NORTH_ANGLE = 0
EAST_ANGLE = 90
SOUTH_ANGLE = 180
WEST_ANGLE = 270

class point:
    x = 0               # X Coordinate
    y = 0               # Y Coordinate
    distance = 0        # Distance from Jeffery
    angle = 0           # Angle it's facing to grab the block. (0 Being North)

    def __init__(self, x, y):
        self.x = x
        self.y = y

    def setAngle(self, angle):
        self.angle = angle

    def getDistance(self, destinationPoint):
        xDistance = destinationPoint.x - self.x
        yDistance = destinationPoint.y - self.y
        self.distance = sqrt(xDistance * xDistance + yDistance * yDistance)
        return self.distance

class pillarPoints:
    # pillar Points
    points = []

    # Get the best pillar. Index 0 is the best, Index 3 is the worst.
    def getBestPoint():
        # Check if any points have been generated yet.
        if (len(points) == 0):
            print("No pillar points were generated. Can't find best one.")
            return None

        lowestPointIndex = 0

        # Go through each of the points. (Four sides of a cube.)
        for index, point in enumerate(points, start=1):
            if (points[index].distance < points[lowestPointIndex].distance):
                lowestPointIndex = index

        return points[lowestPointIndex]

    # Populate the points with the current position and pillar location. (Top left corner)
    def populatePillarPoints(currentPoint, pillarLocation):
        northPoint = point(pillarLocation.X + 115, pillarLocation.Y - 400)
        northPoint.setAngle(NORTH_ANGLE)
        northPoint.getDistance(currentPoint)

        eastPoint = point(pillarLocation.X + 630, pillarLocation.Y - 115)
        eastPoint.setAngle(EAST_ANGLE)
        eastPoint.getDistance(currentPoint)

        southPoint = point(pillarLocation.X + 115, pillarLocation.Y - 630)
        southPoint.setAngle(SOUTH_ANGLE)
        southPoint.getDistance(currentPoint)

        westPoint = point(pillarLocation.X - 400, pillarLocation.Y - 115)
        westPoint.setAngle(WEST_ANGLE)
        westPoint.getDistance(currentPoint)
        
# Author: Marissa Law
def findAngle(xDistance, yDistance, currentAngle):
    # /* these cases handle motion parallel or perpendicular to the y plane */
    if(xDistance == 0 and yDistance == 0):
        alpha = 0
    elif(xDistance > 0 and yDistance == 0):
        alpha = 90
    elif(xDistance < 0 and yDistance == 0):
        alpha = 270
    elif(xDistance == 0 and yDistance > 0):
        alpha = 180
    elif(xDistance == 0 and yDistance < 0):
        alpha = 0
    else:
        #/*
        #    take inverse tangent of inverse of slope (run over rise).
        #    this will be the angle measured from the y-axis to the path of motion.
        #    handles all other cases.
        #*/
        alpha = atan(xDistance/yDistance) * 180.0 / PI

    beta = alpha - currentAngle

    return beta


# Author: Marissa Law
def moveCoordinates(x1, y1, x2, y2, currentAngle):
    xDistance = x2 - x1
    yDistance = y2 - y1

    beta = findAngle(xDistance, yDistance, currentAngle)
    distance = sqrt(xDistance * xDistance + yDistance * yDistance) - 40*(beta/90) - 400*(beta/180)


    #/* move robot */
    tightTurn(beta)
    moveDistance(100, 1, distance)

    return beta

# Author: Marissa Law
# This function should be called after the QR code is read to determine the initial position of Jeffery (based on the parking garage)
# Jeffery should start facing the QR code
# The first two sets of coordinates are the upper right and lower right corners of the parking garage, respectively
# The next two sets of coordinates are the upper-left and lower-right corners of the first needed box
def leaveParkingGarage(xp1, yp1, xp2, yp2, x2, y2):
    deltaX = xp2 - xp1
    deltaY = yp2 - yp1

    initialAngle = findAngle(deltaX, deltaY)

    jefferyLength = 400 # 400 mm
    robotSpeed = 100
    

    moveDistance(robotSpeed, movement.FORWARDS, jefferyLength) # backup until jeffery is out of garage, lined out with outer exit

    # new coordinates are based on front right wheel, right now we're navigating to hte lower right corner of the box

    moveCoordinates(xp2, yp2, x2, y2, initialAngle)

# Author: Marissa Law
# def checkForObstacles(x1, y1, x2, y2, currentAngle)
#    pivot = 15 # pivot increment (in degrees), must adjust in future
#
#    xDistance = x2 - x1
#    yDistance = y2 - y1
#
#    targetDistance = sqrt(xDistance * xDistance + yDistance * yDistance)
#
#    while(getDistance() - targetDistance <= 30)) # tolerance is 30 mm for now, may adjust in future
#        tightTurn(10)
#        
#
#
#   moveCoordinates(x1, y1, x2, y2, currentAngle)
