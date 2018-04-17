/*
Rohit Lodha
2015A7PS0040P
*/
#ifndef SEMANTICSHEADER
#define SEMANTICSHEADER

#include "astDef.h"

bool checkOperatorType(AStree op, AStree ex1, AStree ex2);
int getTypeID(char* name , SymbolTablePtr b, int lineno);
void inputType(astptr root , function *f);
bool outputType(astptr root , function *f);
void semanticAnalysis(astptr root);

#endif