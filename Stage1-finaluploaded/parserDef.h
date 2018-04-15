/*
Rohit Lodha
2015A7PS0040P
*/
#ifndef parserdef
#define parserdef

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <stdbool.h>

#include "lexerDef.h"
#define NO_OF_RULES 44
#define hashtablesize 100 
#define NO_OF_TERMINALS 40
#define NO_OF_NONTERMINALS 44
#define BUFFER_SIZE 200

char buffer[BUFFER_SIZE];
//char* terminals[NO_OF_TERMINALS];
bool** firstMatrix;
bool** followMatrix;
bool** synchMatrix;

typedef struct grammarnode{
	int type;  //type =1 for non terminal, type=0 for terminal, type=2 for |, type=3 for epsilon // can also use enum in place of int type
	char name[40];
	struct grammarnode *next;
	struct grammarnode *prev;
} grammarnode;

typedef grammarnode *GrammarNode;

typedef struct rulenode{
	int lineno;
	GrammarNode head;
	GrammarNode tail;
	int firstcalculated;  //0 for not done, 1 for done
	int followcalculated; //0 for not done, 1 for done
} Rule;

Rule* grammar[NO_OF_RULES];

typedef struct hashtable{
	int type;
	int ruleNo;  // 0 for terminal
	char name[50];
	struct hashtable* next;
} hashtable;

GrammarNode** parsetable;

hashtable* HashTable[hashtablesize];

typedef struct stack{
	GrammarNode bottom;
	GrammarNode top;
	//int size;
} stack;

typedef stack* Stack;
typedef struct treenode{
	tokenInfo tk;
	struct treenode* parent;
	struct treenode** child;
	int noc; //no of child
	struct treenode* next;
	GrammarNode ruleNode;
} treenode;

typedef treenode* parsetree;

#endif
