# From Python
# It requires OpenCV installed for Python
from imutils import translate, rotate, resize
from playsound import playsound
import speech_recognition as sr
from sys import platform
import numpy as np
import socket
import random
import Listen
import keras
import time
import sys
import cv2
import os

IP_ADDR = ''  #192.168.43.65 #0.0.0.0.0
PORT = 4000

WRONG_ANSWER = "files/buzzer.wav"
CORRECT_ANSWER = "files/clang.mp3"
GAME_WIN = "files/TaDa.mp3"

RAND_LOW = 0
RAND_HIGH = 1
FORGIVE_TIME = 0.75 #0.75 #time after light goes red to forgive movement
FORGIVE_DIST = 10 #inches players can move when not allowed to move without penalty
WIN_DIST = 12 #inches away from ultrasonic sensors that is considered the finish line
QUESTION_READ_TIME = 5
INTERMESSAGE_TIME = 3
INTER_RETREAT_TIME = 3
NUM_UNRECOGNIZED_TRIES = 3

RED = 0#.encode() #red light output
GREEN = 1#.encode() # green light output
WARNING = 2#.encode() #warning for when getting close to moving too much
FAULT = 3#.encode() #when player has moved too much and must go back

DAB = 1
TPOSE = 2
OTHER = 0

Questions = {
	1: "What gets wet while drying? Hint: Mostly in bathrooms.",
	2: "What is full of holes but still holds water? Hint: used mostly in kitchen",
	3: "I follow you all the time and copy your every move, but you can’t touch me or catch me. What am I?",
	4: "What is black when it’s clean and white when it’s dirty? Hint: in classrooms",
	5: "Jimmy’s mother had three children. The first was called April, the second was called May. What was the name of the third?",
	6: "The person who makes it has no need of it; the person who buys it has no use for it. The person who uses it can neither see nor feel it. What is it? Hint: death",
	7: "Tall when I’m young and short when I’m old, I’ll help you to see when the darkness unfolds.",
	8: "What belongs to you, but other people use it more than you? Hint: given by your parents",
	9: "Poor people have it. Rich people need it. If you eat it you die. what is it?",
	10: "What goes up but never comes down? Hint: Birthdays",
	11: "What can you catch, but not throw? Hint: people freaking out about this now",
	12: "I am an odd number. Take away a letter and I become even. What number am I?",
	13: "If you drop me I’m sure to crack, but give me a smile and I’ll always smile back. What am I? Hint: on the wall",
	14: "People make me, save me, use me, raise me. What am I?",
	15: "What has to be broken before you can eat it?",
	16: "Feed me and I will live, but give me a drink and I will die. What am I?"
}

Answers={
	1:"towel",
	2:"sponge",
	3:"shadow",
	4:"blackboard",
	5:"jimmy",
	6:"coffin",
	7:"candle",
	8:"name",
	9:"nothing",
	10:"age",
	11:"disease",
	12:"seven",
	13:"mirror",
	14:"money",
	15:"egg",
	16:"fire"
}

'''
Questions = {
1: "When driving in fog, you should use your:\nAlpha:Fog lights only\t Bravo:High beams\t Charlie:Low beams",
2: "You just sold your vehicle. You must notify the DMV within how many days?\nAlpha:5\t Bravo:10\t Charlie:15",
3: "True or False:\nTo avoid last minute moves, you should be looking down the road to where your vehicle will be in about 5 to 10 seconds.",
4: "You have been involved in a minor traffic collision with a parked vehicle and you can't find the owner. You must:\nAlpha: Leave a note on the vehicle\t Bravo: Report accident without delay to city police or CHP\t Charlie: Both \t Delta: neither",
5: "Unless otherwise posted the speed limit in a residential area is:\nAlpha: 15mph\t Bravo: 25mph\t Charlie: 35mph",
6: "To turn left from a multilane one-way street into a one-way street, you should start your turn from: \nAlpha: Any lane(as long as it is safe)\t Bravo: The lane closest to the left curb\t Charlie: The lane in the center of the road",
7: "You may not park your vehicle:\nAlpha: On the side of the freeway in an emergency\t Bravo: Next to a red painted curb\t Charlie: Within 100 feet of an elementary school",
8: "Two sets of solid, double, yellow lines that are two or more feet apart:\nAlpha: May be crossed to enter or exit a private driveway\t Bravo: May not be crossed for any reason\t Charlie: Should be treated as a seperate traffic lane",
9: "It is illegal to park your vehicle:\nAlpha: In an unmarked crosswalk\t Bravo: Within three feet of a private driveway\t Charlie: In a bicycle lane",
10: "A solid yellow line next to a broken yellow line means that vehicles:\nAlpha: in both directions may pass\t Bravo: Next to a broken line may pass\t Charlie: Next to the solid line may pass"
}

Answers={
1:"charlie",
2:"alpha",
3:"false",
4:"charlie",
5:"bravo",
6:"bravo",
7:"bravo",
8:"bravo",
9:"alpha",
10:"bravo"
}
'''

