#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <crypt.h>
#include <time.h>

#include "randpw.h"

void usage(char *progname)
{
  fprintf(stderr, "Usage: %s [-n <count>] [-c (do not obfuscate)] [-e (encrypt)] [-t <ct password>]\n", progname);
}

void ppw(char *pw, int encrypt)
{
  char salt[3];
  if (encrypt)
    {
      salt[0]=rand()%26+'A';
      salt[1]=rand()%26+'A';
      salt[2]='\0';
      printf("%s:%s\n", pw, crypt(pw, salt));
    }
  else
    printf("%s\n", pw);
}

int main(int argc, char *argv[])
{
  int i=1;
  int o;
  int munge=1;
  int encrypt=0;
  static char tmpbuf[8192];
  while ((o=getopt(argc, argv, "n:cet:"))!=-1)
    {
      switch (o)
	{
	case 'n':
	  i=atoi(optarg);
	  break;
	case 'c':
	  munge=0;
	  break;
	case 'e':
	  encrypt=1;
	  srand(time(NULL));
	  break;
	case 't':
	  ppw(optarg, encrypt);
	  if (i==1)
	    i=0;
	  break;
	default:
	  usage(argv[0]);
	  return -1;
	  break;
	}
    }
  if (optind < argc)
    {
      usage(argv[0]);
      return -1;
    }
  while (i--)
    {
      while (!randpw(tmpbuf));
      if (munge)
	mungepw(tmpbuf);
      ppw(tmpbuf, encrypt);
    }
  return 0;
}
