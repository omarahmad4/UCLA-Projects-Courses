//NAME: Omar Ahmad
//EMAIL: omarahmad4@ucla.edu
//ID: 804801272

#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <mraa.h>
#include <mraa/gpio.h>
#include <mraa/aio.h>
#include <getopt.h>
#include <errno.h>
#include <poll.h>
#include <time.h>
#include <math.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <termios.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include "fcntl.h"

int errno, status, i;
int power = 1; //on/off status
int isLog = 0;
int logFD = -1;  //creating/opening log file
int sockFD;
double period = 1;
char scale ='F';
time_t Time;
struct tm * myTime;

int id = -1;
int portnum;
char hostname[128];
char usageMsg[] = "Usage: ./lab4c_tcp [--log=logfile] [--period=(period in seconds)] [--scale=(F or C)] [--id=ID] [--host=hostname] [port number]";


void errorExit(char errmsg[], int code, int doErrno)
{
  if(doErrno)
    fprintf(stderr, "Error: %s\nErrno: %s\n", errmsg, strerror(errno));
  else
    fprintf(stderr, "Error: %s\n", errmsg);
  if(code)
    {
      fprintf(stderr, "Exiting...\n");
      exit(code);
    }
}
/* Use this variable to remember original terminal attributes. */
struct termios saved_attributes;