def chooseQuestion():
	global previous_question
	num = previous_question
	while num == previous_question:
		num = random.randrange(1, len(Questions)+1)
	previous_question = num
	return (Questions[num], num)

def listen(playerNum):
	return Listen.listen(playerNum)

def getRandTime():
	return random.uniform(RAND_LOW, RAND_HIGH)

def getDists(ColorToSend = -1):
	conn.send(str(ColorToSend).encode()) 
	dataRec = conn.recv(4096).decode()
	distances = dataRec.split(',') #1st elem is p1 dist and 2nd is p2 dist
	return distances

def moveBackToStart(playerNum):  #removed implementation where it forces them to go back specific distance in favor of just telling them to go back to the start line
	print("Player %d move back to the start line!" % (playerNum))
	time.sleep(INTER_RETREAT_TIME)
	print("Resuming play shortly...")
	time.sleep(INTER_RETREAT_TIME)
	'''
	global initDists
	playerIndex = playerNum - 1
	distances = getDists()
	gap = float(initDists[playerIndex])-float(distances[playerIndex])
	#print("gap: " + str(gap)) 
	while( gap > FORGIVE_DIST):
		print("Player %d move back to the start line!" % (playerNum))
		print("You still have %d inches to go" % (gap - FORGIVE_DIST))
		time.sleep(INTER_RETREAT_TIME)
		distances = getDists()
		gap = float(initDists[playerIndex])-float(distances[playerIndex])
	print("Resuming play shortly...")
	time.sleep(INTER_RETREAT_TIME)
	'''	
	
def moveBackDist(playerNum, Dist, OrigDist = -1):  #removed implementation where it forces them to go back specific distance in favor of just telling them to take a step back
	print("Player %d move back 2 feet (1 tile)" % (playerNum))
	time.sleep(INTER_RETREAT_TIME)
	print("Resuming play shortly...")
	time.sleep(INTER_RETREAT_TIME)
	'''
	#if origdist is defined, move Dist back from there else move Dist back from current position
	global initDists
	playerIndex = playerNum - 1
	distances = getDists()
	if OrigDist == -1:
		targetDist = float(distances[playerIndex]) + float(Dist)
	else:
		targetDist = float(OrigDist) + float(Dist)
	#print("Target: " + str(targetDist))
	initD = float(initDists[playerIndex])
	if (targetDist > initD):
		targetDist = initD
	#print("Target: " + str(targetDist))
	gap = targetDist - float(distances[playerIndex])
	#gap = float(OrigDist[playerIndex])-float(distances[playerIndex])

	#print("gap: " + str(gap)) 
	while( gap > FORGIVE_DIST):
		print("Player %d continue moving back!" % (playerNum))
		print("You still have %d inches to go" % (gap - FORGIVE_DIST))
		time.sleep(INTER_RETREAT_TIME)
		distances = getDists()
		#gap = float(OrigDist[playerIndex])-float(distances[playerIndex])
		gap = targetDist - float(distances[playerIndex])
	print("Resuming play shortly...")
	time.sleep(INTER_RETREAT_TIME)
	'''
	
