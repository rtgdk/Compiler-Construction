/*
Rohit Lodha
2015A7PS0040P
*/
#ifndef ASTHEADER
#define ASTHEADER

#include "astDef.h"

extern int hash3(char* str);
extern void createHashtable3();
extern int present2(char* name);
extern void printHashTable2();
extern AStree tokenizeTree2(AStree tree,tokenInfo tk);
extern AStree createASTree(parsetree pt);
extern AStree addChildren2(AStree tree, parsetree pt);
extern AStree next2(AStree tree);
extern AStree parseToAST(parsetree PT,int abs);
extern void pruneAST(AStree ast,int childNo);
extern void operatorAST(AStree ast,int childNo);
extern int isOperator(AStree ast);
extern AStree modifyAST(AStree ast);
extern AStree makeAST(parsetree PT, int abs);
extern void printAST(AStree ast);
extern void calculateCompression();

#endif
