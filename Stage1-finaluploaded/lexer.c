/*
Rohit Lodha
2015A7PS0040P
*/
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <stdbool.h>

#include "lexer.h"

int lineno = 1;
int currentpos=0;
int buffersize = -1;
int buffersize2 = -1;

void initialiseLexer(){
	lineno = 1;
	currentpos=0;
	buffersize = -1;
}

/***
For Developer Reference
1. start
2. Space,tab, new line
3. Comment
4. =
5. .
6. <
7. >
8. Number  --> RNUM
9. _
10. "
11. a-z
12. [
13. ]
14. (
15. )
16. ;
17. ,
18. +
19. _
20. *
21. /
22. @
23. =/
24. .a
25. .an
26. .and
27. .o
28. .or
29. .n
30. .no
31. .not
32. RNUM a.
33. RNUM a.b
34,35,36,37 -> Main
38. FunID
39. String "a
40 String "a

//Error

51. ""
52. "ErrorChar"
53. ">20"
54. number.<error>
55. _<number>
56. _<invalid>
57. _size>20
58. .<invalid>
59. Invalid Boolean operator
60. Invalid ID size
**/

char* keywords[]={"end","int","real","string","matrix","if","else","endif","read","print","function"};
char* keywords2[]={"END","INT","REAL","STRING","MATRIX","IF","ELSE","ENDIF","READ","PRINT","FUNCTION"};
int hashtablesize = 103;
int* KeywordHashTable;
/*
For hashing Non Terminals to bucket no in HashTable, using dj2b algorithm
*/
int hash(char* str)
{
	unsigned long hash = 5381;
	int c;
	while (c = *str++)
	    hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
	return (int)(hash % hashtablesize);
}

/*
Map all keywords to hash table
*/
void initialiseKeywordHash(){
	int i=0;
	KeywordHashTable = (int*)malloc(hashtablesize*sizeof(int));
	for(i=0;i<hashtablesize;i++){
		KeywordHashTable[i] = -1;
	}
	for(i=0;i<NO_OF_KEYWORDS;i++){
		int a = hash(keywords[i]);
		//printf("%d -- %s\n",a,keywords[i]);
		KeywordHashTable[a] = i;
	}
}

/*
find if the token is a keyword, if true return the name
*/

int isKeyword(tokenInfo tk,char* name){
	int a  = hash(tk.lexeme);
	if (KeywordHashTable[a]==-1) return -1;
	if (strcmp(keywords[KeywordHashTable[a]],tk.lexeme)==0) {
		strcpy(name,keywords2[KeywordHashTable[a]]);
		//printf("Returning %d\n",KeywordHashTable[a]);
		return KeywordHashTable[a];
	}
	else return -1;
}



/*
return charcater of the file one by one
*/	
char getStream(FILE* f)
{
    if(buffersize == -1 || currentpos == buffersize)
    {
        buffersize = fread(buffer2 , sizeof(char) , MAX_SIZE , f);
        currentpos = 0;
    }
    if(buffersize == 0)
        return '\0';
    else
        return buffer2[currentpos++];
}



