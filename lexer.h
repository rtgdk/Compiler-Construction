#ifndef LexerHeader
#define LexerHeader
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <stdbool.h>
#include "lexerDef.h"

FILE *getStream(FILE* fp, char* buffer);
void removeComments(char *testcaseFile, char *cleanFile);
tokenInfo* getNextToken(FILE *fp, char* buf[], char** begin, char** forward, 
	int* currentBuf, unsigned long int* currentLine);
	
#endif
