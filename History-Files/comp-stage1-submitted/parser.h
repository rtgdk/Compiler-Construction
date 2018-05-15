/*
Rohit Lodha
2015A7PS0040P
*/
#ifndef PARSERHEADER
#define PARSERHEADER

#include "parserDef.h"

extern int hash2(char* str);
extern void printHashTable();
extern GrammarNode makenode(int type,int value,char* name);
extern Rule* makerule(int lineno);
extern void initHashtable();
extern void insert(int hashvalue,int type,char* name, int ruleNode);
extern void createGrammar(char *grammarfile);
extern void printGrammar();
extern void initialisefirstandfollowMatrix();
extern hashtable* present(char* name);
extern void findFirst(char* name);
extern void findfollow(char* name);
extern void findFirstSet();
extern void findFollowSet();
extern void initParseTable();
extern int* firstofRHS(GrammarNode rhs);
extern void createParseTable();
extern void printParseTable();
extern Stack createStack();
extern GrammarNode top(Stack* st);
extern void push(Stack* st,GrammarNode gn);
extern void pop(Stack* st);
extern parsetree createParseTree();
extern parsetree next(parsetree tree);
extern parsetree tokenizeTree(parsetree tree,tokenInfo tk);
extern parsetree makeTreeFromGrammarNode(GrammarNode node);
extern parsetree addChildren(parsetree tree, GrammarNode ruleNode);
extern bool isStackEmpty(Stack* st);
extern void initialiseSynchMatrix();
extern void expectedToken(char* name,char* expected);
extern parsetree parseInputSourceCode(char *testcaseFile, GrammarNode** parsetable);
extern void printParseTree(parsetree PT, FILE* fp);
extern void fileprint(parsetree tr,FILE *fp);

#endif
