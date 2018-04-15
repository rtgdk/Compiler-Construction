/*
Rohit Lodha
2015A7PS0040P
*/
#ifndef astdef
#define astdef

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
} astnode;

typedef astnode* AStree;

#endif