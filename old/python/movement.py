# sudo apt-get install python-serial
import serial
import RPi.GPIO as GPIO
import time


FORWARDS = "F"
BACKWARDS = "B"

port = serial.Serial("/dev/ttyAMA0",
                    baudrate=9600)

port.close()
port.isOpen()
port.open()

def moveHead(headPosition):
    port.write("mh;".encode())
    port.write((str(headPosition) + ";").encode()) #+ ";".encode())

def moveDistance(speed, direction, distance):
    port.write("md;".encode())
    port.write(str(speed).encode() + ';'.encode())
    port.write(str(direction).encode() + ';'.encode())
    port.write(str(distance).encode() + ';'.encode())

def tightTurn(degrees):
    port.write("tt;".encode())
    port.write(str(degrees).encode() + ';'.encode())

def clamp():
	port.write("c;".encode())

def unclamp():
	port.write("uc;".encode())
