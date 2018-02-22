#ifndef LexerDefination
#define LexerDefination 


extern char * keywords[];
extern char * str_characters[];
extern char * str_symbols[];
extern char * token_names[]; 

#define NO_OF_KEYWORDS 30
unsigned long hash_table_keywords[NO_OF_KEYWORDS];


typedef enum{
	
}character;


typedef struct
{
	character type; // like ASSIGNOP
 	int lineno;
 	char lexeme[50]; // like =
	char name[50];
}tokenInfo;


#endif
