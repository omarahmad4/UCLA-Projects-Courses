/*
  NAME: Omar Ahmad 
  EMAIL: omarahmad4@ucla.edu
  ID: 804801272
*/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <termios.h>
#include <getopt.h>
#include <errno.h>
#include <string.h>
#include <poll.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/stat.h>    //open(2)
#include <sys/types.h>   //socket, conenct, listen, shutdown, accept
#include <sys/socket.h> 
#include <netdb.h>       //for gethostbyname
#include "zlib.h"        //compression
#include <fcntl.h>
#include <ulimit.h>



int errno, status, i; //global bc used in multiple functions
/*int pipeTo[2];  //to shell (from parent)
  int pipeFrom[2];//from shell (to parent)

  void createPipe(int fd[2])
  {
  if(pipe(fd) == -1)
  {
  fprintf(stderr, "ERROR creating pipe! Exiting...\n");
  exit(1);
  }
  }
*/


int myDef (char* inp, int inLen, char* out, int outLen)
{
  //  void* zinp = (void*) inp;
  //void* zout = (void*) out;
  z_stream stream;
  stream.next_in = (Bytef *) inp;
  stream.avail_in = inLen;
  stream.next_out = (Bytef *) out;
  stream.avail_out = outLen;

  stream.zalloc = Z_NULL; //must be initialized according to man
  stream.zfree = Z_NULL;
  stream.opaque = Z_NULL;
  
  int zret;
  zret = deflateInit(&stream, 9); //max compression
  if(zret != Z_OK)
    {
      fprintf(stderr, "Error with deflateInit! Returned \"%d\"  Exiting...\n", zret);
      exit(1);
    }
  zret = deflate(&stream, Z_FINISH); //Z_ZYNC_FLUSH); finish bc all in one step

  int ret = -1;
  ret = (int) stream.total_out;

  if(ret < 0)
    {
      fprintf(stderr, "Error deflating! Exiting...\n");
      exit(1);
    }
  
  if(deflateEnd(&stream) != Z_OK)
    {
      fprintf(stderr, "Error with deflateEnd! Exiting...\n");
      exit(1);
    }
  return ret;
}

int myinf (char* inp, int inLen, char* out, int outLen)
{
  //  void* zinp = (void*) inp;
  //void* zout = (void*) out;
  z_stream stream;
  stream.next_in = (Bytef *) inp;
  stream.avail_in = inLen;
  stream.next_out = (Bytef *) out;
  stream.avail_out = outLen;

  stream.zalloc = Z_NULL; //must be initialized according to man
  stream.zfree = Z_NULL;
  stream.opaque = Z_NULL;
  
  int zret;
  zret = inflateInit(&stream); 
  if(zret != Z_OK)
    {
      fprintf(stderr, "Error with inflateInit! Returned \"%d\"  Exiting...\n", zret);
      exit(1);
    }
  zret = inflate(&stream, Z_FINISH); //Z_ZYNC_FLUSH); finish bc all in one step

  int ret = -1;
  ret = (int) stream.total_out;

  if(ret < 0)
    {
      fprintf(stderr, "Error inflating! Exiting...\n");
      exit(1);
    }
  
  if(inflateEnd(&stream) != Z_OK)
    {
      fprintf(stderr, "Error with inflateEnd! Exiting...\n");
      exit(1);
    }
  

  
  return ret;
}


/* Use this variable to remember original terminal attributes. */
struct termios saved_attributes;

void reset_input_mode (void)
{
  tcsetattr (STDIN_FILENO, TCSANOW, &saved_attributes);
  
  /*  if(waitpid(cpid, &status, 0) < 0 )
   {
      fprintf(stderr, "Error with waitpid! Exiting...\n");
      exit(1);
    }
  */
  if (WIFEXITED(status)) //if child terminated normally 
    {
      fprintf(stderr, "SHELL EXIT SIGNAL=%d STATUS=%d\n", WTERMSIG(status), WEXITSTATUS(status));
      exit(0);
    }
  
}

void set_input_mode (void)
{
  struct termios tattr;

  // Make sure stdin is a terminal. 
  if (!isatty (STDIN_FILENO))
    {
      fprintf (stderr, "Not a terminal.\n");
      exit (1);
    }

  // Save the terminal attributes so we can restore them later. 
  tcgetattr (STDIN_FILENO, &saved_attributes);  //stdin_fileno = 0
  atexit (reset_input_mode);

  // Set the funny terminal modes. 
  tcgetattr (STDIN_FILENO, &tattr);
  tattr.c_iflag = ISTRIP;
  tattr.c_lflag = 0;    
  tattr.c_oflag = 0;
  tcsetattr (STDIN_FILENO, TCSANOW, &tattr);
}

