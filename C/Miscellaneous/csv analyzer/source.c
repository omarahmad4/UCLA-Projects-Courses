//NAME: Omar Ahmad
//EMAIL: omarahmad4@ucla.edu
//ID: 804801272

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <fcntl.h>
#include <inttypes.h>
#include <time.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>
#include <stdbool.h>

#define MINBLOCKSIZE 1024

int errno;
char buf [128];

struct block_Data
{
  int iNum;
  int OFFSET;
  int indirection; //0 = none, 1 = indirect, 2 = double, 3 = triple
  int uses;
  bool free;
  struct block_Data * duplicate; //if any duplicates
};

struct inode_Data
{
  bool free;
  int uses;
  int links;
  int foundLinks;
};

void errorExit(char errmsg[], int code, int doErrno)
{
  if(doErrno)
    fprintf(stderr, "Error: %s\nErrno: %s\tExiting...\n", errmsg, strerror(errno));
  else
    fprintf(stderr, "Error: %s\nExiting...\n", errmsg);
  if(code)
    exit(code);
}

char * getField(char* line, int n)
{
  int j = 1;
  char comma = ',';
  char newline = '\n';
  memset(buf, '\0', strlen(buf));
  int len = strlen(line);
  int k = 0;
  for (k = 0; k < len; k++)
    {
      if(! strncmp(&line[k],&newline,1))
	break;
      if(! strncmp(&line[k],&comma, 1))
	{
	  j++;
	  continue;
	}
      if(j == n)
        strncat(buf, &line[k],1);
      if(j > n)
	break;
    }
  return buf;
}

