#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <crypt.h>
#include <time.h>

#include "randpw.h"

void usage(char *progname) {
  fprintf(stderr, "Usage: %s [-n <count>] [-l <length>] [-c (do not obfuscate)] [-e (encrypt)] [-t <cleartext password>] [<cleartext password> ...]\n", progname);
}

void ppw(char *pw, int encrypt) {
  char salt[3];
  if (encrypt) {
    salt[0]=rand()%26+'A';
    salt[1]=rand()%26+'A';
    salt[2]='\0';
    printf("%s:%s\n", pw, crypt(pw, salt));
  } else
    printf("%s\n", pw);
}

int main(int argc, char *argv[]) {
  int i=1;
  int o;
  int munge=1;
  int encrypt=0;
  length=10;
  static char tmpbuf[8192];
  while ((o=getopt(argc, argv, "n:l:cet:"))!=-1) {
    switch (o) {
    case 'n':
      i=atoi(optarg);
      break;
    case 'l':
      length=atoi(optarg);
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
  if (length >= sizeof(tmpbuf))
    length = sizeof(tmpbuf)-1;
  if (optind < argc) {
    if (i==1)
      i=0;
    while (optind < argc)
      ppw(argv[optind++], encrypt);
  }
  while (i--) {
    while (!randpw(tmpbuf));
    if (munge)
      mungepw(tmpbuf);
    ppw(tmpbuf, encrypt);
  }
  return 0;
}