void reset_input_mode (void)
{
  tcsetattr (STDIN_FILENO, TCSANOW, &saved_attributes);
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
    errorExit("Not a terminal", 1, 0);

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


void doShutdown()
{
  time(&Time);
  myTime = localtime(&Time); 

  int hr  = myTime->tm_hour;
  int min = myTime->tm_min;
  int sec = myTime->tm_sec;
  char msg [128];
  sprintf(msg, "%.2i:%.2i:%.2i SHUTDOWN\n", hr, min, sec);
  write(logFD, msg, strlen(msg));
  dprintf(sockFD, "%s", msg);

  exit(0);
}

void checkCommand(char * inp)
{
  int inpLen = strlen(inp);
  if(memcmp(inp, "START", 5) == 0)       //start command
    {
      write(logFD, "START\n", 6);
      //dprintf(sockFD, "START\n");
      power = 1;
      inp = inp + 5; //NOT SURE IF WILL WORK
      return;
    }
  else if (memcmp(inp, "STOP", 4) == 0)  //stop command
    {
      write(logFD, "STOP\n", 5);
      //dprintf(sockFD, "STOP\n");
      power = 0;
      inp = inp + 4;
      return;
    }
  else if (memcmp(inp, "SCALE=", 6) == 0) //scale command
    { 
      if(inpLen == 6)
	return;
      else 
	{
	  if(inp[6] == 'C' || inp[6] == 'F') //only update if valid
	    scale = inp[6];  
	  char logmsg [31];
	  sprintf(logmsg, "SCALE=%c\n", inp[6]);
	  write(logFD, logmsg, strlen(logmsg));

	  //dprintf(sockFD, "SCALE=%c\n", inp[6]);
	  inp = inp + 7;
	  return;
	}
    }
  else if (memcmp(inp, "LOG", 3) == 0)
    {
      if(inpLen == 3)
	return; //do nothing when log without anything after bc message will come next round ??
      else
	{
	  write(logFD, inp, inpLen); //might have to add newline 
	  //dprintf(sockFD, "%s", inp);
	  inp = inp + inpLen;

	}
    }
  else if (memcmp(inp, "OFF", 3) == 0)
    {
      write(logFD, inp, inpLen); //might have to add newline 
      //dprintf(sockFD, "%s", inp);
      doShutdown();
    }
  else if (memcmp(inp, "PERIOD=", 7) == 0)
    {
      if(inpLen == 7)
	return;
      else
	{
	  inp = inp + 7;
	  double newPer = atof(inp); 
	  if(newPer > 0)
	    period = newPer;
	  char logmsg [31];
	  sprintf(logmsg, "PERIOD=%s", inp);
	  write(logFD, logmsg, strlen(logmsg));
	  //dprintf(sockFD, "PERIOD=%s", inp);
	  memset(inp, '\0', 128);    
	}
    }
  
  return;
}


int main(int argc, char **argv)
{
  static struct option optData[] =
    {

      {"period", required_argument, NULL, 'p'},
      {"scale", required_argument, NULL, 's'},
      {"id", required_argument, NULL, 'i'},     //required param
      {"host", required_argument, NULL, 'h'},   //required param
      {"log", required_argument, NULL, 'l'},    //required param    
      {0,0,0,0}
    };

  char logfile [255];
  int option;
  int isArg = 1;
  int isID = 0;
  int isHost = 0;
  int isPort = 0;

  
  while (isArg)
    {
      option = getopt_long(argc, argv, "l:p:s:i:h:", optData, NULL);
      switch(option)
	{
	case 'l':
	  strncpy(logfile, optarg, 255);
	  isLog = 1;
	  break;
	case 'p':
	  period = atof(optarg);
	  if(period <= 0)
	    errorExit("Period must be positive", 1, 0);
	  break;
	case 's':
	  if(strlen(optarg) == 1 && (optarg[0] == 'F' || optarg[0] == 'C' ))
	    scale = optarg[0];
	  else
	    errorExit("Scale takes 'F' or 'C'", 1, 0);
	  break;
	case 'i':
	  if(strlen(optarg) != 9)
	    errorExit("--id takes only 9 digit ID's", 1, 0);
	  id = atoi(optarg);
	  isID=1;	  
	  break;
	case 'h':
	  strncpy(hostname, optarg, 128);
	  isHost=1;	  
	  break;
	case -1:
	  isArg = 0;
	  break;
	default:
	  errorExit(usageMsg, 1, 0);
	  break;
	}
    }

  int j = 0;
  for(i = optind; i < argc; i++, j++)
    {
      if(j > 0)
	errorExit("Only one non-option argument allowed: port", 1, 0);
      portnum = atoi(argv[i]);
      isPort = 1;
    }

  if(!isLog || !isID || !isHost || !isPort)
    {
      errorExit("log, id, host, and port are mandatory parameters", 0, 0);
      errorExit(usageMsg, 1, 0);
    }
  
  logFD = open(logfile, O_RDWR | O_APPEND | O_CREAT, 0666); //appends logfile if exists (trunc to delete)
  if(logFD == -1)
    errorExit("opening logfile", 1, 0);

  //IDK IF NEEDED 
  //  set_input_mode (); //after getopt and opening lofile


  sockFD = socket(AF_INET, SOCK_STREAM, 0);
  if(sockFD == -1)
    errorExit("creating socket", 1, 1);

  struct hostent * host; //(server)
  host = gethostbyname(hostname);
  if(host == NULL)
    errorExit("getting host name of server", 1, 0);
  //int hostLen = host->h_length;
  //  sockFD.sin_port=htons(port);

  struct sockaddr_in mySockAddr;
  memset(&mySockAddr, 0 , sizeof(mySockAddr));
  mySockAddr.sin_family = AF_INET;
  mySockAddr.sin_port = htons(portnum);
  //  mySockAddr.sin_addr.s_addr = atoi(host->h_addr); //"localhost";//what addr goes here??


  bcopy(host->h_addr, &mySockAddr.sin_addr.s_addr, host->h_length);
  
  if(connect(sockFD, (struct sockaddr *) &mySockAddr, sizeof(mySockAddr)) < 0 ) //hostlen for last param?
    errorExit("connecting to socket\r", 1, 0);

  
  dprintf(sockFD, "ID=%d\n", id);
  dprintf(logFD, "ID=%d\n", id);




  
  mraa_aio_context temperature;
  temperature = mraa_aio_init(1); //AIN0
  int temp;                       //temp value
  
  const int R0 = 100000;          //100k ohms
  const int B = 4275;

  int pollret;
  struct pollfd pollfds[1];
  pollfds[0].fd = sockFD;
  pollfds[0].events = POLLIN | POLLHUP | POLLERR;

  char msg [100];
  float finTemp;
  float R;

  int hr, min, sec;
  char retBuf[128];
  struct timespec start , end;

  while(1)
    {
      time(&Time);
      myTime = localtime(&Time);

      if(power)    // && (myTime->tm_sec >= next))
	{
	  temp = mraa_aio_read(temperature);

	  R = 1023.0/(float)temp - 1.0;
	  R = R*R0;
	  finTemp = 1.0/(log((R/R0))/B+1/298.15)-273.15; //final temp  = temp in C (right now)
	  if(scale == 'F')
	    finTemp = finTemp * 9/5 + 32;            //final temp  in Fahrenheit

	  hr  = myTime->tm_hour;
	  min = myTime->tm_min;
	  sec = myTime->tm_sec;
	  memset(msg, 0, strlen(msg));
	  sprintf(msg, "%.2i:%.2i:%.2i %.1f\n", hr, min, sec, finTemp);
	  dprintf(sockFD, "%s", msg);
	  write(logFD, msg, strlen(msg));
	  
	}
      
      clock_gettime(CLOCK_MONOTONIC, &start);
      clock_gettime(CLOCK_MONOTONIC, &end);
      //spend all extra time scanning for input - most consistent method of spending period away from output
      while(end.tv_sec - start.tv_sec + (double)(end.tv_nsec - start.tv_nsec)/1000000000 < period)
	{
	  pollret = poll(pollfds, 1, 0);
	  if(pollret < 0)
	    errorExit("polling", 1, 0);
	  else
	    {
	      if(pollfds[0].revents & POLLIN)
		{
		  memset(retBuf, '\0', 128);
		  //fgets(retBuf, 128, sockFD);
		  read(sockFD, retBuf, 128);
		  checkCommand(retBuf);
		}
	    }


	  clock_gettime(CLOCK_MONOTONIC, &end);
	}
    }

  mraa_aio_close(temperature);
  return 0;  
}