/*
Return token one by one of the file fp
*/	
tokenInfo getNextToken(FILE *fp, int g){
	tokenInfo tk;
	char ch;
	int state = 1;
	int lexemecount = 0;
	int MAXIMUM=30;
	while(1){
		ch = getStream(fp);  
		tk.lineno = lineno;
		switch(state)
		{
			case 1: //start
				switch(ch){
					case '\n': lineno++;   //no break ?
					case ' ':
					case '\t': state = 1; break;
					case '#': state = 3; break;
					case '=': tk.lexeme[lexemecount++] = ch; state = 4; break;
					case '.': tk.lexeme[lexemecount++] = ch; state = 5; break;
					case '<': tk.lexeme[lexemecount++] = ch; state = 6; break;
					case '>': tk.lexeme[lexemecount++] = ch; state = 7; break;
					case '0' ... '9': tk.lexeme[lexemecount++] = ch; state = 8; break;
					case '_': tk.lexeme[lexemecount++] = ch; state = 9; break;
					case '"': tk.lexeme[lexemecount++] = ch; state = 10; break;
					case 'a' ... 'z':
					case 'A' ... 'Z' : tk.lexeme[lexemecount++] = ch; state = 11; break;
					case '[': tk.type = SQO; tk.lexeme[lexemecount++] = ch; tk.lexeme[lexemecount++] ='\0' ; strcpy(tk.name,"SQO") ;return tk;
					case ']': tk.type = SQC; tk.lexeme[lexemecount++] = ch; tk.lexeme[lexemecount++] ='\0' ; strcpy(tk.name,"SQC"); return tk;
					case '(': tk.type = OP; tk.lexeme[lexemecount++] = ch; tk.lexeme[lexemecount++] ='\0' ; strcpy(tk.name,"OP"); return tk;
					case ')': tk.type = CL; tk.lexeme[lexemecount++] = ch; tk.lexeme[lexemecount++] ='\0' ; strcpy(tk.name,"CL"); return tk;
					case ';': tk.type = SEMICOLON; tk.lexeme[lexemecount++] = ch; tk.lexeme[lexemecount++] ='\0' ; strcpy(tk.name,"SEMICOLON"); return tk;
					case ',': tk.type = COMMA; tk.lexeme[lexemecount++] = ch; tk.lexeme[lexemecount++] ='\0' ; strcpy(tk.name,"COMMA"); return tk;
					case '+': tk.type = PLUS; tk.lexeme[lexemecount++] = ch; tk.lexeme[lexemecount++] ='\0' ; strcpy(tk.name,"PLUS"); return tk;
					case '-': tk.type = MINUS; tk.lexeme[lexemecount++] = ch; tk.lexeme[lexemecount++] ='\0' ; strcpy(tk.name,"MINUS"); return tk;
					case '*': tk.type = MUL; tk.lexeme[lexemecount++] = ch; tk.lexeme[lexemecount++] ='\0' ; strcpy(tk.name,"MUL"); return tk;
					case '/': tk.type = DIV; tk.lexeme[lexemecount++] = ch; tk.lexeme[lexemecount++] ='\0' ; strcpy(tk.name,"DIV"); return tk;
					case '@': tk.type = SIZE; tk.lexeme[lexemecount++] = ch; tk.lexeme[lexemecount++] ='\0' ; strcpy(tk.name,"SIZE"); return tk;
					case '\0': tk.type = EOF1; tk.lexeme[lexemecount++] = ch; tk.lexeme[lexemecount++] ='\0'; strcpy(tk.name,"EOF");return tk;
					default : tk.lexeme[lexemecount++] = ch; tk.lexeme[lexemecount++] ='\0'; tk.type=ERROR; 
					if(g==1) printf("\nLine No.%d: Lexical Error: Unknown Symbol '%s'.\n",tk.lineno,tk.lexeme); return tk; 
				}
				break;
				// state 2 not needed as \t \n not capture anything
				// case 2: //\t, space, \n
				// switch(ch){
					// case '\n': lineno++;
					// case ' ':
                    // case '\t': state=2; break;   // just break, capture 2 spaces/tabs/lines
					// case '\0': tk.type = EOF1; strcpy(tk.lexeme , "EOF"); return tk;
					// default: currentpos--; state=1; break;
				// }
				// break;
				case 3: // #
				switch(ch){
					case '\n': lineno++; state = 1; break; //tk.type = COMMENT; strcpy(tk.name,"COMMENT") ;tk.lexeme[lexemecount++] ='\0'; return tk;//tk.type = COMMENT; tk.lexeme[lexemecount++] ='\0' ; strcpy(tk.name,"COMMENT") ;return tk; //
					case '\0':  break;// error? break;
					default : state = 3; break; //can use while loop here
				}
				break;
				case 4: // =
					switch(ch){
						case '=': tk.type = EQ; tk.lexeme[lexemecount++] = ch; tk.lexeme[lexemecount++] ='\0' ; strcpy(tk.name,"EQ"); return tk;
						case '/': tk.lexeme[lexemecount++] = ch; state = 23; break;
						//case '\0': //error ? 
						default : currentpos--; tk.type = ASSIGNOP; tk.lexeme[lexemecount] ='\0' ; strcpy(tk.name,"ASSIGNOP"); return tk;
					}
					break;
				case 23: // =/
					switch(ch){
						case '=': tk.type = EQ; tk.lexeme[lexemecount++] = ch; tk.lexeme[lexemecount++] ='\0' ; strcpy(tk.name,"NE"); return tk;
						default : currentpos--; state=59; break;
					}
				case 59:
					currentpos--; tk.lexeme[lexemecount++] = ch; tk.lexeme[lexemecount++] ='\0'; if(g==1) printf("\nLine No.%d: Lexical Error: Invalid Relational operator '%s' \n",tk.lineno,tk.lexeme);tk.type=ERROR; return tk;
				case 5: // .
					switch(ch){
						case 'a': tk.lexeme[lexemecount++] = ch; state=24; break;
						case 'o': tk.lexeme[lexemecount++] = ch; state=27; break;
						case 'n': tk.lexeme[lexemecount++] = ch; state=29; break;
						default : currentpos--; state=58; break;
					}
					break;
				case 24: //.a
					switch(ch){
						case 'n': tk.lexeme[lexemecount++] = ch; state=25; break;
						default : currentpos--; state=58; break;
					}
					break;
				case 25: //.an
					switch(ch){
						case 'd': tk.lexeme[lexemecount++] = ch; state=26; break;
						default : currentpos--; state=58; break;
					}
					break;
				case 26: //.and
					switch(ch){
						case '.': tk.type = AND; tk.lexeme[lexemecount++] = ch; tk.lexeme[lexemecount++] ='\0' ; strcpy(tk.name,"AND"); return tk;
						default : currentpos--; state=58; break;
					}
					break;
				case 27: //.o
					switch(ch){
						case 'r': tk.lexeme[lexemecount++] = ch; state=28; break;
						default : currentpos--; state=58; break;
					}
					break;
				case 28: //.or
					switch(ch){
						case '.': tk.type = OR; tk.lexeme[lexemecount++] = ch; tk.lexeme[lexemecount++] ='\0' ; strcpy(tk.name,"OR"); return tk;
						default : currentpos--; state=58; break;
					}
					break;
				case 29: //.n
					switch(ch){
						case 'o': tk.lexeme[lexemecount++] = ch; state=30; break;
						default : currentpos--; state=58; break;
					}
					break;
				case 30: //.no
					switch(ch){
						case 't': tk.lexeme[lexemecount++] = ch; state=31; break;
						default : currentpos--; state=58; break;
					}
					break;
				case 31: //.not
					switch(ch){
						case '.': tk.type = NOT; tk.lexeme[lexemecount++] = ch; tk.lexeme[lexemecount++] ='\0' ; strcpy(tk.name,"NOT"); return tk;
						default : currentpos--; state=58; break;
					}
					break;
				case 58:
					currentpos--; tk.lexeme[lexemecount++] = ch; tk.lexeme[lexemecount++] ='\0' ; if(g==1) printf("\nLine No.%d: Lexical Error: Invalid Logical operator '%s'\n",tk.lineno,tk.lexeme); tk.type=ERROR; return tk;
				case 6: // <
					switch(ch){
						case '=': tk.type = LE; tk.lexeme[lexemecount++] = ch; tk.lexeme[lexemecount++] ='\0' ; strcpy(tk.name,"LE"); return tk;
						//case '\0': state=59; break;
						default : currentpos--; tk.type = LT; tk.lexeme[lexemecount] ='\0' ; strcpy(tk.name,"LT"); return tk;
					}
					break;
				case 7: // >
					switch(ch){
						case '=': tk.type = GE; tk.lexeme[lexemecount++] = ch; tk.lexeme[lexemecount++] ='\0' ; strcpy(tk.name,"GE"); return tk;
						//case '\0': // error
						default : currentpos--; tk.type = GT; tk.lexeme[lexemecount] ='\0' ; strcpy(tk.name,"GT"); return tk;
					}
					break;
				case 8: // 1 (number)
					switch(ch){
						case '0' ... '9': tk.lexeme[lexemecount++] = ch;  state = 8; break;
						case '.': tk.lexeme[lexemecount++] = ch;  state = 32; break;
						default : currentpos--; tk.type = NUM; tk.lexeme[lexemecount] ='\0' ; strcpy(tk.name,"NUM"); return tk;
					}
					break;
				case 32: //RNUM a.
					switch(ch){
						case '0' ... '9': tk.lexeme[lexemecount++] = ch;  state = 33; break;
						case 'a':
						case 'o':
						case 'n': currentpos--; currentpos--; tk.type = NUM; tk.lexeme[lexemecount-1] ='\0' ; strcpy(tk.name,"NUM"); return tk;
						default : state = 54; break;
					}
					break;
				case 33: //RNUM a.b
					switch(ch){
						case '0' ... '9': tk.type = RNUM; tk.lexeme[lexemecount++] = ch; tk.lexeme[lexemecount++] ='\0' ; strcpy(tk.name,"RNUM"); return tk;
						default : //check
									currentpos--; state = 54; break; //tk.type = RNUM; tk.lexeme[lexemecount++] ='\0' ; strcpy(tk.name,"RNUM"); return tk;
					}
					break;
				case 54: //number.<char>
					currentpos--; tk.lexeme[lexemecount++] = ch; tk.lexeme[lexemecount++] ='\0' ; if(g==1) printf("\nLine No.%d: Lexical Error: Invalid Real Number '%s'\n",tk.lineno,tk.lexeme); tk.type=ERROR; return tk; // or maybe space not given
				case 9: // _
					switch(ch){
						case 'm': tk.lexeme[lexemecount++] = ch; state = 34; break;
						case 'a' ... 'l':
						case 'n' ... 'z': 
						case 'A' ... 'Z':tk.lexeme[lexemecount++] = ch; state = 38; break;
						case '0' ... '9': currentpos--; state = 55; break;
						default : currentpos--; state = 56; break;
					}
					break;
				case 55:
					currentpos--; tk.lexeme[lexemecount++] = ch; tk.lexeme[lexemecount++] ='\0' ; if(g==1) printf("\nLine No.%d: Lexical Error: Invalid Function Name '%s'. _ cannot be followed by a number\n",tk.lineno,tk.lexeme); tk.type=ERROR; return tk;
				case 56:
					currentpos--; tk.lexeme[lexemecount++] = ch; tk.lexeme[lexemecount++] ='\0' ; if(g==1) printf("\nLine No.%d: Lexical Error: Function '%s' contain invalid character. Only alphabets and 1 number(at the end,optional) is allowed.\n",tk.lineno,tk.lexeme); tk.type=ERROR; return tk;
				case 34: // _m
					switch(ch){
						case 'a': tk.lexeme[lexemecount++] = ch; state = 35; break;
						case 'b' ... 'z':
						case 'A' ... 'Z':
						case '0' ... '9': tk.lexeme[lexemecount++] = ch; state = 38; break;
						case '[':
						case '(': 
						case ' ': 
						case '\t':
						case '\n': currentpos--;tk.type = FUNID; tk.lexeme[lexemecount++] ='\0' ; strcpy(tk.name,"FUNID"); return tk;
						default: currentpos--;tk.lexeme[lexemecount++] ='\0' ; strcpy(tk.name,"FUNID"); return tk; state = 56; break; //tk.lexeme[lexemecount++] ='\0' ; strcpy(tk.name,"FUNID"); return tk;
					}
					break;
				case 35: // _ma
					switch(ch){
						case 'i': tk.lexeme[lexemecount++] = ch; state = 36; break;
						case 'a' ... 'h':
						case 'j' ... 'z':
						case 'A' ... 'Z':
						case '0' ... '9': tk.lexeme[lexemecount++] = ch; state = 38; break;
						case '[':
						case '(': 
						case ' ': 
						case '\t':
						case '\n': currentpos--;tk.type = FUNID; tk.lexeme[lexemecount++] ='\0' ; strcpy(tk.name,"FUNID"); return tk;
						default: currentpos--;tk.lexeme[lexemecount++] ='\0' ; strcpy(tk.name,"FUNID"); return tk; state = 56; break;
					}
					break;
				case 36: //_mai
					switch(ch){
						case 'n': tk.lexeme[lexemecount++] = ch; state = 37; break;
						case 'a' ... 'm':
						case 'o' ... 'z': 
						case 'A' ... 'Z':
						case '0' ... '9': tk.lexeme[lexemecount++] = ch; state = 38; break;
						case '[':
						case '(': 
						case ' ': 
						case '\t':
						case '\n': currentpos--;tk.type = FUNID; tk.lexeme[lexemecount++] ='\0' ; strcpy(tk.name,"FUNID"); return tk;
						default: currentpos--;tk.lexeme[lexemecount++] ='\0' ; strcpy(tk.name,"FUNID"); return tk; state = 56; break;
					}
					break;
				case 37: //_main
					switch(ch){
						case 'a' ... 'z':
						case 'A' ... 'Z':
						case '0' ... '9': tk.lexeme[lexemecount++] = ch;  state = 38; break;
						case '[':
						case '(': 
						case ' ': 
						case '\t':
						case '\n': currentpos--;tk.type = MAIN; tk.lexeme[lexemecount] ='\0' ; strcpy(tk.name,"MAIN"); return tk;
						default: currentpos--;tk.lexeme[lexemecount++] ='\0' ; strcpy(tk.name,"MAIN"); return tk;  //state = 56; break;
					}
					break;
				case 38: //_funntion
					switch(ch){
						case 'a' ... 'z':
						case 'A' ... 'Z':
						case '0' ... '9': tk.lexeme[lexemecount++] = ch; state = 38; break;
						case '[':
						case '(': 
						case ' ': 
						case '\t':
						case '\n': currentpos--;
								if (lexemecount<=20){
									tk.type = FUNID; tk.lexeme[lexemecount++] ='\0' ; strcpy(tk.name,"FUNID"); return tk;
								 }
								 else{
									state=57;break;
								 }
						default: currentpos--;tk.lexeme[lexemecount++] ='\0' ; strcpy(tk.name,"FUNID"); return tk; state = 56; break;
					}
					break;
				case 57:
					currentpos--; if(g==1) printf("\nLine No.%d: Lexical Error: Function Name '%s' exceeds the maximum length limit of 20\n",tk.lineno,tk.lexeme); tk.type=ERROR; return tk;
				case 10: // "
					switch(ch){
						case 'a' ... 'z':
						case ' ': tk.lexeme[lexemecount++] = ch; state=39; break;
						case '"': state = 51; break;  //"" length=2, no string
						default : currentpos--; state = 52; break;
					}
					break;
				case 39: //"a
					switch(ch){
						case 'a' ... 'z': 
						case ' ': tk.lexeme[lexemecount++] = ch;  state=39; break;
						case '"':{ 
							tk.lexeme[lexemecount++] = ch; tk.lexeme[lexemecount++] ='\0' ;
							if (lexemecount<=21){
								tk.type = STR; strcpy(tk.name,"STR"); return tk;
							}
							else{
								state = 53; break;
							}
						}
						default: state = 52; break;
					}
					break;
				case 51: //currentpos--;
					 if(g==1) printf("\nLine No.%d: Lexical Error: Empty String\n",tk.lineno); tk.type=ERROR; return tk;
				case 52:
					 if(g==1) printf("\nLine No.%d: Lexical Error: String contains invalid character\n",tk.lineno); tk.type=ERROR; return tk;
				case 53:
					if(g==1) printf("\nLine No.%d: Lexical Error: String '%s' exceeds the maximum length of 20\n",tk.lineno,tk.lexeme); tk.type=ERROR; return tk; //maybe add \0 in lexeme
				
				case 11:  //ID
					switch(ch){
						case 'a' ... 'z': 
						case 'A' ... 'Z': tk.lexeme[lexemecount++] = ch;  state=11; break;
						case '0' ... '9': tk.lexeme[lexemecount++] = ch;
							if (lexemecount<=20){
								tk.lexeme[lexemecount] ='\0' ;
								char name[50];
								int type = isKeyword(tk,name);
								if(type!=-1){
									tk.type = (tp)type; strcpy(tk.name,name);
								}
								else{
									tk.type = ID;  strcpy(tk.name,"ID");
								}
								return tk;
							}
							else{
								state=60; break;
							}
							break;
						default : {
							currentpos--;
							if (lexemecount<=20){
								tk.lexeme[lexemecount] ='\0' ;
								char name[50];
								int type = isKeyword(tk,name);
								if(type!=-1){
									tk.type = (tp)type; strcpy(tk.name,name);
								}
								else{
									tk.type = ID;  strcpy(tk.name,"ID");
								}
								return tk;
							}
							else{
								state=60; break;
							}
							break;
						}
					}
					break;
				case 60:
					currentpos--; if(g==1) printf("\nLine No.%d: Lexical Error: ID '%s' exceeds the maximum length of 20\n",tk.lineno,tk.lexeme); tk.type=ERROR; return tk;
		}
	}
}

