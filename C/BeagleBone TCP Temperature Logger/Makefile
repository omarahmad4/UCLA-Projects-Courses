#NAME: Omar Ahmad
#EMAIL: omarahmad4@ucla.edu
#ID: 804801272

CC = gcc
CFLAGS = -Wall -Wextra -g -lmraa -lm

default: source.c source2.c
	@$(CC) $(CFLAGS) -o lab4c_tcp source.c
	@$(CC) $(CFLAGS) -lssl -lcrypto -o lab4c_tls source2.c

clean:
	@rm -f *.gz  

dist: 
	@tar -cvzf lab4c-804801272.tar.gz source.c source2.c Makefile README 

check:

#	@./test.sh
#	if [ $? -ne 0 ]; \
#	then \
#		echo "FAILED"
#	fi	
