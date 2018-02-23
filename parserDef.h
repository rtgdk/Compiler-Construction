#ifndef PARSERDEFINATIONHEADER
#define PARSERDEFINATIONHEADER
#include "lexerDef.h"
#define NO_OF_RULES 46

// can also use enum in plase of int type
typedef struct grammarnode{
	int type;  //type =1 for non terminal, type=0 for terminal
	int value;
	int or1;
	char name[40];
	int epsilon;  // epsilon =1 for epsilon, epsilon=0 for others
	struct grammernode *next;
	struct grammernode *prev;
}

typedef *grammarnode GrammarNode;

typdef struct rulenode{
	int lineno;
	GrammarNode head;
	GrammarNode tail;
} Rule;

Rule* grammer[NO_OF_RULES];

#endif PARSERDEFINATIONHEADER