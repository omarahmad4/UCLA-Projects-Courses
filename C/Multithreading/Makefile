#NAME: Omar Ahmad
#EMAIL: omarahmad4@ucla.edu
#ID: 804801272

CC = gcc
CFLAGS = -pthread -Wall -Wextra -g -lprofiler

default: lab2_list.c SortedList.c SortedList.h 
	@$(CC)  $(CFLAGS) -o lab2_list lab2_list.c SortedList.c

clean:
	@rm -f lab2_add lab2_list *.gz

####*.png *.csv  #*.txt *.o  #add *~ to remove temp files?                                                         

profile:
	@rm -f profile.out
	LD_PRELOAD=/usr/lib64/libprofiler.so CPUPROFILE=./raw.gperf ./lab2_list -t 12 -i 1000 --sync=s
	pprof --text ./lab2_list raw.gperf > profile.out
	pprof --list=threadFunc ./lab2_list raw.gperf >> profile.out


dist: graphs
	@tar -cvzf lab2b-804801272.tar.gz SortedList.h *.gp *.c *.csv *.png  profile.out Makefile README

graphs: tests
	-@./graph.gp

tests:
	-@./lab2_list -t 1  -i 1000 --sync=m >> lab2_list.csv
	-@./lab2_list -t 2  -i 1000 --sync=m >> lab2_list.csv
	-@./lab2_list -t 4  -i 1000 --sync=m >> lab2_list.csv
	-@./lab2_list -t 8  -i 1000 --sync=m >> lab2_list.csv
	-@./lab2_list -t 12 -i 1000 --sync=m >> lab2_list.csv
	-@./lab2_list -t 16 -i 1000 --sync=m >> lab2_list.csv
	-@./lab2_list -t 24 -i 1000 --sync=m >> lab2_list.csv

	-@./lab2_list -t 1  -i 1000 --sync=s >> lab2_list.csv
	-@./lab2_list -t 2  -i 1000 --sync=s >> lab2_list.csv
	-@./lab2_list -t 4  -i 1000 --sync=s >> lab2_list.csv
	-@./lab2_list -t 8  -i 1000 --sync=s >> lab2_list.csv
	-@./lab2_list -t 12 -i 1000 --sync=s >> lab2_list.csv
	-@./lab2_list -t 16 -i 1000 --sync=s >> lab2_list.csv
	-@./lab2_list -t 24 -i 1000 --sync=s >> lab2_list.csv


	-@./lab2_list -t 1  -i 1 --yield=id -l 4 >> lab2_list.csv
	-@./lab2_list -t 1  -i 2 --yield=id -l 4 >> lab2_list.csv
	-@./lab2_list -t 1  -i 4 --yield=id -l 4 >> lab2_list.csv
	-@./lab2_list -t 1  -i 8 --yield=id -l 4 >> lab2_list.csv
	-@./lab2_list -t 1 -i 16 --yield=id -l 4 >> lab2_list.csv

	-@./lab2_list -t 2  -i 1 --yield=id -l 4 >> lab2_list.csv
	-@./lab2_list -t 2  -i 2 --yield=id -l 4 >> lab2_list.csv
	-@./lab2_list -t 2  -i 4 --yield=id -l 4 >> lab2_list.csv
	-@./lab2_list -t 2  -i 8 --yield=id -l 4 >> lab2_list.csv
	-@./lab2_list -t 2 -i 16 --yield=id -l 4 >> lab2_list.csv

	-@./lab2_list -t 4  -i 1 --yield=id -l 4 >> lab2_list.csv
	-@./lab2_list -t 4  -i 2 --yield=id -l 4 >> lab2_list.csv
	-@./lab2_list -t 4  -i 4 --yield=id -l 4 >> lab2_list.csv
	-@./lab2_list -t 4  -i 8 --yield=id -l 4 >> lab2_list.csv
	-@./lab2_list -t 4 -i 16 --yield=id -l 4 >> lab2_list.csv

	-@./lab2_list -t 8  -i 1 --yield=id -l 4 >> lab2_list.csv
	-@./lab2_list -t 8  -i 2 --yield=id -l 4 >> lab2_list.csv
	-@./lab2_list -t 8  -i 4 --yield=id -l 4 >> lab2_list.csv
	-@./lab2_list -t 8  -i 8 --yield=id -l 4 >> lab2_list.csv
	-@./lab2_list -t 8 -i 16 --yield=id -l 4 >> lab2_list.csv

	-@./lab2_list -t 12  -i 1 --yield=id -l 4 >> lab2_list.csv
	-@./lab2_list -t 12  -i 2 --yield=id -l 4 >> lab2_list.csv
	-@./lab2_list -t 12  -i 4 --yield=id -l 4 >> lab2_list.csv
	-@./lab2_list -t 12  -i 8 --yield=id -l 4 >> lab2_list.csv
	-@./lab2_list -t 12 -i 16 --yield=id -l 4 >> lab2_list.csv

	-@./lab2_list -t 16  -i 1 --yield=id -l 4 >> lab2_list.csv
	-@./lab2_list -t 16  -i 2 --yield=id -l 4 >> lab2_list.csv
	-@./lab2_list -t 16  -i 4 --yield=id -l 4 >> lab2_list.csv
	-@./lab2_list -t 16  -i 8 --yield=id -l 4 >> lab2_list.csv
	-@./lab2_list -t 16 -i 16 --yield=id -l 4 >> lab2_list.csv

	-@./lab2_list -t 1  -i 1 --yield=id -l 4 -s s >> lab2_list.csv
	-@./lab2_list -t 1  -i 2 --yield=id -l 4 -s s >> lab2_list.csv
	-@./lab2_list -t 1  -i 4 --yield=id -l 4 -s s >> lab2_list.csv
	-@./lab2_list -t 1  -i 8 --yield=id -l 4 -s s >> lab2_list.csv
	-@./lab2_list -t 1 -i 16 --yield=id -l 4 -s s >> lab2_list.csv

	-@./lab2_list -t 2  -i 1 --yield=id -l 4 -s s >> lab2_list.csv
	-@./lab2_list -t 2  -i 2 --yield=id -l 4 -s s >> lab2_list.csv
	-@./lab2_list -t 2  -i 4 --yield=id -l 4 -s s >> lab2_list.csv
	-@./lab2_list -t 2  -i 8 --yield=id -l 4 -s s >> lab2_list.csv
	-@./lab2_list -t 2 -i 16 --yield=id -l 4 -s s >> lab2_list.csv

	-@./lab2_list -t 4  -i 1 --yield=id -l 4 -s s >> lab2_list.csv
	-@./lab2_list -t 4  -i 2 --yield=id -l 4 -s s >> lab2_list.csv
	-@./lab2_list -t 4  -i 4 --yield=id -l 4 -s s >> lab2_list.csv
	-@./lab2_list -t 4  -i 8 --yield=id -l 4 -s s >> lab2_list.csv
	-@./lab2_list -t 4 -i 16 --yield=id -l 4 -s s >> lab2_list.csv

	-@./lab2_list -t 8  -i 1 --yield=id -l 4 -s s >> lab2_list.csv
	-@./lab2_list -t 8  -i 2 --yield=id -l 4 -s s >> lab2_list.csv
	-@./lab2_list -t 8  -i 4 --yield=id -l 4 -s s >> lab2_list.csv
	-@./lab2_list -t 8  -i 8 --yield=id -l 4 -s s >> lab2_list.csv
	-@./lab2_list -t 8 -i 16 --yield=id -l 4 -s s >> lab2_list.csv

	-@./lab2_list -t 12  -i 1 --yield=id -l 4 -s s >> lab2_list.csv
	-@./lab2_list -t 12  -i 2 --yield=id -l 4 -s s >> lab2_list.csv
	-@./lab2_list -t 12  -i 4 --yield=id -l 4 -s s >> lab2_list.csv
	-@./lab2_list -t 12  -i 8 --yield=id -l 4 -s s >> lab2_list.csv
	-@./lab2_list -t 12 -i 16 --yield=id -l 4 -s s >> lab2_list.csv

	-@./lab2_list -t 16  -i 1 --yield=id -l 4 -s s >> lab2_list.csv
	-@./lab2_list -t 16  -i 2 --yield=id -l 4 -s s >> lab2_list.csv
	-@./lab2_list -t 16  -i 4 --yield=id -l 4 -s s >> lab2_list.csv
	-@./lab2_list -t 16  -i 8 --yield=id -l 4 -s s >> lab2_list.csv
	-@./lab2_list -t 16 -i 16 --yield=id -l 4 -s s >> lab2_list.csv

	-@./lab2_list -t 1  -i 1 --yield=id -l 4 -s m >> lab2_list.csv
	-@./lab2_list -t 1  -i 2 --yield=id -l 4 -s m >> lab2_list.csv
	-@./lab2_list -t 1  -i 4 --yield=id -l 4 -s m >> lab2_list.csv
	-@./lab2_list -t 1  -i 8 --yield=id -l 4 -s m >> lab2_list.csv
	-@./lab2_list -t 1 -i 16 --yield=id -l 4 -s m >> lab2_list.csv

	-@./lab2_list -t 2  -i 1 --yield=id -l 4 -s m >> lab2_list.csv
	-@./lab2_list -t 2  -i 2 --yield=id -l 4 -s m >> lab2_list.csv
	-@./lab2_list -t 2  -i 4 --yield=id -l 4 -s m >> lab2_list.csv
	-@./lab2_list -t 2  -i 8 --yield=id -l 4 -s m >> lab2_list.csv
	-@./lab2_list -t 2 -i 16 --yield=id -l 4 -s m >> lab2_list.csv

	-@./lab2_list -t 4  -i 1 --yield=id -l 4 -s m >> lab2_list.csv
	-@./lab2_list -t 4  -i 2 --yield=id -l 4 -s m >> lab2_list.csv
	-@./lab2_list -t 4  -i 4 --yield=id -l 4 -s m >> lab2_list.csv
	-@./lab2_list -t 4  -i 8 --yield=id -l 4 -s m >> lab2_list.csv
	-@./lab2_list -t 4 -i 16 --yield=id -l 4 -s m >> lab2_list.csv

	-@./lab2_list -t 8  -i 1 --yield=id -l 4 -s m >> lab2_list.csv
	-@./lab2_list -t 8  -i 2 --yield=id -l 4 -s m >> lab2_list.csv
	-@./lab2_list -t 8  -i 4 --yield=id -l 4 -s m >> lab2_list.csv
	-@./lab2_list -t 8  -i 8 --yield=id -l 4 -s m >> lab2_list.csv
	-@./lab2_list -t 8 -i 16 --yield=id -l 4 -s m >> lab2_list.csv

	-@./lab2_list -t 12  -i 1 --yield=id -l 4 -s m >> lab2_list.csv
	-@./lab2_list -t 12  -i 2 --yield=id -l 4 -s m >> lab2_list.csv
	-@./lab2_list -t 12  -i 4 --yield=id -l 4 -s m >> lab2_list.csv
	-@./lab2_list -t 12  -i 8 --yield=id -l 4 -s m >> lab2_list.csv
	-@./lab2_list -t 12 -i 16 --yield=id -l 4 -s m >> lab2_list.csv

	-@./lab2_list -t 16  -i 1 --yield=id -l 4 -s m >> lab2_list.csv
	-@./lab2_list -t 16  -i 2 --yield=id -l 4 -s m >> lab2_list.csv
	-@./lab2_list -t 16  -i 4 --yield=id -l 4 -s m >> lab2_list.csv
	-@./lab2_list -t 16  -i 8 --yield=id -l 4 -s m >> lab2_list.csv
	-@./lab2_list -t 16 -i 16 --yield=id -l 4 -s m >> lab2_list.csv


	-@./lab2_list -t 1  -i 1000 --sync=m >> lab2_list.csv
	-@./lab2_list -t 2  -i 1000 --sync=m >> lab2_list.csv
	-@./lab2_list -t 4  -i 1000 --sync=m >> lab2_list.csv
	-@./lab2_list -t 8  -i 1000 --sync=m >> lab2_list.csv
	-@./lab2_list -t 12 -i 1000 --sync=m >> lab2_list.csv

	-@./lab2_list -t 1  -i 1000 --sync=s >> lab2_list.csv
	-@./lab2_list -t 2  -i 1000 --sync=s >> lab2_list.csv
	-@./lab2_list -t 4  -i 1000 --sync=s >> lab2_list.csv
	-@./lab2_list -t 8  -i 1000 --sync=s >> lab2_list.csv
	-@./lab2_list -t 12 -i 1000 --sync=s >> lab2_list.csv

	-@./lab2_list -t 1  -i 1000 --sync=m -l 4 >> lab2_list.csv
	-@./lab2_list -t 2  -i 1000 --sync=m -l 4 >> lab2_list.csv
	-@./lab2_list -t 4  -i 1000 --sync=m -l 4 >> lab2_list.csv
	-@./lab2_list -t 8  -i 1000 --sync=m -l 4 >> lab2_list.csv
	-@./lab2_list -t 12 -i 1000 --sync=m -l 4 >> lab2_list.csv

	-@./lab2_list -t 1  -i 1000 --sync=s -l 4 >> lab2_list.csv
	-@./lab2_list -t 2  -i 1000 --sync=s -l 4 >> lab2_list.csv
	-@./lab2_list -t 4  -i 1000 --sync=s -l 4 >> lab2_list.csv
	-@./lab2_list -t 8  -i 1000 --sync=s -l 4 >> lab2_list.csv
	-@./lab2_list -t 12 -i 1000 --sync=s -l 4 >> lab2_list.csv

	-@./lab2_list -t 1  -i 1000 --sync=m -l 8 >> lab2_list.csv
	-@./lab2_list -t 2  -i 1000 --sync=m -l 8 >> lab2_list.csv
	-@./lab2_list -t 4  -i 1000 --sync=m -l 8 >> lab2_list.csv
	-@./lab2_list -t 8  -i 1000 --sync=m -l 8 >> lab2_list.csv
	-@./lab2_list -t 12 -i 1000 --sync=m -l 8 >> lab2_list.csv

	-@./lab2_list -t 1  -i 1000 --sync=s -l 8 >> lab2_list.csv
	-@./lab2_list -t 2  -i 1000 --sync=s -l 8 >> lab2_list.csv
	-@./lab2_list -t 4  -i 1000 --sync=s -l 8 >> lab2_list.csv
	-@./lab2_list -t 8  -i 1000 --sync=s -l 8 >> lab2_list.csv
	-@./lab2_list -t 12 -i 1000 --sync=s -l 8 >> lab2_list.csv

	-@./lab2_list -t 1  -i 1000 --sync=m -l 16 >> lab2_list.csv
	-@./lab2_list -t 2  -i 1000 --sync=m -l 16 >> lab2_list.csv
	-@./lab2_list -t 4  -i 1000 --sync=m -l 16 >> lab2_list.csv
	-@./lab2_list -t 8  -i 1000 --sync=m -l 16 >> lab2_list.csv
	-@./lab2_list -t 12 -i 1000 --sync=m -l 16 >> lab2_list.csv

	-@./lab2_list -t 1  -i 1000 --sync=s -l 16 >> lab2_list.csv
	-@./lab2_list -t 2  -i 1000 --sync=s -l 16 >> lab2_list.csv
	-@./lab2_list -t 4  -i 1000 --sync=s -l 16 >> lab2_list.csv
	-@./lab2_list -t 8  -i 1000 --sync=s -l 16 >> lab2_list.csv
	-@./lab2_list -t 12 -i 1000 --sync=s -l 16 >> lab2_list.csv
