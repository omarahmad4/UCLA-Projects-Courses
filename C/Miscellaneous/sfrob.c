#include <stdio.h>
#include <stdlib.h>

void FreeAndExit(char* a, char** b, int exitN)
{
    int i, j;
    int sizecharp = sizeof(char*);
    int sizecharpp = sizeof(char**);
    //    for (i = 0; i < sizeof(a)/sizecharp; i++)
    //    free (a[i]);
    free (a);
    // for (j = 0; j < sizeof(b)/sizecharpp; j++)
    //    free (b[j]);
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
        while(*a =='\0') a++;
        while(*b =='\0') b++;
        int aSpace = (*a == ' ');
        int bSpace = (*b == ' ');
        if(aSpace & !bSpace) return -1;
        if(bSpace & !aSpace) return 1;
        if(*a != *b) break;
        if(aSpace) return 0;
        a++;
        b++;
    }

    char aUnfrob = defrob(*a);
    char bUnfrob = defrob(*b);
    if (aUnfrob < bUnfrob)
        return -1;
    else return 1;

}

void checkerror()
{
    if(ferror(stdin))
    {
        fprintf(stderr, "Error with I/O");
        exit(1);
    }
}



int main ()
{
    char* chars;
    chars = (char*) malloc(sizeof(char));   //initial size of 20
    char** words;
    words = (char**) malloc(sizeof(char*));
    if (chars == NULL | words == NULL)
    {
        fprintf(stderr, "Error allocating memory");
        exit(1);
    }

    chars[0] = getchar();
    checkerror();
    char nextchar = getchar();
    checkerror();


    int i = 1; //# letters in chars
    int j = 0; //# words in words
    int endfile = feof(stdin);
    while (!endfile)// || nextchar != EOF)// && nextchar != EOF && !ferror(stdin)) // || chars[i] != 'Q')
    {
        if (nextchar == ' ')
        {

            char** tempW = realloc(words, (j+2)*sizeof(char*));
            if(tempW != NULL)
            {
             words = tempW;
             chars[i] = nextchar;
             words[j] = chars;
             j++;
             chars = NULL;
             chars = (char*)malloc(sizeof(char));
             i = 0;
            }
            else
            {
                fprintf(stderr, "Error allocating memory");
                FreeAndExit(chars, words, 1);
                exit(1);
            }

            while (nextchar == ' ')
            {
                nextchar = getchar();
                checkerror();
            }
            if (!endfile) endfile = feof(stdin);
        }
        else if (nextchar == EOF)
        {
            if(chars != NULL)
                {
                    chars[i] = ' ';
                    words[j] = chars;
                    j++;

                }
                endfile = 1;
                break;
        }
        else
        {
            char* tempc = realloc(chars, (i+1)*sizeof(char));
            if (tempc != NULL)
            {
                chars = tempc;
            }
            else
            {
                fprintf(stderr, "Error allocating memory");
                FreeAndExit(chars, words, 1);
            }
        }

    if(endfile) break;
    chars[i] = nextchar;
    nextchar = getchar();
    checkerror();
    i++;
    }

    int a, k, l;
    qsort(words, j, sizeof(char*), helpcmp);

    for(k = 0; k< j; k++)
    {
         for(l = 0; words[k][l] != ' '; l++)
            {
                printf("%c", words[k][l]);
            }
            printf(" ");
    }

       FreeAndExit(chars, words, 0);

      return 0;
}
