//NAME: Omar Ahmad
//EMAIL: omarahmad4@ucla.edu
//ID: 804801272

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <time.h> //for clock
#include <signal.h>
#include "SortedList.h"

//#include <termios.h>
//#include <poll.h>
//#include <sys/wait.h>

/*
  struct SortedListElement {
  struct SortedListElement *prev;
  struct SortedListElement *next;
  const char *key;
  };
*/

struct sublist //struct for sublists contianing own locks
{
  struct SortedListElement myList;
  //  struct SortedListElement * elements; //containing elements of list
  pthread_mutex_t Mex;
  int spinLock;
};

struct sublist * lists;  //array (to be malloced) containing all lists
struct SortedListElement * elementArr; //containing all elmnt of all lists

int errno, opt_yield;
int iterations = 1;
int numThreads = 1;
int numLists = 1;
long numLockOps = 0;
long long counter = 0;
long long * threadTimes;
//pthread_mutex_t Mex;
//volatile int spinLock = 0;

struct threadData
{
  int size;
  int threadNum;
  struct SortedListElement * tElements;
  char * tSync;
};

void sigHandle()
{
  fprintf(stderr, "Found corrupted list - segfault\n");
  exit(2);
}

void errorExit(char errmsg[], int code, int doErrno)
{
  if(doErrno)
    fprintf(stderr, "Error: %s\nErrno: %s\tExiting...\n", errmsg, strerror(errno));
  else
    fprintf(stderr, "Error: %s\nExiting...\n", errmsg);  
  if(code)
    exit(code);
}

unsigned long hash(char *str) //taken from http://www.cse.yorku.ca/~oz/hash.html under djb2
{
  unsigned long hash = 5381;
  int c;
  while ( (c = *str++) )
    hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
  return hash;
}

long long calcTime(struct timespec start, struct timespec end)
{
  return 1000000000*(end.tv_sec - start.tv_sec) + end.tv_nsec - start.tv_nsec;
}

void* threadFunc(void* Data)
{
  struct threadData tData = *(struct threadData *) Data;
  struct timespec start_t, end_t; 
  struct SortedListElement * elements = tData.tElements;
  //  int numElem = tData.size;
  int tNum = tData.threadNum;
  char sync = *tData.tSync;
  char tKey;
  unsigned long hashRet;

  int startPos, endPos;
  startPos = tNum*iterations;
  endPos = (tNum+1)*iterations;
  
  SortedListElement_t * tempPtr; //for lookup
  struct SortedListElement * thisList;
  struct sublist * thisSub;
  int len;
  int j;
  for(j = startPos; j < endPos; j++)
    {
      switch (sync)
	{
	case 'm': //mutex
	  tKey = *elements[j].key;
	  //	  printf("key = %c\n", tKey);
	  //hashRet = hash(&tKey)%numLists;
	  hashRet = (int)tKey % numLists;
	  thisList = &lists[hashRet].myList;
	  thisSub = &lists[hashRet];
	  
	  clock_gettime(CLOCK_MONOTONIC, &start_t); //timing lock time
	  pthread_mutex_lock(&thisSub->Mex);
  	  clock_gettime(CLOCK_MONOTONIC, &end_t);
 	  SortedList_insert(thisList, &elements[j]);
	  pthread_mutex_unlock(&thisSub->Mex);
	  numLockOps ++;
	  threadTimes[tNum] += calcTime(start_t, end_t);

	  clock_gettime(CLOCK_MONOTONIC, &start_t);
	  pthread_mutex_lock(&thisSub->Mex);
	  clock_gettime(CLOCK_MONOTONIC, &end_t);
	  len = SortedList_length(thisList);   //length  
	  pthread_mutex_unlock(&thisSub->Mex);
	  numLockOps ++;
	  threadTimes[tNum] += calcTime(start_t, end_t);

	  if(len == -1)
	    {
	      //printf("startPos = %i, j = %i, key = %c, hashRet = %li\n", startPos, j, tKey, hashRet);
	      errorExit("Found corrupted list while checking length", 2, 0);
	    }
	  else if (len == 0)
	    errorExit("Empty list after insertion", 2, 0);
	  else
	    {
	      clock_gettime(CLOCK_MONOTONIC, &start_t);
	      pthread_mutex_lock(&thisSub->Mex);
	      clock_gettime(CLOCK_MONOTONIC, &end_t);
	      tempPtr = SortedList_lookup(thisList, &tKey);
	      if(tempPtr == NULL)
		errorExit("Lookup failed!", 2, 0); //2 for failure on sync
	      SortedList_delete(tempPtr); //delete the looked up elmt
	      pthread_mutex_unlock(&thisSub->Mex);
	      numLockOps++;
	      threadTimes[tNum] += calcTime(start_t, end_t);
	    }
	  break;
      
	case 's': //spin lock
	  tKey = *elements[j].key;
	  //hashRet = hash(&tKey)%numLists;
	  hashRet = (int)tKey % numLists;
	  

	  thisList = &lists[hashRet].myList;
	  thisSub = &lists[hashRet];
	 
	  clock_gettime(CLOCK_MONOTONIC, &start_t);
	  while(__sync_lock_test_and_set(&thisSub->spinLock, 1))
	    {;}
	  clock_gettime(CLOCK_MONOTONIC, &end_t);
	  SortedList_insert(thisList, &elements[j]);
	  __sync_lock_release(&thisSub->spinLock);
	  numLockOps++;
	  threadTimes[tNum] += calcTime(start_t, end_t);
	
	  clock_gettime(CLOCK_MONOTONIC, &start_t);
	  while(__sync_lock_test_and_set(&thisSub->spinLock, 1))
	    {;}      
	  clock_gettime(CLOCK_MONOTONIC, &end_t);
	  len = SortedList_length(thisList);
	  __sync_lock_release(&thisSub->spinLock);
	  numLockOps++;
	  threadTimes[tNum] += calcTime(start_t, end_t);
	  if(len == -1)
	    errorExit("Found corrupted list while checking length", 2, 0);
	  else if (len == 0)
	    errorExit("Empty list after insertion", 2, 0);
	  else
	    {
	      clock_gettime(CLOCK_MONOTONIC, &start_t);
	      while(__sync_lock_test_and_set(&thisSub->spinLock, 1))
		{;}
	      clock_gettime(CLOCK_MONOTONIC, &end_t);
	      tempPtr = SortedList_lookup(thisList, &tKey);
	      if(tempPtr == NULL)
		errorExit("Lookup failed!", 2, 0); //2 for failure on synch
	      SortedList_delete(tempPtr); //delete the looked up elmt
	      __sync_lock_release(&thisSub->spinLock);
	      numLockOps++;
	      threadTimes[tNum] += calcTime(start_t, end_t); //add times after releasing to not delay releasing
	    }
	  break;
      
	case 'z': //no lock
	default:
	  tKey = *elements[j].key;
	  //hashRet = hash(&tKey)%numLists;
	  hashRet = (int)tKey % numLists;
	  

	  thisList = &lists[hashRet].myList;

	  SortedList_insert(thisList, &elements[j]);

	  len = SortedList_length(thisList);
	  if(len == -1)
	    {
	      //printf("j = %i", j);
	      errorExit("Found corrupted list while checking length", 2, 0);
	    }
	  else if (len == 0)
	    errorExit("Empty list after insertion", 2, 0);
	  else
	    {
	      tempPtr = SortedList_lookup(thisList, &tKey);
	      if(tempPtr == NULL)
		  errorExit("Lookup failed!", 2, 0); //2 for failure on sync
	      SortedList_delete(tempPtr); //delete the looked up elmt
	    }
	  break;
	}
    }
  return 0;
}