/* 
Print tokens
*/
void printtokens(char *name)
{
  tokenInfo to;
  FILE *fp = fopen(name,"r");
  if(fp == NULL)
    {
        printf("File %s does not exist!\n" , name);
        return;
    }    
  printf("   line       Name      LEXEME\n");
  to=getNextToken(fp,1);
  while(to.type!=EOF1 && strcmp(to.name,"")!=0)
    {
		if(to.type!=ERROR){
			printf("%6d%14s%16s\n",to.lineno,to.name,to.lexeme);
		}
      to=getNextToken(fp,1);
   }
  fclose (fp);
  lineno = 1;
  return;
}

/*
print code after removing comments
*/	
void removeComments(char *testcaseFile){
	FILE* fp = fopen(testcaseFile , "r");
    if(fp == NULL)
    {
        printf("File %s does not exist!\n" , testcaseFile);
        return;
    }    
    char c;
    while((c = fgetc(fp)) != EOF)
    {        
        if(c != '#')
            putchar(c);
        else
        {
			while((c = fgetc(fp)) != EOF)
                {
                    if(c == '\n')
                    {
						putchar('\n');
                        break;
                    }
                }
        }

    }
    fclose(fp);
}

// We knoe the length error at 20 only, no need to go all through the string
// loook for how code will give state when \n in between strings
