/*
Rohit Lodha
2015A7PS0040P
*/
#ifndef SYMBOLTABLEHEADER
#define SYMBOLTABLEHEADER

#include "symbolTableDef.h"
#include "parserDef.h"
extern int hash4(char* str);
extern void initSymbolTable();
extern int get_int(char *arr);
extern void addVariable(variable v,SymbolTablePtr st);
extern void addFunction(function f, int start, int end,int type);
extern int getType(parsetree pt) ;
extern int getWidth(int type);
extern void addVariableID(parsetree node, int type, SymbolTablePtr b);
extern void addFunctionID(parsetree node);
extern void addInputList(parsetree node, function *f);
extern void addOutputList(parsetree node,function *f);
extern void makeST(parsetree root);
extern void print_symboltable(SymbolTablePtr br);
extern void printST();

#endif
