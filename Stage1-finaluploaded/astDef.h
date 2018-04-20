/*
Rohit Lodha
2015A7PS0040P
*/
#ifndef ASTDEFHEADER
#define ASTDEFHEADER

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <stdbool.h>

#include "symbolTableDef.h"
#include "parserDef.h"
#define NO_OF_ALLOWED_TERMINALS 27
#define hashtablesize2 102
typedef struct hashtable2{
	char name[50];
	struct hashtable2* next;
} hashtable2;

hashtable2* HashTable2[hashtablesize2];

typedef struct astnode{
	tokenInfo tk;
	struct astnode* parent;
	struct astnode** child;
	int noc; //no of child
	struct astnode* next;
	GrammarNode ruleNode;
	SymbolTablePtr st;
	int type; //1 int, 2 real, 3 string, 4 matrix , 5 for not defined, 6 for int x int, 7 to not propoagate the error and continue checking whether other symbols are valid or not
} astnode;

typedef astnode* AStree;
int noast;

#endif