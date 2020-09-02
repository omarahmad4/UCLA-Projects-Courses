
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

int Foption = 0;
void FreeAndExit(char* a, char** b, int bsize, int exitN)
{
  int j;
  // int sizecharp = sizeof(char*);
  //  int sizecharpp = sizeof(char**);
  free (a);
  for (j = 0; j < bsize; j++)
    free (b[j]);
  free (b);
  if (exitN) exit(exitN);
}

char defrob(const char d){ return (d^42);}

int helpcmp(const void* a, const void* b)
{
  return frobcmp(*(char**) a, *(char**) b);
}

int frobcmp(char const* a, char const* b)  //define for qsort how to cmp
{
  while(1)
    {
      
      
      while(*a == '\0') a++;
      while(*b == '\0') b++;
      int aSpace = (*a == ' ');
      int bSpace = (*b == ' ');
      if(aSpace & !bSpace) return -1;
      if(bSpace & !aSpace) return 1;
      char derefA = *a;
      char derefB = *b;

      if(Foption)
	{
	  derefA = toupper(*a);
	  derefB = toupper(*b);
	}
      if(derefA != derefB) break;
      if(aSpace) return 0;
      a++;
      b++;
    }
  char aUnfrob;
  char bUnfrob;
  if(Foption)
    {
      aUnfrob = defrob(toupper(*a));
      bUnfrob = defrob(toupper(*b));
    }
  else
    {
      aUnfrob = defrob(*a);
      bUnfrob = defrob(*b);
    }
  if (aUnfrob < bUnfrob)
    return -1;
  else return 1;

}

void checkerror(char* a, char** b, int bsize, int exitN)
{
  if(ferror(stdin))
    {
      fprintf(stderr, "Error with I/O\n");
      FreeAndExit(a, b, bsize, exitN);
      exit(1);
    }
}

int main (int argc, char* argv[])
{

  if(argc > 2)
    {
      fprintf(stderr, "Error - Only 1 optional argument allowed (-f)\n");
      exit(1);
    }
  
  int IsFile, bufcount, bufmax;
  char* buf;
  bufcount = 0;
  bufmax = 0;
  Foption = 0;
  IsFile = 0;
  if(argc == 2)
    {
      if( strcmp("-f", argv[1]))
	{
	  fprintf(stderr, "Error - Only option -f allowed\n");
	  exit(1);
	}
      else
	Foption = 1;
      //    len = strlen(argv[1]);
    }

  struct stat fileData;
  if(fstat(0, &fileData) < 0)
    {
      fprintf(stderr, "Error with fstat\n");
      exit(1);
    }

  
  if(S_ISREG(fileData.st_mode))
    {
      IsFile = 1;
      bufmax = fileData.st_size;
      buf = (char*)malloc(bufmax*sizeof(char));
      if(buf == NULL)
	{
	  fprintf(stderr, "Error allocating memory\n");
	  exit(1);
	}
      if(read(0, buf, bufmax) < 0)
	{
	  fprintf(stderr, "Error reading file\n");
	  exit(1);
	}
    }
  char* chars;
  chars = (char*) malloc(sizeof(char));   //initial size of 20
  char** words;
  words = (char**) malloc(sizeof(char*));
  if (chars == NULL || words == NULL)
    {
      fprintf(stderr, "Error allocating memory");
      exit(1);
    }

  char nextchar;
  //  nextchar = getchar();
  //checkerror();

  int i = 0; //# letters in chars
  int j = 0; //# words in words
  int done = 1;

  int endfile = 0;

  while (done > 0)//(!endfile)
    {

      if(bufcount >= bufmax)
	{
	  done = read(0, &nextchar, 1);
	  checkerror(chars, words, j, 1);
	}
      else
	{
	  nextchar = buf[bufcount];
	  bufcount++;
	}
      if (nextchar == ' ')
        {
	  chars[i] = nextchar;

	  char** tempW = realloc(words, (j+2)*sizeof(char*));
          if(tempW != NULL && chars != NULL)
            {
	      words = tempW;
	      words[j] = chars;
	      j++;
	      chars = (char*)malloc(sizeof(char));
	      i=0;
	    }
	  else
            {
	      fprintf(stderr, "Error allocating memory");
	      FreeAndExit(chars, words, j, 1);
	      exit(1);
            }
	  if(!done)
	    {
	      break;
	    }
        }
      else if (nextchar == EOF)
        {
	  if(chars != NULL)
	    {
	      chars[i] = ' ';
	      words[j] = chars;
	      j++;
	    }
	  break;
        }
      else// if (nextchar != '\n')
        {
	  char* tempc = realloc(chars, (i+2)*sizeof(char));
	  if (tempc != NULL)
            {
	      chars = tempc;
	      chars[i] = nextchar;
	      i++;
	    }
	  else
            {
	      fprintf(stderr, "Error allocating memory");
	      FreeAndExit(chars, words, j, 1);
            }
        }
      if(done <= 0 && chars != NULL)
	{
	  chars[i-1] = ' ';
	  char** tempW = realloc(words, (j+1)*sizeof(char*));
          if(tempW != NULL && chars != NULL)
            {
	      words = tempW;
	      words[j] = chars;
	      j++;

	    }
	  else
            {
	      fprintf(stderr, "Error allocating memory");
	      FreeAndExit(chars, words, j, 1);
	      exit(1);
            }
	}
    }

  int a, k, l;
  qsort(words, j, sizeof(char*), helpcmp);

  for(k = 0; k < j; k++)
    {
      for(l = 0; words[k][l] != ' '; l++)
	{
	  write(1, &words[k][l], 1);
	}
      write(1, " ", 1);
    }
  for ( k = 0; k < j; k++)
    free (words[k]);
  free (words);
  return 0;
}