int main (int argc, char **argv)
{
  static struct option optData[] =
    {
     {"port", required_argument, NULL, 'p'},
     {"log", required_argument, NULL, 'l'},
     {"compress", no_argument, NULL, 'c'},
     {      0, 0,    0,   0}
    };

  int option, port, isExit, isLog, isCompress;
  port = 0;
  isExit = 0;
  isLog = 0;
  isCompress = 0;
  char log [255];
  while(1)
    {
      option = getopt_long(argc, argv, "", optData, NULL);
      switch(option)
	{
	case 'p':
	  port = atoi(optarg);
	  if(port < 1024)
	    {
	      fprintf(stderr, "Error: invalid port number! Must be at least 1024. Exiting...\n");
	      exit(1);
	    }
	  break;
	case 'l':
	  strncpy(log, optarg, 255);
	  isLog = 1;
	  break;
	case 'c':
	  isCompress = 1;
	  break;
	case -1:
	  isExit = 1;
	  break;
	default:
	  fprintf(stderr, "ERROR: Unrecognized option \"%s\" detected!\n", argv[optind-1]);
	  fprintf(stderr, "Usage: ./lab1b-client --port=<PortNumber> (--log=<FileName>)\n");
	  exit(1);
	  break;
	}
      if (isExit)
	break;
    }
  if(!port)
    {
      fprintf(stderr, "Error: Must include port option with port number!\n");
      fprintf(stderr, "Usage: ./lab1b-client --port=<port #> (--log=<logfile>)\n");
      exit(1);
    }

  int logFD = -1;  //creating/opening log file
  if(isLog)
    {
      logFD = open(log, O_RDWR | O_TRUNC | O_CREAT, 0666);
      if(logFD == -1)
	{
	  fprintf(stderr, "Error opening logfile! Exiting...\n");
	  exit(1);
	}
      if(ulimit(UL_SETFSIZE, 10000) < 0)
	fprintf(stderr, "Error with ulimit! contining...\n");
    }




  //DEBUGGER START
  char bugLog[255];
  sprintf(bugLog, "bugLog.txt");
  int bLog = -1;
  //logFD = open(log, O_RDWR | O_TRUNC, 0666);
  bLog = open(bugLog, O_RDWR | O_APPEND | O_CREAT, 0666);
  ulimit(UL_SETFSIZE, 10000);


  if(bLog)
    {
      ;
    }

  
  //DEBUGGER END
  


  

  set_input_mode (); //after getopt and opening lofile
  
  int sockFD;
  sockFD = socket(AF_INET, SOCK_STREAM, 0);
  if(sockFD == -1)
    {
      fprintf(stderr, "Error creating socket!\n%s\nExiting...", strerror(errno));
      exit(1);
    }
  
  struct hostent * host; //(server)
  host = gethostbyname("localhost");
  //int hostLen = host->h_length;
  //  sockFD.sin_port=htons(port);
  
  struct sockaddr_in mySockAddr;
  memset(&mySockAddr, 0 , sizeof(mySockAddr));
  mySockAddr.sin_family = AF_INET;
  mySockAddr.sin_port = htons(port);
  //memcpy((void *) mySockAddr.sin_addr.s_addr, host->h_addr, host->h_length);
  mySockAddr.sin_addr.s_addr = atoi(host->h_addr); //"localhost";//what addr goes here??

  if(connect(sockFD, (struct sockaddr *) &mySockAddr, sizeof(mySockAddr)) < 0 ) //hostlen for last param?
    {
      fprintf(stderr, "Error connecting to socket! Exiting...\n");
      exit(1);
    }


  int nBytes;

  struct pollfd pollfds[2];
  pollfds[0].fd = 0;
  pollfds[0].events = POLLIN | POLLERR | POLLHUP;
  pollfds[1].fd = sockFD; //pipeFrom[0];
  pollfds[1].events = POLLIN | POLLERR | POLLHUP;
	
  int pollret;
  char buffer[256];
  char a;

  while (1)
    {
      pollret = poll(pollfds, 2, 0);

      if (pollret < 0)
	{
	  fprintf(stderr, "Error with poll!\n%s\n", strerror(errno));
	  exit(1);
	}
      //else if (pollret == 0)
      //	exit(0);
      else if (pollret > 0)
	{
	  if(pollfds[0].revents & POLLIN) //read from keyboard input
	    {
	      memset(buffer, '\0', 256);
	      nBytes = read(0, buffer, 256);
	      if(nBytes < 0)
		{
		  fprintf(stderr, "Error with read!\n%s\n", strerror(errno));
		  exit(1);
		}
	      else if (nBytes == 0)
		exit(0);
		
	      if(isLog)
		{
		  if(!isCompress)
		    {
		      //char test;  //for bypassing unitialized var warnings :(
		      char msg[1000];//  = &test;
 		      sprintf(msg, "SENT %d bytes: %s\n", nBytes, buffer);
		      write(logFD, msg, strlen(msg)); //using 3 writes bc using all in one sprintf didnt work
				      //printf("strlen(msg): %ld\n", strlen(msg));
		      //		      write(logFD, buffer, nBytes);
		      // write(logFD, "\n", 1);
		    }
		}

	      if(isCompress)
		{		  
		  char compData[256]; //not sure how big needs to be - how good is compresssion?
		  int cret = myDef(buffer, nBytes, compData, 4000);
		  write(sockFD, compData, cret); //write to sockFD here not in for loop below
		  if(isLog)
		    {
		      //		      char test;
		      char msg[512]; // = &test;
		      sprintf(msg, "SENT %d bytes: %s\n", (int) cret, compData);
		      write(logFD, msg, strlen(msg));
		    }
		}


	      
	      for(i = 0; i < nBytes; i++)
		{
		  a = buffer[i];
		  switch (a)
		    {
		      /*		    case '\4':    //^D
					    if(close(pipeTo[1]) < 0)
					    {
					    fprintf(stderr, "Error with close! \n%s\n", strerror(errno));
					    exit(1);
					    }
					    break;
		      */
		      /*		    case '\3':    //^C
					    kill(cpid, SIGINT);
					    break;
		      */
		    case '\r':
		    case '\n':
		      if(write(1, "\r\n", 2) < 0) //||   //write to screen
			// write(pipeTo[1], "\n", 1) < 0)//write to server
			{
			  fprintf(stderr, "Error with write!\n%s\n", strerror(errno));
			  exit(1);
			}
		      if(!isCompress && (write(sockFD, "\n", 1) < 0))
			{
			  fprintf(stderr, "Error writing to server socket! Exiting...\n");
			  exit(1);
			}
		      break;
		    default:      //reg input
		      if(write(1, &a, 1) < 0 ) //|| write(pipeTo[1], &a, 1) < 0)
			{
			  fprintf(stderr, "Error writing to stdout!\n%s\n", strerror(errno));
			  exit(1);
			}
		      if(!isCompress && (write(sockFD, &a, 1) < 0)) //write to socket
			{
			  fprintf(stderr, "Error writing to socket!\n%s\nExiting...", strerror(errno));
			  exit(1);
			}
		    }
		}
	    }
	      
	  if(pollfds[1].revents & POLLIN) //read from socket
	    {
	      memset(buffer, '\0', 256);
	      nBytes = read(sockFD, buffer, 256);
	      if(nBytes < 0)
		{
		  fprintf(stderr, "Error with read!\n%s\n", strerror(errno));
		  exit(1);
		}

	      if(nBytes == 0)
		exit(0); //not sure maybe exit(1) or nothing at all 
		//break;
	      if(nBytes == 1 && buffer[0] == '\4')
		exit(0);
	      
	      if(isLog)
		{
		  //		  char test;
		  char  msg[1000]; // = &test;
		  sprintf(msg, "RECEIVED %d bytes: %s\n", nBytes, buffer);
                  write(logFD, msg, strlen(msg));
		}

	      if(isCompress)
		{
		  char decompData[4000];
		  int cret;
		  cret = myinf(buffer, nBytes, decompData, 4000);
		  char decompBuf[4100];
		  int index = 0;
		  for(i = 0; i < cret; i ++) //translate decompdata by replacing \n with \r\n
		    {
		      if(decompData[i] == '\n')
			{
			  decompBuf[index] = '\r';
			  decompBuf[index + 1] = '\n';
			  index = index + 2;
			}
		      else //regular conversion
			{
			  decompBuf[index] = decompData[i];
			  index++;
			}
		    }
		  if(write(1, decompBuf, index) < 0)
		    {
		      fprintf(stderr, "Error writing to stdout! Exiting...\n");
		      exit(1);
		    }
		}
	      else //if no compress
		{
 		  for(i = 0; i < nBytes; i++)
		    {
		      switch(buffer[i])
			{
			case '\n':
			  if(write(1, "\r\n", 2) < 0)
			    {
			      fprintf(stderr, "Error with write!\n%s\n", strerror(errno));
			      exit(1);
			    }
			  break;
			case '\4':
			  exit(0);
			  break;
			default:
			  if(write(1, &buffer[i], 1) < 0)
			    {
			      fprintf(stderr, "Error with write!\n%s\n", strerror(errno));
			      exit(1);
			    }
			}
		    }
		}
	    }

	  if(pollfds[1].revents & (POLLERR | POLLHUP)) //error 
	    {
	      //	      if(close(pipeFrom[0]) < 0)
	      //	fprintf(stderr, "Error closing\n%s\n", strerror(errno));
	      //fprintf(stderr, "Error with polling\n\r%s\n\r", strerror(errno));
	      exit(1);
	    }

	  if(pollfds[1].revents & POLLHUP) //error 
	    {
	      //	      if(close(pipeFrom[0]) < 0)
	      //	fprintf(stderr, "Error closing\n%s\n", strerror(errno));
	      //fprintf(stderr, "Error with polling\n\r%s\n\r", strerror(errno));
	      exit(0);
	    }

	}
    }
    
    
  //not sure if lines below are needed at all
  /*  
      char c[256];  
      while (1)
      {
      nBytes = read (0, &c, 256);
      for(i = 0; i < nBytes; i++)
      {
      switch(c[i])
      {
      case '\4':
      exit(0);
      break;
      case '\r':
      case '\n':
      write(1, "\r\n", 2);
      continue;
      default:
      write(1, &c[i], 1);
      break;
      }
      }
      }
  */  
  return 0;
}
