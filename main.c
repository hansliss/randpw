#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "randpw.h"

void usage(char *progname)
{
  fprintf(stderr, "Usage: %s [-n <count>] [-c (cleartext)]\n", progname);
}

int main(int argc, char *argv[])
{
  int i=1;
  int o;
  int munge=1;
  static char tmpbuf[8192];
  while ((o=getopt(argc, argv, "n:c"))!=-1)
    {
      switch (o)
	{
	case 'n':
	  i=atoi(optarg);
	  break;
	case 'c':
	  munge=0;
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
      printf("%s\n", tmpbuf);
    }
  return 0;
}
