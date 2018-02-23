#ifndef PARSERDEFINATIONHEADER
#define PARSERDEFINATIONHEADER
#include "lexerDef.h"
#define NO_OF_RULES 46

// can also use enum in plase of int type
typedef struct grammarnode{
	int type;  //type =1 for non terminal, type=0 for terminal, type=2 for |, type=3 for epsilon
	int value;
	char name[40];
	struct grammernode *next;
	struct grammernode *prev;
} grammarnode;

typedef grammarnode *GrammarNode;

typedef struct rulenode{
	int lineno;
	GrammarNode head;
	GrammarNode tail;
} Rule;

Rule* grammer[NO_OF_RULES];
#endif PARSERDEFINATIONHEADER