int main (int argc, char **argv)
{
  static struct option optData[] =
    {
     {"threads",required_argument, NULL, 't'},
     {"iterations",required_argument, NULL, 'i'}, 
     {"yield", required_argument, NULL, 'y'}, 
     {"sync",required_argument, NULL, 's'}, 
     {"lists",required_argument, NULL, 'l'}, 
     {0,0,0,0}
    };

  signal(SIGSEGV, sigHandle);
    
  int option;
  char sync = 'z';
  char * yield;
  
  int isArg = 1;
  while (isArg)
    {
      option = getopt_long(argc, argv, "t:i:s:l:", optData, NULL);
      switch(option)
	{
	case 't':
	  numThreads = atoi(optarg);
	  if(numThreads <  1)
	    errorExit("Must input positive number of threads!", 1, 0);
	  break;
	case 'i':
	  iterations = atoi(optarg);
	  if(iterations < 1)
	    errorExit("Must input positive number of iterations!", 1, 0);
	  break;
	case 'l':
	  numLists = atoi(optarg);
	  if(numLists < 1)
	    errorExit("Must input positive number of lists!", 1, 0);
	  break;
	case 'y':
	  if(strlen(optarg) > 3)
	    errorExit("Yield takes any combination of i, d, and l", 1, 0); 
	  opt_yield = 1;
	  yield = optarg;
	  break;
	case 's':
	  if(strlen(optarg) == 1 && (optarg[0] == 'm' || optarg[0] == 's')) // || optarg[0] == 'c'))
	    sync = optarg[0];
	  else
	    errorExit("Sync takes 'm' or 's'", 1, 0);
	  break;
	case -1:
	  isArg = 0;
	  break;
	default:
	  errorExit("Usage: ./lab2-add [--threads=NUMTHREADS] [--iterations=NUMITERATIONS] [--yield=(idl)] [--sync=(m|s)]", 1, 0);
	  break;
	}
    }

  int i;
  int yieldI = 0;
  int yieldD = 0;
  int yieldL = 0;
  if(opt_yield)
    for(i = 0; i < (int)strlen(yield); i++)
      {
	switch (yield[i])
	  {
	  case 'i':
	    yieldI = 1;
	    break;
	  case 'd':
	    yieldD = 1;
	    break;
	  case 'l':
	    yieldL = 1;
	    break;
	  }
      }  

  int size = numThreads*iterations;
  char randChar;
  lists = (struct sublist *) malloc(sizeof(struct sublist)*numLists);
  for(i = 0; i < numLists; i++)
    {
      if(sync == 'm')
	pthread_mutex_init(&lists[i].Mex, NULL); //moved up here bc mutexes weren't working :(
      else if (sync == 's')
	lists[i].spinLock = 0;
    }



  for(i = 0; i < numLists; i++) //initializing
    {
      lists[i].myList = *(SortedList_t *)malloc(sizeof(SortedList_t));
      struct SortedListElement * aList = &lists[i].myList;

      aList->key = NULL;
      aList->next = aList;
      aList->prev = aList;
      // lists[i].myList.key = NULL; //setting key of head of all sublists to NULL
      //lists[i].myList.next = &lists[i].myList;
      //lists[i].myList.prev = &lists[i].myList;
      //      elements = (SortedListElement_t*) malloc(2*size/numlists*sizeof(SortedListElement_t)); //idk 
      //      elements = (SortedListElement_t*) malloc(size*sizeof(SortedListElement_t)); 

    }
  
  //  elementArr = (struct SortedListElement *) malloc(sizeof(struct SortedListElement)*size);
  elementArr = malloc(sizeof(struct SortedListElement)*size);
  for(i = 0; i < size; i++)
    {
      //      randChar = rand();
      randChar = 'A' + (random() % 26);
      elementArr[i].key = &randChar;
    }

  struct threadData tData; //Data structure init to pass to threads
  tData.size = size;
  tData.tElements = elementArr;
  tData.tSync = &sync;

  threadTimes = malloc(sizeof(long long) * numThreads);
  memset(threadTimes, 0, sizeof(long long) * numThreads);
  
  struct timespec start, end;
  if(clock_gettime(CLOCK_MONOTONIC, &start) < 0)
    errorExit("Error wih clock_gettime", 1 ,1);

  pthread_t  threads[numThreads];

  switch(sync)
    {
    case 'm':
      //      pthread_mutex_init(&Mex, NULL);
      for(i = 0; i < numThreads; i++)
	{
	  tData.threadNum = i;
	  pthread_create(&threads[i], NULL, &threadFunc, (void *) &tData);
	}
      for(i = 0; i < numThreads; i++)
	pthread_join(threads[i], NULL);
      for(i = 0; i < numLists; i++)
	pthread_mutex_destroy(&lists[i].Mex);
      break;
    case 's':
    default:
      for(i = 0; i < numThreads; i++)
	{
	  tData.threadNum = i;
	  if (pthread_create(&threads[i], NULL, &threadFunc, (void *) &tData))
	    errorExit("creating threads", 1, 0);
	}
      for(i = 0; i < numThreads; i++)
	if (pthread_join(threads[i], NULL))
	  errorExit("joining threads", 1, 0);
      break;
    }  

  if(clock_gettime(CLOCK_MONOTONIC, &end) < 0)
    errorExit("Error wih clock_gettime", 1 ,1);

  long long startT = start.tv_sec*1000000000 +  start.tv_nsec;
  long long endT = end.tv_sec*1000000000 + end.tv_nsec;
  long long totalTime = endT - startT;
    
  int len;
  for(i = 0; i < numLists; i++)
    {
      len = SortedList_length(&lists[i].myList);
      if(len == -1)
	errorExit("Found corrupted list while checking final lengths", 2, 0);
      else if(len != 0)
	{
	  fprintf(stderr, "List #%i length not equal to zero. Equals %i instead\n", i, len);
	  exit(2);
	}
    }


  long int operations = numThreads*iterations*3;
  char  msg [100];
  char  buf  [50];
  strcpy(msg, "list");
  if(opt_yield)
    {
      strcat(msg, "-");
      if(yieldI)
	strcat(msg, "i");
      if(yieldD)
	strcat(msg, "d");
      if(yieldL)
	strcat(msg, "l");
    }
  else
    {
      strcpy(buf, "-none");
      strcat(msg, buf);
    }

  
  memset(buf, 0, strlen(buf));
  switch(sync)
    {
    case 'm':
      strcpy(buf, "-m");
      strcat(msg, buf);
      break;
    case 's':
      strcpy(buf, "-s");
      strcat(msg, buf);
      break;
    case 'z':
    default:
      strcpy(buf, "-none");
      strcat(msg, buf);
      break;
    }


  long long threadTime = 0;
  for(i = 0; i < numThreads; i++)
    threadTime += threadTimes[i];
  long long aveWaitForLock;
  if(sync == 's' || sync == 'm')
    aveWaitForLock = threadTime/numLockOps;
  else
    aveWaitForLock = 0;
  printf("%s,%i,%i,%i,%li,%lli,%lli,%lli\n", msg, numThreads, iterations, numLists, operations, totalTime, totalTime/operations, aveWaitForLock);    


  //  free(elements);
  //  free(&myList);
  return 0;
}
