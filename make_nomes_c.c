#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define NOMESIZE 4

/* Macros */

#define TRUE 1
#define FALSE 0
#define boolean int
#define WHITESPACE 0
#define TOKEN_Alpha 1
#define TOKEN_Connect 2
#define TOKEN_Delim 4
#define TOKEN_Junk 8

#define MINLENGTH 4

/* Types */

struct stree {
  char *data;
  int type;
  int num;
  struct stree *left,*right;
};

struct delim {
  char data;
  struct delim* next;
};

/* Globs */

char *Alpha="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
char *Connect="";
char *Delims="";

/* Övriga tecken betraktas som skräp */ 
 
struct delim *Delim_Stack=NULL;
 
/* Funcs */

void push(char c) {
  struct delim *tmp;
  tmp=(struct delim *)malloc(sizeof(struct delim));
  tmp->data=c;
  tmp->next=Delim_Stack;
  Delim_Stack=tmp;
}

void dup() {
  if (Delim_Stack)
    push(Delim_Stack->data);
}

char pop() {
  char tmpc;
  struct delim *tmp;
  if (!Delim_Stack)
    return ('\0');
  tmp=Delim_Stack;
  tmpc=tmp->data;
  Delim_Stack=tmp->next;
  free(tmp);
  return (tmpc);
}

char peek() {
  if (!Delim_Stack) 
    return('\0');
  else
    return (Delim_Stack->data);
}
 
int Prim_Token(char c) {
  if (strchr(Alpha,c)) return(TOKEN_Alpha); 
  if (strchr(Connect,c)) return(TOKEN_Connect); 
  if (strchr(Delims,c)) return(TOKEN_Delim); 
  if ((c>32)&&(c!=127)) return(TOKEN_Junk); 
  return(WHITESPACE);
}     
      

char *readword(char *cin) {
  static char inbuf[128];
  char c=(*cin);
  int tok;
  int bufind=0;
  char *retword;
  boolean ready=FALSE;
  tok=Prim_Token(c);
  while ((!feof(stdin))&&((tok==WHITESPACE)||
			  (tok==TOKEN_Junk)||
			  (tok==TOKEN_Connect))) {
    c=getchar();
    tok=Prim_Token(c);
  }
  if (feof(stdin)) return NULL;
  if (tok==TOKEN_Delim) {
    push(c);
    inbuf[bufind++]=c;
    c=getchar();
    tok=Prim_Token(c);
  }
  while ((!feof(stdin))&&(!ready)) {
    if (tok==TOKEN_Delim) {
      if (c==peek()) {
	pop();
	inbuf[bufind++]=c;
	c=getchar();
	tok=Prim_Token(c);
	if (!peek()) 
	  ready=TRUE;
      } else if (peek()) {
	inbuf[bufind++]=c;
	c=getchar();
	tok=Prim_Token(c);
      } else
	ready=TRUE;
    }

    if ((tok==WHITESPACE)||(tok==TOKEN_Junk)) {
      if (!peek())
	ready=TRUE;
      else {
	inbuf[bufind++]=c;
	c=getchar();
	tok=Prim_Token(c);
      }
    }
    if ((tok==TOKEN_Alpha)||(tok==TOKEN_Connect)) {
      inbuf[bufind++]=c;
      c=getchar();
      tok=Prim_Token(c);
    }
  }
  if (feof(stdin)) c='\0';
  if (bufind==0) return NULL;
  inbuf[bufind]='\0';
  if (!(retword=(char *)malloc(strlen(inbuf)+1))) {
    fprintf(stderr,"Insufficient memory!\n");
    exit(1);
  }
  strcpy(retword,inbuf);
  (*cin)=c;
  return(retword);
}

int insertword(char *word,struct stree **tree) {
  int diff;
  if (!word) return 0;
  if (!(*tree)) {
    if (!((*tree)=(struct stree *)malloc(sizeof(struct stree)))) {
      fprintf(stderr,"Insufficient memory!\n");
      exit(1);
    }
    (*tree)->data=word;
    (*tree)->num=1;
    (*tree)->left=NULL;
    (*tree)->right=NULL;
  } else {
    diff=strcmp((*tree)->data,word);
    if (!diff) (*tree)->num++;
    else
      if (diff<0) insertword(word,&((*tree)->right));
      else
	insertword(word,&((*tree)->left));
  }
  return (1);
}

void dumpnomes(struct stree *tree, FILE *nomefile) {
  int i;
  if (tree) {
    dumpnomes(tree->left, nomefile);
    if (strlen(tree->data)>=NOMESIZE) {
      for (i=0;i<=strlen(tree->data)-NOMESIZE;i++)
	fwrite(&(tree->data[i]),1,NOMESIZE,nomefile);
    }
    dumpnomes(tree->right, nomefile);
  }
}

void dumpwstart(struct stree *tree, FILE *beginfile) {
  if (tree) {
    dumpwstart(tree->left, beginfile);
    if (strlen(tree->data)>=NOMESIZE) {
      fwrite(tree->data,1,NOMESIZE,beginfile);
    }
    dumpwstart(tree->right, beginfile);
  }
}

void freetree(struct stree **tree) {
  if ((*tree)) {
    freetree(&((*tree)->left));
    freetree(&((*tree)->right));
    if ((*tree)->data) 
      free((*tree)->data);
    free(*tree);
    (*tree)=NULL;
  }
}

int main() {
  FILE *outfile;
  struct stree *wordtree=NULL;
  char *w;
  int i;
  char c=getchar();
  while ((w=readword(&c))!=NULL) {
    if (strlen(w) >= MINLENGTH) {
      for (i=0;i<strlen(w);i++)
	w[i]=tolower(w[i]);
      insertword(w,&wordtree);
    }
  }

  if (!(outfile=fopen("nomes.c","w"))) {
    perror("nomes.c");
    return 1;
  }
  fprintf(outfile, "%s", "char *nomes=\"");
  dumpnomes(wordtree, outfile);
  fprintf(outfile, "%s", "\"; char *wstart=\"");
  dumpwstart(wordtree, outfile);
  fprintf(outfile, "%s\n", "\";");
  fclose(outfile);
  freetree(&wordtree);
  return 0;
}
