#ifndef PARSERDEFINATIONHEADER
#define PARSERDEFINATIONHEADER
#include "lexerDef.h"
#define NO_OF_RULES 46

// can also use enum in plase of int type
typedef struct grammarnode{
	int type;  //type =1 for non terminal, type=0 for terminal, type=2 for |, type=3 for epsilon
	int value; // -3 for epsilon, -2 for |, -1 for terminal, value is the index in hashtable
	char name[40];
	struct grammernode *next;
	struct grammernode *prev;
} grammarnode;

typedef grammarnode *GrammarNode;

// typedef struct firstfollownode{
	// char name[40];
	// struct firstfollownode *next;
// } firstfollownode;

// typedef firstfollownode *FirstFollowNode;

typedef struct rulenode{
	int lineno;
	GrammarNode head;
	GrammarNode tail;
	int firstcalculated;  //0 for not done, 1 for done
	int followcalculated; //0 for not done, 1 for done
} Rule;

Rule* grammer[NO_OF_RULES];
#endif PARSERDEFINATIONHEADER