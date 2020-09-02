#NAME: Omar Ahmad
#EMAIL: omarahmad4@ucla.edu
#ID: 804801272

CC = gcc
CFLAGS = -Wall -Wextra -lz  #-g

default: lab1b-client.c lab1b-client lab1b-server.c lab1b-server
	@$(CC)  $(CFLAGS) -o lab1b-client lab1b-client.c 
	@$(CC)  $(CFLAGS) -o lab1b-server lab1b-server.c


clean:
	@rm -f lab1b-server lab1b-client *.txt  *.0  *.gz #add *~ to remove temp files?                                                         
dist:
	@tar -cvzf lab1b-804801272.tar.gz lab1b-client.c lab1b-server.c Makefile README

check: #add test cases    

