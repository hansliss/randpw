#include <stdio.h>

#define NOMESIZE 4
#if 0
# define NOMEFILE "/usr/local/etc/nome.dat"
# define BEGINFILE "/usr/local/etc/wordstart.dat"
#else
# define NOMEFILE "nome.dat"
# define BEGINFILE "wordstart.dat"
#endif

int main()
{
  char inbuf[1024];
  int i;
  FILE *nomefile,*beginfile;
  if (!(nomefile=fopen(NOMEFILE,"w")))
    {
      perror(NOMEFILE);
      return 1;
    }
  if (!(beginfile=fopen(BEGINFILE,"w")))
    {
      perror(BEGINFILE);
      return 1;
    }
  while (fgets(inbuf,1023,stdin))
    {
      if (inbuf[strlen(inbuf)-1]=='\n')
	inbuf[strlen(inbuf)-1]='\0';
      if (strlen(inbuf)>=NOMESIZE)
	{
	  fwrite(inbuf,1,NOMESIZE,beginfile);
	  for (i=0;i<=strlen(inbuf)-NOMESIZE;i++)
	    fwrite(&(inbuf[i]),1,NOMESIZE,nomefile);
	}
    }
  fclose(nomefile);
  fclose(beginfile);
  return 0;
}
