/*
Rohit Lodha
2015A7PS0040P
*/
#ifndef SEMANTICSHEADER
#define SEMANTICSHEADER

#include "semanticsDef.h"
#include "symbolTableDef.h"
#include "parserDef.h"
#include "astDef.h"

extern bool checkIOStmt(AStree ast);
extern bool checkOperatorType(AStree op, AStree ex1, AStree ex2);
extern bool checkRelationalOp(AStree ast);
extern bool checkRightSideType2(AStree ast,function* f);
extern int getTypeID(char* name , SymbolTablePtr b, int lineno);
extern variablenodeptr getID(char* name , SymbolTablePtr b);
extern void markAssigned(variablenodeptr v , SymbolTablePtr b) ;
extern bool inputType(AStree root , function *f);
extern bool outputType(AStree root , function *f);
extern void semanticAnalysis(AStree root);
extern function checkFunction(char* fname, SymbolTablePtr b, int lineno);
extern bool compatibleFunction(function f1, function f2, int line);
extern void checkRightSideType1(AStree ast);

#endif


