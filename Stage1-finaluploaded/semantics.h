/*
Rohit Lodha
2015A7PS0040P
*/
#ifndef SEMANTICSHEADER
#define SEMANTICSHEADER

#include "astDef.h"

bool checkOperatorType(AStree op, AStree ex1, AStree ex2);
int getTypeID(char* name , SymbolTablePtr b, int lineno);
void inputType(AStree root , function *f);
bool outputType(AStree root , function *f);
void semanticAnalysis(AStree root);

#endif
