/*
Rohit Lodha
2015A7PS0040P
*/
#ifndef LexerHeader
#define LexerHeader

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <stdbool.h>

#include "lexerDef.h"

extern void initialiseLexer();
extern int hash(char* str);
extern void initialiseKeywordHash();
extern int isKeyword(tokenInfo tk,char* name);
extern char getStream(FILE* f);
extern void removeComments(char *testcasefile);
extern tokenInfo getNextToken(FILE *fp,int g);
extern void printtokens(char *name);
	
#endif
