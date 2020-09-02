### make so pings sensors in background and doesnt wait constantly for server's comms
### make comms independent of game


import RPi.GPIO as GPIO
import numpy as np
import time
import board
import neopixel
import socket 
import threading

IP_ADDRESS = '192.168.137.1' #'192.168.1.8'  #'192.168.137.77' #'0.0.0.0' or ''
PORT = 4000

SENSOR_RESET_TIME = 0.25
SLEEPTIME = 0.05  #time for dist to wait - maybe set to 0??
MEANWINDOWSIZE = 10  #size of running average window - relate to sleeptime

RED = 0 #red light output
GREEN = 1 # green light output
WARNING = 2 #warning for when getting close to moving too much
FAULT = 3 #when player has moved too much and must go back

TRIG1 = 4
ECHO1 = 3

TRIG2 = 2
ECHO2 = 14

GPIO.setwarnings(False)
GPIO.setmode(GPIO.BCM)
GPIO.setup(TRIG1, GPIO.OUT)
GPIO.setup(ECHO1, GPIO.IN)
GPIO.setup(TRIG2, GPIO.OUT)
GPIO.setup(ECHO2, GPIO.IN)

pixels = neopixel.NeoPixel(board.D18, 12)
player1dists = []
player1dists.append(0.0)
Dist1 = 0.0
player2dists = []
player2dists.append(0.0)
Dist2 = 0.0

def running_mean(x, N):
	cumsum = np.cumsum(np.insert(x, 0, 0))
	return (cumsum[N:] - cumsum[:-N]) / float(N)

def getDistHelper(trig, echo):
	GPIO.output(trig, False)
	time.sleep(SLEEPTIME)
	GPIO.output(trig, True)           #sending pulse that will bounce off object to be measured
	time.sleep(0.00001)
	GPIO.output(trig, False)

	#while GPIO.input(echo) == 0:        #timing how long sound wave takes to return
	#pulse_start = time.time()
	#while GPIO.input(echo) == 0:
	#	if ((time.time() - pulse_start) > 0.5):
	#		break;
	#	pass
	#pulse_end = time.time()
	pulse_start = temp = time.time()
	while GPIO.input(echo) == 0:        #timing how long sound wave takes to return
		pulse_start = time.time()
		if(pulse_start - temp > SENSOR_RESET_TIME):
			break
	pulse_end = time.time()
	while GPIO.input(echo) == 1:
		pulse_end = time.time()
		if(pulse_end - pulse_start > SENSOR_RESET_TIME):
			break
	
	pulse_duration = pulse_end - pulse_start
	distance = pulse_duration * 17150
	inchDist = distance/2.54
	return inchDist

def getDist():
	while True:	
		#Dist1 = 0
		global Dist1
		global Dist2
		currDist = getDistHelper(TRIG1, ECHO1) 
		if(currDist < 20*12):
			player1dists.append(round(currDist, 2))
		currLen = len(player1dists)
		mean1 = running_mean(player1dists[currLen-MEANWINDOWSIZE:currLen], MEANWINDOWSIZE)
		if (len(mean1) != 0): 
			Dist1 = round(mean1[0], 2)
			#print("Dist1 : " + str(Dist1))
		#else: print("len mean 1 is 0")
		
		#Dist2 = 0
		currDist = getDistHelper(TRIG2, ECHO2)
		if(currDist < 20*12):
			player2dists.append(currDist)
		currLen = len(player2dists)
		mean2 = running_mean(player2dists[currLen-MEANWINDOWSIZE:currLen], MEANWINDOWSIZE)
		if (len(mean2) != 0): 
			Dist2 = round(mean2[0],2)
			#print("Dist2: " + str(Dist2))
		#else: print("len mean 2 is 0")
	
	#if(len(mean1) != 0 && len(mean2) != 0):
	#	break
		print(str(Dist1) + ',' +  str(Dist2))
	
def changeLight(color): # maybe add winning/resetting sequence(s)
	#change the light to red or green
	if (color == 0):
		pixels.fill((255,0,0))
	elif (color == 1):
		pixels.fill((0,255,0))
	elif (color == 2): 
		print("not supported (yet?) \n")
	elif (color == 3):
		print("not supported (yet?) \n")
	else:
		print("invalid input to changeLight!\n")
		
	
if __name__ == "_main__": 
	x = threading.Thread(target = getDist, daemon = True)
	x.start()
	while 1:
		print("dist1: %f\t dist2: %f" %(Dist1, Dist2))
		time.sleep(0.05)
		

if __name__ == "__main__": 
	#print "Executed when invoked directly"
	try:
		client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		client.connect((IP_ADDRESS, PORT))
		
		
		x = threading.Thread(target = getDist, daemon = True)
		x.start() #x is constantly using ultrasonic sensors and getting running average in background
	
		while 1:
			dataRec = int(client.recv(4096).decode()) 

			if (dataRec != -1):  #server should send RED, GREEN, WARNING, FAULT, etc.
				changeLight(dataRec)
		
			dataToSend = str(Dist1) + ',' +  str(Dist2)
			client.send(dataToSend.encode())
			
			
	except KeyboardInterrupt:
		print("\nCleaning up\n")
	except BrokenPipeError:
		print("\nBroken Pipe!\n")
	except ValueError:
		print("\nHost likely disconnected!\n")
	finally:
		pixels.fill((0,0,0))
		GPIO.cleanup()
		client.close()
#	changeLight(RED)
#	while 1:
		#change light red to green and back and measure dist of players
#		startPlayer1Dist = getDist(1)
#		startPlayer2Dist = getDist(2)



#else: 
	#print "Executed when imported"


