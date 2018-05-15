# Compiler-Construction
## Compiler for C-like language developed as part of Compiler Construction course, BITS Pilani - CS F363

## By Rohit Lodha (2015A7PS0040P)

## Language Specification is given in the repository (the pdf file)

## In the History-Files folder - There are old versions files for stage1 and stage2 of the course project.

## Description of the files

### lexer.c

This file contains the lexer, which generates lexemes for use by parser and other components.
Also it contains function to separate out comments from the code.

### parser.c

This file contains the code for the parser, which works on the output of the lexer and generates the parse tree. In the process, first and 
follow sets are build and parsetable is constructed from them. The parser checks for any error in the code and try to fix them using 
synchronising sets.

### ast.c
The AST is then generated from the parse tree based on some common heuristics. This process typically shortens the parse tree by removing
 about 65% of the nodes.

### symbolTable.c

This file contains the code for creating symbol table. This can be made using ast or parse tree.(In the current code it is done by parse 
tree). It stores information about various tokens (data type, point of initialization, scope, last reference etc.). This is then used for
type checking.

### semantics.c

Using the symbol table, type checking and semantic analysis is done in this file.


### Test cases are given as text file c1,c2,..,testcase1, testcase2,..
More test cases are in the History-Files folder


### NASM code generation is not implemented.

 