def checkDists(desiredDists): # , initDists):  #desired is where they should be 
	p1 = float(desiredDists[0])
	p2 = float(desiredDists[1])
	distances = getDists(RED)
	#print("init dists: %f & %f" % (p1, p2)) 
	#print("new dists: %f & %f" % (float(distances[0]), float(distances[1]))) 
	#print("diff dists: %f & %f" % (abs(float(distances[0]) - p1), abs(float(distances[1]) - p2)))
	diff1 = p1 - float(distances[0])
	#print("p1: %d\t d[0]: %d\t diff1: %d" % (p1, float(distances[0]), diff1))
	if (diff1 > FORGIVE_DIST):
		print("Player 1 moving while I am speaking!")
		#PRINT BELOW LINE IN ITALICS IF POSS TO SIGNIFY A 'NARRATOR'
		#print("The Sphinx attacks player 1 and moves them a foot behind where they were before their question began")
		print("The Sphinx attacks player 1 and moves them 2 feet back")
			
		#print("Player 1 go back to the beginning")
		#moveBackToStart(1)
		moveBackDist(1, 12, p1)
		
	diff2 = p2 - float(distances[1])
	#print("p2: %d\t d[0]: %d\t diff2: %d" % (p2, float(distances[1]), diff2))
	if (diff2 > FORGIVE_DIST):
		print("Player 2 moving while I am speaking!")
		#PRINT BELOW LINE IN ITALICS IF POSS TO SIGNIFY A 'NARRATOR'
		#print("The Sphinx attacks player 2 and moves them a foot behind where they were before their question began")
		print("The Sphinx attacks player 2 and moves them 2 feet back")
		#print("Player 2 go back to the beginning")
		#moveBackToStart(2)
		moveBackDist(2, 12, p2)
				
def administerQuestion(playerNum): #playerNum is 1 or 2
	QnA = chooseQuestion() #QnA is a tuple of the question and the question number
	print("I have a riddle for player %d:" % (playerNum))
	print(QnA[0])
	print("Wait until I prompt you to speak")
	time.sleep(QUESTION_READ_TIME)
	audio = listen(playerNum)
	times_null = 0
	while (audio == "null"):
		print("I could not understand, Try again")
		audio = listen(playerNum)
		times_null += 1
		if times_null == NUM_UNRECOGNIZED_TRIES:
			print("Speak clearly next try")
			print("The Sphinx attacks player %d and this pushes them 2 feet back" % (playerNum))
			moveBackDist(playerNum, 12)
			return False
	#if Answers[QnA[1]] == "true":
		
	print("You said:\n %s" % (audio))
	if Answers[QnA[1]] in audio.lower():
		print ("That is correct")
		playsound(CORRECT_ANSWER)
		return True
	else: ##add ability to relisten when audio deciphered but doesn't contain  
		print("That is incorrect")
		playsound(WRONG_ANSWER)
		print("The Sphinx attacks player %d and this pushes them 2 feet back" % (playerNum))
		moveBackDist(playerNum, 12)
		return False

def checkAndAdminGesture(playerNum):
	distances = getDists()
	playerIndex = playerNum - 1
	if float(distances[playerIndex]) < WIN_DIST:
			print("Player %d come and face me and prepare to perform the holy gesture to declare your loyalty to me" % (playerNum))
			if recogGesture(2):
				print("\nCongratulations Player %d! You win!\n Your treasure awaits..." % (playerNum))
				playsound(GAME_WIN)
				time.sleep(5)
				sys.exit()
			else:
				print("You are unworthy! Return to the beginning to try again")
				#Gesture recognition failed. Return to the start line")
				moveBackToStart(playerNum)

def recogGesture(gestureNum):
	np.random.seed(1337)
	dir_path = os.path.dirname(os.path.realpath(__file__))
	sys.path.append(dir_path + '/../../python/openpose/Release');
	os.environ['PATH']  = os.environ['PATH'] + ';' + dir_path + '/../../x64/Release;' +  dir_path + '/../../bin;'
	import pyopenpose as op
	# Custom Params (refer to include/openpose/flags.hpp for more parameters)
	params = dict()
	params["model_folder"] = "../../../models/"

	print("OpenPose start")
	cap = cv2.VideoCapture(0)
	cap.set(cv2.CAP_PROP_FRAME_WIDTH, 1280)
	cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 720)

	tposer = keras.models.load_model('dab-tpose-other.h5')

	# Construct it from system arguments
	# op.init_argv(args[1])
	# oppython = op.OpenposePython()

	# Starting OpenPose
	opWrapper = op.WrapperPython()
	opWrapper.configure(params)
	opWrapper.start()

	# Process Image
	datum = op.Datum()

	np.set_printoptions(precision=4)

	fps_time = 0

	bounced = time.time()
	debounce = 3 # wait 3 seconds before allowing another command
	while cap.isOpened():
		ret_val, frame = cap.read()

		datum.cvInputData = frame
		opWrapper.emplaceAndPop([datum])

		# need to be able to see what's going on
		image = datum.cvOutputData
		cv2.putText(image,
				   "FPS: %f" % (1.0 / (time.time() - fps_time)),
					(10, 20),  cv2.FONT_HERSHEY_SIMPLEX, 0.5,
					(0, 255, 0), 2)
		
		cv2.imshow("Openpose", image)

		if datum.poseKeypoints.any():
			first_input = datum.poseKeypoints
			try:
				first_input[:,:,0] = first_input[:,:,0] / 720
				first_input[:,:,1] = first_input[:,:,1] / 1280
				first_input = first_input[:,:,1:]
				first_input = first_input.reshape(len(datum.poseKeypoints), 50)
			except:
				continue

			output = tposer.predict_classes(first_input)
			for j in output:
				if j == gestureNum:
					if (time.time() - bounced) < debounce:
						continue
					print("gesture detected!")
					return True
					bounced = time.time()
				#else: return False
				#elif j == 2:
				#	if (time.time() - bounced) < debounce:
				#		continue				   
				#	print("tpose detected")
				#	bounced = time.time()
					
		fps_time = time.time()
		
		# quit with a q keypress, b or m to save data
		key = cv2.waitKey(1) & 0xFF
		if key == ord("q"):
			break

	# clean up after yourself
	cap.release()
	cv2.destroyAllWindows()  
	
