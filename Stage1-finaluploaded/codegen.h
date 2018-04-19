/*
Rohit Lodha
2015A7PS0040P
*/

#ifndef codegenh
#define codegenh
#include "astDef.h"

extern void gen_code_util(char* str,AStree ast);
extern void gen_code(AStree ast);
extern void get_var_name(char* name, AStree a);
extern void left(AStree a);
extern void right(AStree a);
extern void compose(AStree a,tp t);
extern void gen_data(SymbolTablePtr broot);
extern void gen_prog(AStree aroot,SymbolTablePtr broot);
FILE* fasm;
int label;

#endif