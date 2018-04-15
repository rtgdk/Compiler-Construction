/*
Rohit Lodha
2015A7PS0040P
*/
#ifndef LexerDefination
#define LexerDefination 
//#include <stdio.h>
//#include <stdlib.h>
//#include <malloc.h>
//#include <string.h>
//#include <stdbool.h>
#define NO_OF_KEYWORDS 11
#define MAX_SIZE 1024

extern char* keywords[];
extern char* keywords2[];
extern int* KeywordHashTable;
char buffer2[MAX_SIZE];

typedef enum{ END , INT , REAL , STRING , MATRIX , IF , ELSE , ENDIF , READ , PRINT , FUNCTION , PLUS, MINUS, MUL,	DIV, LT, LE, GT, GE, EQ, NE, 
    MAIN , SEMICOLON, SQO, SQC, CL, OP, COMMA, SIZE, ASSIGNOP, AND, OR, NOT, NUM, RNUM, FUNID, STR, ID,EOF1,ERROR
} tp;


typedef struct
{
	tp type; // like ASSIGNOP
 	int lineno;
 	char lexeme[60]; // like =
	char name[60];
}tokenInfo;


#endif
