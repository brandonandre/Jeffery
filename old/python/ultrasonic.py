import RPi.GPIO as GPIO
import time
import movement

def moveToObject():
    movement.moveDistance(100, movement.FORWARDS, getDistance())

def getDistance():
	GPIO.setmode(GPIO.BCM)
	GPIO.setwarnings(False)

	trigPin = 23
	echoPin = 24

	#setup pins
	GPIO.setup(trigPin,GPIO.OUT)
	GPIO.setup(echoPin,GPIO.IN)

	#send pulse
	GPIO.output(trigPin,True)
	time.sleep(0.00001)
	GPIO.output(trigPin,False)

	#time pulse
	while GPIO.input(echoPin)==0:
   		pulse_start = time.time()

	while GPIO.input(echoPin)==1:
    		pulse_end = time.time()

	#determine distance based on pulse length
	pulse_duration = pulse_end - pulse_start
	distance = pulse_duration * 17150

	#round to 2 decimals
	#distance = round(distance,2)
	return ((distance - 3) * 10)

pass