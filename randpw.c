#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#include "nomes.h"

#ifndef SEEK_END
#define SEEK_END 2
#endif
#ifndef SEEK_SET
#define SEEK_SET 0
#endif

#define PWLEN 8
#define BUFSIZE 8192

#define PWFILE "/usr/local/etc/passwds.dat"
#if 1
#define RNDFILE "/var/run/randpw.rnd"
#else
#define RNDFILE "randpw.rnd"
#endif

char *replaceable="behjloqsxz";
char *replacement="364/!095#*";
char *uppercaseable="abcdefghjklmnopqrstuvwxyz";

void randomize()
{
  FILE *f;
  unsigned long seed=time(NULL);
  static char inbuf[BUFSIZE];
  if ((f=fopen(RNDFILE, "r"))!=NULL)
    {
      fgets(inbuf, BUFSIZE, f);
      fclose(f);
      sscanf(inbuf, "%li", &seed);
    }
  srandom(seed);
}

#define NomeSize 4

typedef char Nome[NomeSize];

Nome *quadnlist;
Nome *startlist;
int quadnum;
int startnum;

int init_randpw()
{
  quadnum=strlen(nomes)/NomeSize;
  quadnlist=(Nome *)malloc(quadnum*NomeSize);
  memcpy((char*)quadnlist, nomes, quadnum*NomeSize);
  startnum=strlen(wstart)/NomeSize;
  startlist=(Nome *)malloc(startnum*NomeSize);
  memcpy((char*)startlist, wstart, startnum*NomeSize);
  randomize();
  return 1;
}

void term_randpw()
{
  FILE *f;
  if ((f=fopen(RNDFILE, "w"))!=NULL)
    {
      fprintf(f, "%li\n", random());
      fclose(f);
    }
  else
    perror(RNDFILE);
  free(quadnlist);
}

int do_randpw(char *pw)
{
  int i,j,k,ready,rtimeout;
  k=random()%startnum;
  strncpy(pw,startlist[k],NomeSize);
  pw[PWLEN]='\0';
  i=NomeSize;
  ready=0;
  rtimeout=0;
  while(!ready)
    {
      while(i<PWLEN)
	{
	  j=0;
	  k=random()%quadnum;
	  while((strncmp(&pw[i-(NomeSize-1)],quadnlist[k],(NomeSize-1)))&&((j++)<5000))
	    k=random()%quadnum;
	  if (j>=5000)
	    {
	      if ((rtimeout++)>10000)
		{
		  fprintf(stderr,"do_randpw() failed.\n");
		  return 0;
		}
	      k=random()%startnum;
	      strncpy(pw,startlist[k],NomeSize);
	      i=NomeSize;
	    }
	  else
	    pw[i++]=quadnlist[k][(NomeSize-1)];
	}
	ready=1;
    }
  return 1;
}

int randpw(char *pw)
{
  init_randpw();
  if (!do_randpw(pw))
    return 0;
  term_randpw();
  return 1;
}

int mungepw(char *pw)
{
  int j;
  char *c;
  for (j=0;j<strlen(pw);j++)
    if ((random()%1000) > 600)
      {
	if ((c=strchr(replaceable,pw[j]))!=NULL)
	  pw[j]=replacement[c-replaceable];
	else if (strchr(uppercaseable, pw[j]))
	  pw[j]=toupper(pw[j]);
      }
  return 1;
}
