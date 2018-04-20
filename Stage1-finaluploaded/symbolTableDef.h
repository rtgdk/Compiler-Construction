/*
Rohit Lodha
2015A7PS0040P
*/
#ifndef SYMBOLTABLEDEF
#define SYMBOLTABLEDEF

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <stdbool.h>

#include "parserDef.h"
#define VariableTableSize 100

// typedef struct array{
// 	int col;
// 	int row;
// } array;

typedef struct variable{
	char name[40];
	int type;  //1 int, 2 real, 3 string, 4 matrix , 5 for not defined
	int linedec; //line no of declaration
	int scopeDepth;
	int width; //size of id
	int col;
	int row;
	bool assigned;
} variable;

typedef struct variablenode{
	variable v;
	struct variablenode* next; 
} variablenode;

typedef struct variablenode* variablenodeptr;

typedef struct function{
	char name[50];
	int noOfInput;
	variablenodeptr inputList;
	int noOfOutput;
	variablenodeptr outputList;
	int linedec;
}function;

typedef struct functionnode{
	function f;
	struct functionnode* next; 
} functionnode;

typedef struct functionnode* functionnodeptr;

typedef struct SymbolTableNode{
	int type;  //1 for main, 2 for function, 3 for if , 4 for else, 5 for root
	function f;
	variablenodeptr VariableTable[VariableTableSize];
	int noc;
	struct SymbolTableNode* parent;
	struct SymbolTableNode** child;
	int start;
	int end;
	int counter;
} SymbolTableNode;

typedef struct SymbolTableNode* SymbolTablePtr;
SymbolTablePtr globalTable;
SymbolTablePtr currentTable;
int currentScope;
variablenodeptr matVar,strVar;
SymbolTablePtr matTable,strTable;
int sterror;
int totaloffset;
char* types[] = {"INTEGER" , "REAL" , "STRING","MATRIX"};
#endif