if __name__ == "__main__": 
	if (len(Questions) != len(Answers)):
		print("error! length of questions must equal length of answers\nExiting...")
		exit()
	
	serv = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	serv.bind((IP_ADDR, PORT)) 
	serv.listen(5)
	conn, addr = serv.accept() ##not sure about positioning - in or out of loop
	previous_question = -1
	
	try:
		##wait for distances to not be zero
		distances = getDists()
		while( float(distances[0]) == 0 or float(distances[1]) == 0): 
			distances = getDists()
		distances = getDists()
		
		print("You two were part of a large group of adventurers")
		time.sleep(INTERMESSAGE_TIME)
		print("Now you are the only ones left")
		time.sleep(INTERMESSAGE_TIME)
		print("You were searching for tomb of ancient Egyptian Pharaoh Tutankhamun which holds boundless treasure")
		time.sleep(INTERMESSAGE_TIME)
		print("The last obstacle between you and the treasure is the mysterious and powerful Sphinx")
		time.sleep(INTERMESSAGE_TIME)
		print("He speaks to you with a booming voice:")
		time.sleep(INTERMESSAGE_TIME)
		print("The first of you to reach me shall receive the treasure")
		time.sleep(INTERMESSAGE_TIME)
		print("When I speak to you, do not move. If you do I will attack you")
		time.sleep(INTERMESSAGE_TIME)
		print("I will question each of you to make sure you are clever, worthy, and capable of handling such a prize")
		time.sleep(INTERMESSAGE_TIME)
		print("When I am about to question you, this lamp will turn red")
		getDists(RED)
		time.sleep(INTERMESSAGE_TIME)
		print("When you may move, it will turn green")
		getDists(GREEN)
		time.sleep(INTERMESSAGE_TIME)
		getDists(RED)
		print("Once you reach me, you will have one last test: you will have to perform a holy gesture to signify your loyalty to me")
		time.sleep(INTERMESSAGE_TIME)
		print("Arrange yourselves to be equally far from me")
		time.sleep(INTERMESSAGE_TIME)
		while (abs(float(distances[0])-float(distances[1])) >  FORGIVE_DIST):
			print("Arrange yourselves to be equally far from me")
			time.sleep(1)
			distances = getDists()
		initDists = getDists()  ## move location of init dists?
		#time.sleep(INTERMESSAGE_TIME)
		print("Prepare yourselves, only one shall succeed....")
		time.sleep(INTERMESSAGE_TIME)
				
		while 1:  #main game loop
			getDists(GREEN)
			time.sleep(getRandTime())
			getDists(RED)
			time.sleep(FORGIVE_TIME) 
			redDists = getDists(RED)
			
			checkAndAdminGesture(1)
			checkAndAdminGesture(2)
			
			checkDists(redDists)
			administerQuestion(1)
			checkDists(redDists)
			administerQuestion(2)	
			checkDists(redDists)
			

	except SystemExit:
		print("\nGame over. Cleaning up...\n")
	except KeyboardInterrupt:
		print("\nKeyboard Interrupt!\nCleaning up\n")
	except ConnectionAbortedError:
		print("Connection Aborted by Client")
	except ConnectionResetError:
		print("Connection Reset by Client")
	finally:
		conn.close()