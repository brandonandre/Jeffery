# import the necessary packages
from logic import moveCoordinates
from imutils.video import VideoStream
from pyzbar import pyzbar
import argparse
import datetime
import imutils
import time
import cv2

"""
Purpose: Runs the game simuation
*    Date: March 1th, 2019
*    Authors: Chris Ikongo
"""
class Coordinates(object):
    X1 = None
    Y1 = None
    X2 = None
    Y2 = None

    def __init__(self, id):
        pass
    def addCoord(self, value):

        if self.X1 is None:
            self.X1 = value

        elif self.Y1 is None:
            self.Y1 = value

        elif self.X2 is None:
            self.X2 = value

        elif self.Y2 is None:
            self.Y2 = value

    def printList(self):
        print(
            "X1 {0} | Y1 {1} | X2 {2} | Y2 {3}".format(self.X1, self.Y1, self.X2, self.Y2))


class BoardGame:

    coordList = []
    qrcode = None
    # Becomes true if the string is parsed
    parsed = False

    ### (K,Q,J,N)(I,I,K,G)(N,C,P,E)(E,O,C,Q)”
    def __init__(self, qrcode):
        BoardGame.qrcode = qrcode
        print('Constructor invoked and this is the code ' + BoardGame.qrcode)
    """
    * Purpose: Reads the qrcode from the webcam
    * Date: April 1st 2019
    * Author: Justin Augus
    *
    """
    def getQrCode(self):
        #use the webcam
        vs = VideoStream(src=0).start()
        time.sleep(2.0)

        #boolean for if a barcode is found
        barcodeFound = None

        # loop over the frames from the video stream
        while True:
            # grab the frame from the threaded video stream and resize it to
            # have a maximum width of 400 pixels
            frame = vs.read()
            frame = imutils.resize(frame, width=600)
        
            # find the barcodes in the frame and decode each of the barcodes
            barcodes = pyzbar.decode(frame)
            
            # loop over the detected barcodes
            for barcode in barcodes:
                #barcode was found
                barcodeFound = True
        
                # the barcode data is a bytes object so if we want to draw it
                # on our output image we need to convert it to a string first
                barcodeData = barcode.data.decode("utf-8")
                
            #if the barcode is found print and break
            if barcodeFound is True:
                print("[BARCODE] " + barcodeData)#barcodeData is the string to pass into Chirs's parsing class
                break
                
        cv2.destroyAllWindows()
        return barcodeData#THIS IS THE QR CODE

    # 
    """
    * Purpose: Function that parses the string
    * Date: March 2nd 2019
    * Author: Chris Ikongo
    *
    """
    def parse(self, qrcode = None):
        box = 0

        if qrcode == None:
            qrcode = BoardGame.qrcode
        # BoardGame.coordList[box] = Coordinates()
        print(qrcode)  # qrcode = "(K,Q,J,N)(I,I,K,G)(N,C,P,E)(E,O,C,Q)"
        for s in qrcode:

            if s == '(':
                i = 1
                BoardGame.coordList.append(Coordinates("ID={0}".format(box)))
                continue

            if s == ')':
                i = 0
                BoardGame.coordList[box].printList()
                box += 1
                continue

            if s.isalpha():
                # Switch case
                if s == 'A':
                    BoardGame.coordList[box].addCoord(0)
                elif s == 'B':
                    BoardGame.coordList[box].addCoord(115)
                elif s == 'C':
                    BoardGame.coordList[box].addCoord(230)
                elif s == 'D':
                    BoardGame.coordList[box].addCoord(345)
                elif s == 'E':
                    BoardGame.coordList[box].addCoord(460)
                elif s == 'F':
                    BoardGame.coordList[box].addCoord(575)
                elif s == 'G':
                    BoardGame.coordList[box].addCoord(690)
                elif s == 'H':
                    BoardGame.coordList[box].addCoord(805)
                elif s == 'I':
                    BoardGame.coordList[box].addCoord(920)
                elif s == 'J':
                    BoardGame.coordList[box].addCoord(1035)
                elif s == 'K':
                    BoardGame.coordList[box].addCoord(1150)
                elif s == 'L':
                    BoardGame.coordList[box].addCoord(1265)
                elif s == 'M':
                    BoardGame.coordList[box].addCoord(1380)
                elif s == 'N':
                    BoardGame.coordList[box].addCoord(1495)
                elif s == 'O':
                    BoardGame.coordList[box].addCoord(1610)
                elif s == 'P':
                    BoardGame.coordList[box].addCoord(1725)
                elif s == 'Q':
                    BoardGame.coordList[box].addCoord(1840)
                elif s == 'R':
                    BoardGame.coordList[box].addCoord(1955)
                elif s == 'S':
                    BoardGame.coordList[box].addCoord(2070)
                elif s == 'T':
                    BoardGame.coordList[box].addCoord(2185)
                elif s == 'U':
                    BoardGame.coordList[box].addCoord(2300)

                #BoardGame.coordList[box].addCoord(s)
        BoardGame.parsed = True


    # Function that returns the object onces it parsed
    def getObject(self):
        return self

p1 = BoardGame("(K,Q,J,N)(I,I,K,G)(N,C,P,E)(E,O,C,Q)")
p1.parse()
print(p1.coordList[0].X2)
moveCoordinates(p1.coordList[0].X1, p1.coordList[0].Y1, p1.coordList[0].X2, p1.coordList[0].Y2)