int main(int argc, char **argv)
{
  if (argc != 2)
    errorExit("Must be called with exactly one argument\nUsage: ./lab3b [CSV data]", 1, 0);

  const char * fileName;
  fileName = argv[1];
  int len = strlen(fileName);
  char * csvTag = ".csv";

  if(strncmp(&fileName[len - 4], csvTag,4)) //returns 0 for equal
    errorExit("paramaeter must be .csv file!\nUsage: ./lab3b [CSV data]", 1, 0);

  FILE * myFile = fopen(fileName, "r");
  if(myFile == NULL)
    errorExit("opening file", 1, 1);



  char indir[4][31] =  { "", "INDIRECT ", "DOUBLE INDIRECT ", "TRIPLE INDIRECT "};
  char line[1024];
  
  int  i, numBlocks, numInodes, blockNum, inodeNum;
  bool foundError = false;

  struct block_Data bData[63];
  struct inode_Data iData[24];

  for(i = 0; i<64; i++) //initializing
    {
      bData[i].iNum = 0;
      bData[i].OFFSET = 0;
      bData[i].uses = 0;
      bData[i].free = false;
      bData[i].duplicate = NULL;
      bData[i].indirection = 0;
    }
  for(i = 0; i < 24; i ++)
    {
      iData[i].free = false;
      iData[i].uses = 0;
      iData[i].links = 0;
      iData[i].foundLinks = 0;
    }
  
  while(fgets(line, 1024, myFile)) //reading in all lines
    {
      if(! strcmp(getField(line, 1), "SUPERBLOCK"))
	{
	  numBlocks = atoi(getField(line, 2));
	  numInodes = atoi(getField(line, 3));
	  //blockSize = atoi(getField(line, 4));
	  //inodeSize = atoi(getField(line, 5));
	  //firstInode = atoi(getField(line, 8));
	}
      else if (! strcmp(getField(line, 1), "GROUP"))
	{
	  //	  printf("found group\n");
	}
      
      else if(! strcmp(getField(line, 1), "BFREE")) 
	{
	  blockNum = atoi(getField(line, 2));
	  bData[blockNum].free = true;           //will check after done reading in all lines
	}
      else if(! strcmp(getField(line, 1), "IFREE"))
	{
	  inodeNum = atoi(getField(line, 2));
	  iData[inodeNum].free = true;
	}
      else if(! strcmp(getField(line, 1), "INODE"))
	{
	  inodeNum = atoi(getField(line, 2));
	  iData[inodeNum].uses++;
	  iData[inodeNum].links = atoi(getField(line, 7));
	  	  
	  int offset;
	  char tag [31];
	  sprintf(tag, "BLOCK");
	  int ind = 0;
	  for(i = 13; i <= 27; i++)
	    {
	      blockNum = atoi(getField(line, i));
	      offset = i-13;
	      if(offset == 12)
		{
		  sprintf(tag, "INDIRECT BLOCK");
		  ind = 1;
		}
	      else if (offset == 13)
		{
		  offset += 255;
		  sprintf(tag, "DOUBLE INDIRECT BLOCK");
		  ind = 2;
		}
	      else if (offset == 14)
		{
		  offset += 65790;
		  sprintf(tag, "TRIPLE INDIRECT BLOCK");
		  ind = 3;
		}
	      else
		ind = 0;
	      
	      if(blockNum > 0 && blockNum < 63)
		{
		  if(bData[blockNum].iNum != 0) //already a block's data in there
		    {
		      struct block_Data next;
		      struct block_Data * ptr = &bData[blockNum]; //.duplicate;
		      
		      while(ptr->duplicate != NULL)
			{
			  ptr = ptr->duplicate;
			}
		      
		      ptr->duplicate = &next;
		      next.iNum = inodeNum;
		      next.OFFSET = offset;
		      next.indirection = ind;
		      next.duplicate = NULL;
		    }
		  else
		    {
		      bData[blockNum].iNum = inodeNum;
		      bData[blockNum].OFFSET = offset;
		      bData[blockNum].indirection = ind;
		    }
		  
		}
	      
	      if (blockNum < 0 || blockNum > numBlocks) //INVALID
		{
		  foundError = true;
		  printf("INVALID %s %d IN INODE %d AT OFFSET %d\n", tag, blockNum, inodeNum, offset);
		}
	      else if(blockNum != 0 && blockNum < 8)
		{
		  foundError = true;
		  printf("RESERVED %s %d IN INODE %d AT OFFSET %d\n", tag, blockNum, inodeNum, offset); 
		}
	      else
		bData[blockNum].uses++;
	    }
	}
      else if(! strcmp(getField(line, 1), "DIRENT"))
	{
	  int iNUM = atoi(getField(line, 2));
  	  int ref = atoi(getField(line, 4));
	  if(iData[ref].free) //unalloc error
	    {
	      foundError = true;
	      if( strcmp(getField(line, 7), "'..'") &&  strcmp(getField(line, 7), "'.'"))
		printf("DIRECTORY INODE %d NAME %s UNALLOCATED INODE %d\n", iNUM, getField(line, 7), ref);
	      else
	      	iData[ref].foundLinks++;
	    }
	  else if( ref > numInodes)
	    {
	      foundError = true;
	      printf("DIRECTORY INODE %d NAME %s INVALID INODE %d\n", iNUM, getField(line, 7), ref);
	    }
	  else
	    iData[ref].foundLinks++;

	  if( !strcmp(getField(line, 7), "'..'"))
	    {
	      int linkN = atoi(getField(line, 4));
	      if(linkN != 2)
		{
		  foundError = true;
		  printf("DIRECTORY INODE %d NAME %s LINK TO INODE %d SHOULD BE %d\n",
			 iNUM, getField(line, 7), linkN, iNUM);
		}
	    }
	  else if  ( !strcmp(getField(line, 7), "'.'"))
	    {
	      int linkN = atoi(getField(line, 4));
	      if(linkN != iNUM)
		{
		  foundError = true;
		  printf("DIRECTORY INODE %d NAME %s LINK TO INODE %d SHOULD BE %d\n",
			 iNUM, getField(line, 7), linkN, iNUM);
		}
	    }
	}
      else if(! strcmp(getField(line, 1), "INDIRECT"))
	{
	  blockNum = atoi(getField(line, 5));
	  if(bData[blockNum].uses == 0)
	    bData[blockNum].uses++;
	  
	  blockNum = atoi(getField(line, 6));
	  if(bData[blockNum].uses == 0)
	    bData[blockNum].uses++;
	}
    } //end while - no more lines coming in

  for (i = 8; i < 63; i ++)
    {
      if(bData[i].uses != 0 && bData[i].free)
	{
	  foundError = true;
	  printf("ALLOCATED BLOCK %d ON FREELIST\n", i);
	}
      else if (bData[i].uses == 0 && !bData[i].free)
	{
	  foundError = true;
	  printf("UNREFERENCED BLOCK %d\n", i);
	}
      if(bData[i].uses > 1)
	{
	  struct block_Data * ptr = &bData[i]; 
	  foundError = true;

	  do
	    {
	      printf("DUPLICATE %sBLOCK %d IN INODE %d AT OFFSET %d\n",
		     indir[ptr->indirection], i, ptr->iNum, ptr->OFFSET);	      
	      ptr = ptr->duplicate;
	    }
	  while(ptr != NULL);
	}
    }
  for(i = 2; i < 24; i++)
    {
      if(iData[i].links != iData[i].foundLinks)
	{
	  foundError = true;
	  printf("INODE %d HAS %d LINKS BUT LINKCOUNT IS %d\n", i, iData[i].foundLinks, iData[i].links);
	}
      if(i > 2 && i < 11)
	continue;
      if(iData[i].uses != 0 && iData[i].free)
	{
	  foundError = true;
	  printf("ALLOCATED INODE %d ON FREELIST\n", i);
	}
      else if (iData[i].uses == 0 && !iData[i].free)
	{
	  foundError = true;
	  printf("UNALLOCATED INODE %d NOT ON FREELIST\n", i);
	}
    }

  if(foundError)
    return 2;
  else
    return 0;
}

