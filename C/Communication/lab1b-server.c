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
#include <sys/types.h>   //socket, conenct, listen, shutdown, accept
#include <sys/socket.h> 
#include <netdb.h>       //for gethostbyname
#include "zlib.h"        //compression



int errno, status, cpid, clientFD; //global bc used in multiple functions
int pipeTo[2];  //to shell (from parent) (parent = server)
int pipeFrom[2];//from shell (to parent) (parent = server)


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


void createPipe(int fd[2])
{
  if(pipe(fd) == -1)
    {
      fprintf(stderr, "ERROR creating pipe! Exiting...\n");
      exit(1);
    }
}


/* Use this variable to remember original terminal attributes. */
struct termios saved_attributes;

void reset_input_mode (void)
{
  tcsetattr (STDIN_FILENO, TCSANOW, &saved_attributes);
  
  if(waitpid(cpid, &status, 0) < 0 )
    {
      fprintf(stderr, "Error with waitpid! Exiting...\n");
      exit(1);
    }
  if (WIFEXITED(status)) //if child terminated normally 
    {
      fprintf(stderr, "SHELL EXIT SIGNAL=%d STATUS=%d\n", WTERMSIG(status), WEXITSTATUS(status));
      exit(0);
    }
  fprintf(stderr, "SHELL EXIT SIGNAL=%d STATUS=%d\n", WTERMSIG(status), WEXITSTATUS(status));
  write(clientFD, "\4", 1);
		     
  exit(1);
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
     {"compress", no_argument, NULL, 'c'},
     {      0, 0,    0,   0}
    };
  int option, port, isExit, isCompress;
  port = 0;
  isExit = 0;
  isCompress = 0;
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

  int sockFD;
  sockFD = socket(AF_INET, SOCK_STREAM, 0);
  if(sockFD == -1)
    {
      fprintf(stderr, "Error creating socket!\n%s\nExiting...", strerror(errno));
      exit(1);
    }
  
  struct hostent * host; //(server)
  host = gethostbyname("localhost");
  unsigned int hostLen = host->h_length;

  struct sockaddr_in mySockAddr;
  memset( &mySockAddr, 0, sizeof(mySockAddr)); //from ex - sets to 0 - not sure if needed
  mySockAddr.sin_family = AF_INET;
  mySockAddr.sin_port = htons(port);
  mySockAddr.sin_addr.s_addr = atoi(host->h_addr); //"localhost";//what addr goes here??
  
  if(bind(sockFD, (struct sockaddr *) &mySockAddr, sizeof(mySockAddr)) < 0 )
    {
      fprintf(stderr, "Error binding to socket! Exiting...\n");
      exit(1);
    }
  
  if(listen(sockFD, 5) < 0 )
    {
      fprintf(stderr, "Error listening to socket! Exiting...\n");
      exit(1);
    }


  clientFD = accept(sockFD, (struct sockaddr *) &mySockAddr, &hostLen); //clientFD = fd of connected socket 

  if(clientFD == -1)
    {
      fprintf(stderr, "Error accepting connection!\n%s\nExiting...", strerror(errno));
      exit(1);
    }

  //  close(0);
  //dup(clientFD);
  
  //  set_input_mode (); //needed?? in sever -- yes / client -- idk

  createPipe(pipeTo);
  createPipe(pipeFrom);
  
  int nBytes, i;

  cpid = fork();
  if(cpid < 0)
    {
      fprintf(stderr, "Error with forking! Exiting...\n");
      exit(1);
    }
  else if (cpid == 0) //child (shell)
    {
      //      close(pipeTo[1]);
      // close(pipeFrom[0]);
      close(0);      //stdin
      dup(pipeTo[0]);
      close(pipeTo[0]);
      close(1);      //stdout
      dup(pipeFrom[1]);
      close(2);      //stderr
      dup(pipeFrom[1]);
      close(pipeFrom[1]);

      if (execl("/bin/bash", "shell", (char*) NULL) == -1)
	{
	  fprintf(stderr, "Error with shell!\n %s\n", strerror(errno));
	  write(clientFD, "\4", 1);
	  exit(1);
	}
    }
  else                //parent
    {
      close(pipeFrom[1]);  //close bc not using these two
      close(pipeTo[0]);

      struct pollfd pollfds[2];
      pollfds[0].fd =  clientFD; //0; changed from 0 (stdin) to new clientfd from accept call
      pollfds[0].events = POLLIN | POLLERR | POLLHUP;
      pollfds[1].fd = pipeFrom[0];
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
	  else if (pollret > 0)
	    {
	      if(pollfds[0].revents & POLLIN) //read from socket input
		{
		  memset(buffer,'\0', 256);
		  nBytes = read(clientFD, buffer, 256);
		  if(nBytes < 0)
		    {
		      fprintf(stderr, "Error with read!\n%s\n", strerror(errno));
		      exit(1);
		    }
		  else if (nBytes == 0) //EOF??
		    {
		      write(clientFD, "\4", 1);
		     
		      exit(0);
		      //		      close(pipeTo[1]);
		      //printf("nBytes == 0!! from socket input\n");
		     
		    }
		  else if (nBytes == 1 && buffer[0] == '\4')
		    {
		      write(clientFD, "\4", 1);
		     
		      exit(0);

		    }
		    
		  if(isCompress)
		    {
		      char decompData[4000];
		      // char dBuf[5000];
		      //int index = 0;
		      int cret = myinf(buffer, nBytes, decompData, 4000);
		      for(i = 0; i < cret; i++)
			{
			  a = decompData[i];
			  switch(a)
			    {
			    case '\4': //^D
			      //			      shutdown(clientFD, SHUT_RDWR);
			      write(pipeTo[1], &a, 1);
			      write(clientFD, "\4", 1);
			      exit(0);
			      //			       close(pipeTo[1]);
			      break;
			    case '\3': //^C
			      kill(cpid, SIGINT);
			      break;
			    case '\r':
                            case '\n':
                              if((write(1, "\r\n", 2) < 0) ||   //write to screen
				 (write(pipeTo[1], "\n", 1) < 0))//write to shell
                                {
                                  fprintf(stderr, "Error with write!\n%s\n", strerror(errno));
                                  exit(1);
                                }
			      //     dBuf[index] = '\n';
			      // index++;
                              break;
                            default:      //reg input
                              if(write(        1, &a, 1) < 0 ||
				 write(pipeTo[1], &a, 1) < 0)
                                {
                                  fprintf(stderr, "Error with write!\n%s\n", strerror(errno));
                                  exit(1);
                                }
			      // dBuf[index] = a;
			      //index++;
			    }
			}

		      /*		      if(write(pipeTo[1], &dBuf, index) < 0)
					      {
					      fprintf(stderr, "ERROR writing to shell! Exiting...\n");
					      exit(1);
					      }
		      */

		      
		      //		      if(cret == 0)
		      //	close(pipeTo[1]);
 		    }
		  else //if no compress
		    {
		      char scBuf[512];
		      char shBuf[512];
		      int scI = 0;
		      int shI = 0;
		      
		      for(i = 0; i < nBytes; i++)
			{
			  a = buffer[i];
			  switch (a)
			    {
			    case '\4':    //^D
			      if(close(pipeTo[1]) < 0)
				{
				  fprintf(stderr, "Error with close! \n%s\n", strerror(errno));
				  exit(1);
				}
			      write(clientFD, "\4", 1);			      

			      //		      exit(1);
			      break;
			    case '\3':    //^C
			      kill(cpid, SIGINT);
			      //			      exit(1); //IDK
			      break;
			    case '\r':
			    case '\n':
			      scBuf[scI] = '\r';
			      scBuf[scI + 1] = '\n';
			      scI += 2;
			      shBuf[shI] = '\n';
			      shI++;
			      /*
				if(write(1, "\r\n", 2) < 0 ||   //write to screen
				write(pipeTo[1], "\n", 1) < 0)//write to shell
				{
				fprintf(stderr, "Error with write!\n%s\n", strerror(errno));
				exit(1);
				}
			      */
			      break;
			    default:      //reg input
			      scBuf[scI] = a;
			      scI++;
			      shBuf[shI] = a;
			      shI++;
			      /*
				if(write(        1, &a, 1) < 0 ||
				write(pipeTo[1], &a, 1) < 0)
				{
				fprintf(stderr, "Error with write!\n%s\n", strerror(errno));
				exit(1);
				}
			      */
			    }

			}
		      if(write(1, scBuf, scI) < 0 || write(pipeTo[1], shBuf, shI) < 0)
			{
			  fprintf(stderr, "Error writing to screen or shell! Exiting...\n");
			  exit(1);
			}
		    }
		}
	      
	      if(pollfds[1].revents & POLLIN) //read from shell
		{
		  memset(buffer, '\0', 256);
		  nBytes = read(pipeFrom[0], buffer, 256);
		  if(nBytes < 0)
		    {
		      fprintf(stderr, "Error with read!\n%s\n", strerror(errno));
		      exit(1);
		    }
		  if(nBytes == 0) //EOF?
		    {
		      printf("nBytes == 0 from shell\n");
		      //PRINT SHELL EXIT STATUS SOMEHOW
		      write(clientFD, "\0", 1); //delete?

		      write(clientFD, "\4", 1);
		      shutdown(clientFD, SHUT_RDWR);
		      exit(1);
		    }

		  // else if (nBytes == 1 && buffer[0] == '\4')
		  //  exit(0);


		  if(isCompress)
		    {
		        
		      char compData[256]; //compressed Data
		      int cret = myDef(buffer, nBytes, compData, 256);
		      if(write(clientFD, compData, cret) < 0)
			{
			  fprintf(stderr, "Error writing to client through socket!\n");
			  exit(1);
			}
		    }
		  else //if no compress
		    {
		      char shBuf[512];
		      int index = 0;
		      for(i = 0; i < nBytes; i++)
			{
			  a=buffer[i];
			  switch(a)
			    {
			    case '\n':
			      /*
				if(write(clientFD, "\r\n", 2) < 0)
				{
				fprintf(stderr, "Error with write!\n%s\n", strerror(errno));
				exit(1);
				}
			      */
			      shBuf[index] = '\r';
			      shBuf[index + 1] = '\n';  
			      index = index + 2;
			      break;
			    default:
			      /*
				if(write(clientFD, &buffer[i], 1) < 0)
				{
				fprintf(stderr, "Error with write!\n%s\n", strerror(errno));
				exit(1);
				}
			      */
			      shBuf[index] = a;
			      index++;
			      break;
			    }
			}
		      if(write(clientFD, &shBuf, index) < 0)
			{
			  fprintf(stderr, "Error writing shell output to client! Exiting...\n");
			  exit(1);
			}
		      else
			printf("TEST %s\n", shBuf);
		    }
			 
		  /*
		    if(pollfds[0].revents & (POLLERR | POLLHUP))//NOT SURE IF NEEDED
		    close(pipeTo[1];
		  */
	      
		  if(pollfds[1].revents & POLLERR) //error 
		    {
		      if(close(pipeFrom[0]) < 0)
			fprintf(stderr, "Error closing\n%s\n", strerror(errno));
		      //		      fprintf(stderr, "Error with polling\n\r%s\n\r", strerror(errno));

		      write(clientFD, "\4", 1);
		      exit(1);
		    }

		  
		  if(pollfds[1].revents &  POLLHUP) //error 
		    {
		      if(close(pipeFrom[0]) < 0)
			fprintf(stderr, "Error closing\n%s\n", strerror(errno));
		      //fprintf(stderr, "Error with polling\n\r%s\n\r", strerror(errno));
		      write(clientFD, "\4", 1);
		      
		      exit(0);
		    }
	
		}
	    }
	}
    }    

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

