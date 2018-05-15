#ifndef LexerDefination
#define LexerDefination 


extern char * keywords[];
extern char * str_characters[];
extern char * str_symbols[];
extern char * token_names[]; 

#define NO_OF_KEYWORDS 30
unsigned long hash_table_keywords[NO_OF_KEYWORDS];


typedef enum{
	END , INT , REAL , STRING , MATRIX , MAIN , IF , ELSE , ENDIF , READ , PRINT , FUNCTION ,
	PLUS, MINUS, MUL,	DIV, LT, LE, GT, GE, EQ, NE,
	SEMICOLON, SQO, SQC, CL, OP, COMMA, SIZE, ASSIGNOP, AND, OR, NOT, NUM, RNUM, FUNID, STR, ID,EOF1,ERROR
} character;


typedef struct
{
	character type; // like ASSIGNOP
 	int lineno;
 	char lexeme[50]; // like =
	char name[50];
}tokenInfo;


#endif
