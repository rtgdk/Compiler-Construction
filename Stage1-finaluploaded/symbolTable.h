/*
Rohit Lodha
2015A7PS0040P
*/
#ifndef SYMBOLTABLEHEADER
#define SYMBOLTABLEHEADER

#include "symbolTableDef.h"
#include "parserDef.h"

extern void addFunction(function f, int start, int end,int type);
extern void addInputList(parsetree node, function *f);
extern void addOutputList(parsetree node,function *f);
extern void addVariableID(parsetree node, int type, SymbolTablePtr b);
extern void addVariable(variable v,SymbolTablePtr st);
extern void calculateCol(parsetree rhs,int* count2);
extern bool calculateRow(parsetree rhs,int* count1,int* count2);
extern variable convertNodeToVariable(parsetree node);
extern function createFunction(char* name);
extern SymbolTablePtr createNewST(function f);
extern variablenodeptr createVptrFromNode(parsetree node);
extern bool existAndMat(char* name,int lineno);
extern bool existAndStr(char* name,int lineno);
extern int getWidth(int type);
extern int hash4(char* str);
extern void initSymbolTable();
extern void makeST(parsetree root);
extern void printSTUtility();
extern void printST(SymbolTablePtr br);
extern void updateStrSize(parsetree rhs);
extern void updateMatSize(parsetree rhs);
extern int getType(parsetree pt);
#endif