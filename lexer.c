#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "lexer.h"

#define MAX_SIZE 30
char buffer[MAX_SIZE];
int buffersize

/***
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

char* keywords[]={"END","INT","REAL","STRING","MATRIX","MAIN","IF","ELSE","ENDIF","READ","PRINT","FUNCTION"}

int isKeyword(tokenInfo tk,char* name){
	for(i=0;i<noOfKeywords;i++){
		if(strcmp(keywords[i],tk.lexeme)==0){
			name = keywords[i];
			return i;
		}
	}
	return -1; //not a keyword
}
FILE *getStream(FILE *fp,int k) //k is buffersize
    {
        size_t file_size;                                //size of the file
        if (fp == NULL) {
			printf("No file")
			return fp;
		}
		file_size=fread(B,sizeof(char),MAXIMUM,fp);   //allocating buffer with file contents
        if(file_size==0){
			buffer[0]=EOF;
		}
		else if(file_size<sizeof(char)*buffersize){
			buffer[file_size] =EOF;
		}
		buffer[bufferSize]='\0';
		if(feof(fp))B[file_size]='$';            //assigning bottom-marker of buffer with $
        return fp;
    }
	
void removeComments(char *testcaseFile, char *cleanFile){
	FILE* f1 = fopen(testcasefile , "r");
    if(f1 == NULL)
    {
        printf("File %s does not exist!\n" , testcasefile);
        return;
    }    
    char c;
    while((c = fgetc(f1)) != EOF)
    {        
        if(c != '#')
            putchar(c);
        else
        {
			while((c = fgetc(f1)) != EOF)
                {
                    if(c == '\n')
                    {
                        break;
                    }
                }
        }

    }
    fclose(f1);
}


tokenInfo getNextToken(FILE *fp){
	tokenInfo tk;
	char ch;
	int state = 1;
	int lineno = 1;
	currentpos = 0;
	lexemecount = 0;
	while(1){
		fp = getStream(fp,30);
		ch = buffer[currentpos++];
		tk.lexeme[lexemecount++] = ch;
		tk.lineno = lineno;
		switch(state)
		{
			case 1: //start
				switch(ch){
					case '\n': lineno++;   //no break ?
					case ' ':
					case '\t': state = 2; break;
					case '#': state = 3; break;
					case '=': state = 4; break;
					case '.': state = 5; break;
					case '<': state = 6; break;
					case '>': state = 7; break;
					case '0':
					case '1':
					case '2':
					case '3':
					case '4':
					case '5':
					case '6':
					case '7':
					case '8':
					case '9': state = 8; break;
					case '_': state = 9; break;
					case '"': state = 10; break;
					case 'a':
					case 'b':
					case 'c':
					case 'd':
					case 'e':
					case 'f':
					case 'g':
					case 'h':
					case 'i':
					case 'j':
					case 'k':
					case 'l':
					case 'm':
					case 'n':
					case 'o':
					case 'p':
					case 'q':
					case 'r':
					case 's':
					case 't':
					case 'u':
					case 'v':
					case 'w':
					case 'x':
					case 'y':
					case 'z': state = 11; break;
					case '[': tk.type = SQO; tk.lexeme[lexemecount++] ='\0' ; tk.name="SQO"; return tk;
					case ']': tk.type = SQC; tk.lexeme[lexemecount++] ='\0' ; tk.name="SQC"; return tk;
					case '(': tk.type = OP; tk.lexeme[lexemecount++] ='\0' ; tk.name="OP"; return tk;
					case ')': tk.type = CL; tk.lexeme[lexemecount++] ='\0' ; tk.name="CL"; return tk;
					case ';': tk.type = SEMICOLON; tk.lexeme[lexemecount++] ='\0' ; tk.name="SEMICOLON"; return tk;
					case ',': tk.type = COMMA; tk.lexeme[lexemecount++] ='\0' ; tk.name="COMMA"; return tk;
					case '+': tk.type = PLUS; tk.lexeme[lexemecount++] ='\0' ; tk.name="PLUS"; return tk;
					case '-': tk.type = MINUS; tk.lexeme[lexemecount++] ='\0' ; tk.name="MINUS"; return tk;
					case '*': tk.type = MUL; tk.lexeme[lexemecount++] ='\0' ; tk.name="MUL"; return tk;
					case '/': tk.type = DIV; tk.lexeme[lexemecount++] ='\0' ; tk.name="DIV"; return tk;
					case '@': tk.type = SIZE; tk.lexeme[lexemecount++] ='\0' ; tk.name="SIZE"; return tk;
				}
				break;
			
			case 2: //\t, space, \n
				switch(ch){
					case '\n': lineno++;
					case ' ':
                    case '\t': break;   // just break, capture 2 spaces/tabs/lines
					case '\0': tk.type = EOF; strcpy(tk.lexeme , "EOF"); return tk;
					default: currentpos--; state=1; break;
				}
				break;
			
			case 3: // #
				switch(ch){
					case '\n': lineno++; state = 1; break;
					// case '\0':  // error? break;
					default : state = 3; break; //can use while loop here
				}
				break;
			case 4: // =
				switch(ch){
					case '=': tk.type = EQ; tk.lexeme[lexemecount++] ='\0' ; tk.name="EQ"; return tk;
					case '/': state = 23; break;
					//case '\0': //error ? 
					default : currentpos--; tk.type = ASSIGNOP; tk.lexeme[lexemecount-1] ='\0' ; tk.name="ASSIGNOP"; return tk;
				}
				break;
			case 23: // =/
				switch(ch){
					case '=': tk.type = EQ; tk.lexeme[lexemecount++] ='\0' ; tk.name="NE"; return tk;
					default : state=59; break;
				}
			case 59:
				printf("\n %d: Lexical Error: Invalid Relational operator",tk.lineno); return tk; 
			case 5: // .
				switch(ch){
					case 'a': state=24; break;
					case 'o': state=27; break;
					case 'n': state=29; break;
					default : state=58; break;
				}
				break;
			case 24: //.a
				switch(ch){
					case 'n': state=25; break;
					default : state=58; break;
				}
				break;
			case 25: //.an
				switch(ch){
					case 'd': state=26; break;
					default : state=58; break;
				}
				break;
			case 26: //.and
				switch(ch){
					case '.': tk.type = AND; tk.lexeme[lexemecount++] ='\0' ; tk.name="AND"; return tk;
					default : state=58; break;
				}
				break;
			case 27: //.o
				switch(ch){
					case 'r': state=28; break;
					default : state=58; break;
				}
				break;
			case 28: //.or
				switch(ch){
					case '.': tk.type = OR; tk.lexeme[lexemecount++] ='\0' ; tk.name="OR"; return tk;
					default : state=58; break;
				}
				break;
			case 29: //.n
				switch(ch){
					case 'o': state=29; break;
					default : state=58; break;
				}
				break;
			case 30: //.no
				switch(ch){
					case 't': state=30; break;
					default : state=58; break;
				}
				break;
			case 31: //.not
				switch(ch){
					case '.': tk.type = NOT; tk.lexeme[lexemecount++] ='\0' ; strcpy(tk.name,"NOT"); return tk;
					default : state=58; break;
				}
				break;
			case 58:
				printf("\n %d: Lexical Error: Invalid Logical operator",tk.lineno); return tk; 
			case 6: // <
				switch(ch){
					case '=': tk.type = LE; tk.lexeme[lexemecount++] ='\0' ; strcpy(tk.name,"LE"); return tk;
					//case '\0': state=59; break;
					default : currentpos--; tk.type = LT; tk.lexeme[lexemecount-1] ='\0' ; strcpy(tk.name,"LT"); return tk;
				}
				break;
			case 7: // >
				switch(ch){
					case '=': tk.type = GE; tk.lexeme[lexemecount++] ='\0' ; strcpy(tk.name,"GE"); return tk;
					//case '\0': // error
					default : currentpos--; tk.type = GT; tk.lexeme[lexemecount-1] ='\0' ; strcpy(tk.name,"GT"); return tk;
				}
				break;
			case 8: // 1 (number)
				switch(ch){
					case '0':
					case '1':
					case '2':
					case '3':
					case '4':
					case '5':
					case '6':
					case '7':
					case '8':
					case '9': state = 8; break;
					case '.': state = 32; break;
					default : currentpos--; tk.type = NUM; tk.lexeme[lexemecount-1] ='\0' ; strcpy(tk.name,"NUM"); return tk;
				}
				break;
			case 32: //RNUM a.
				switch(ch){
					case '0':
					case '1':
					case '2':
					case '3':
					case '4':
					case '5':
					case '6':
					case '7':
					case '8':
					case '9': state = 33; break;
					case 'a':
					case 'o':
					case 'n': currentpos--; tk.type = NUM; tk.lexeme[lexemecount-1] ='\0' ; strcpy(tk.name,"NUM"); return tk;
					default : state = 54; break;
				}
				break;
			case 33: //RNUM a.b
				switch(ch){
					case '0':
					case '1':
					case '2':
					case '3':
					case '4':
					case '5':
					case '6':
					case '7':
					case '8':
					case '9': tk.type = RNUM; tk.lexeme[lexemecount++] ='\0' ; strcpy(tk.name,"RNUM"); return tk;
					default : //check
								currentpos--;tk.type = RNUM; tk.lexeme[lexemecount++] ='\0' ; strcpy(tk.name,"RNUM"); return tk;
				}
				break;
			case 54: //number.<char>
				printf("\n %d: Lexical Error: Invalid Real Number '%s'",tk.lineno,tk.lexeme); return tk; // or maybe space not given
			case 9: // _
				switch(ch){
					case 'm': state = 34; break;
					case 'a':
					case 'b':
					case 'c':
					case 'd':
					case 'e':
					case 'f':
					case 'g':
					case 'h':
					case 'i':
					case 'j':
					case 'k':
					case 'l':
					case 'n':
					case 'o':
					case 'p':
					case 'q':
					case 'r':
					case 's':
					case 't':
					case 'u':
					case 'v':
					case 'w':
					case 'x':
					case 'y':
					case 'z': state = 38; break;
					case '0':
					case '1':
					case '2':
					case '3':
					case '4':
					case '5':
					case '6':
					case '7':
					case '8':
					case '9': state = 55; break;
					default : state = 56; break
				}
				break;
			case 55:
				printf("\n %d: Lexical Error: Invalid Function Name '%s'. _ cannot be followed by a number",tk.lineno,tk.lexeme); return tk;
			case 56:
				printf("\n %d: Lexical Error: Function Name '%s' contain invalid character",tk.lineno,tk.lexeme); return tk;
			case 34: // _m
				switch(ch){
					case 'a': state = 35; break;
					case 'b':
					case 'c':
					case 'd':
					case 'e':
					case 'f':
					case 'g':
					case 'h':
					case 'i':
					case 'j':
					case 'k':
					case 'l':
					case 'm':
					case 'n':
					case 'o':
					case 'p':
					case 'q':
					case 'r':
					case 's':
					case 't':
					case 'u':
					case 'v':
					case 'w':
					case 'x':
					case 'y':
					case 'z': 
					case '0':
					case '1':
					case '2':
					case '3':
					case '4':
					case '5':
					case '6':
					case '7':
					case '8':
					case '9': state = 38; break;
					case '[':
					case '(': 
					case ' ': currentpos--;tk.type = FUNID; tk.lexeme[lexemecount++] ='\0' ; strcpy(tk.name,"FUNID"); return tk;
					default: state = 56; break;
				}
				break;
			case 35: // _ma
				switch(ch){
					case 'i': state = 36; break;
					case 'a':
					case 'b':
					case 'c':
					case 'd':
					case 'e':
					case 'f':
					case 'g':
					case 'h':
					case 'j':
					case 'k':
					case 'l':
					case 'm':
					case 'n':
					case 'o':
					case 'p':
					case 'q':
					case 'r':
					case 's':
					case 't':
					case 'u':
					case 'v':
					case 'w':
					case 'x':
					case 'y':
					case 'z': 
					case '0':
					case '1':
					case '2':
					case '3':
					case '4':
					case '5':
					case '6':
					case '7':
					case '8':
					case '9': state = 38; break;
					case '[':
					case '(': 
					case ' ': currentpos--;tk.type = FUNID; tk.lexeme[lexemecount++] ='\0' ; strcpy(tk.name,"FUNID"); return tk;
					default: state = 56; break;
				}
				break;
			case 36: //_mai
				switch(ch){
					case 'n': state = 37; break;
					case 'a':
					case 'b':
					case 'c':
					case 'd':
					case 'e':
					case 'f':
					case 'g':
					case 'h':
					case 'i':
					case 'j':
					case 'k':
					case 'l':
					case 'm':
					case 'o':
					case 'p':
					case 'q':
					case 'r':
					case 's':
					case 't':
					case 'u':
					case 'v':
					case 'w':
					case 'x':
					case 'y':
					case 'z': 
					case '0':
					case '1':
					case '2':
					case '3':
					case '4':
					case '5':
					case '6':
					case '7':
					case '8':
					case '9': state = 38; break;
					case '[':
					case '(': 
					case ' ': currentpos--;tk.type = FUNID; tk.lexeme[lexemecount++] ='\0' ; strcpy(tk.name,"FUNID"); return tk;
					default: state = 56; break;
				}
				break;
			case 37: //_main
				switch(ch){
					case 'a':
					case 'b':
					case 'c':
					case 'd':
					case 'e':
					case 'f':
					case 'g':
					case 'h':
					case 'i':
					case 'j':
					case 'k':
					case 'l':
					case 'm':
					case 'n':
					case 'o':
					case 'p':
					case 'q':
					case 'r':
					case 's':
					case 't':
					case 'u':
					case 'v':
					case 'w':
					case 'x':
					case 'y':
					case 'z': 
					case '0':
					case '1':
					case '2':
					case '3':
					case '4':
					case '5':
					case '6':
					case '7':
					case '8':
					case '9': state = 38; break;
					case '[':
					case '(': 
					case ' ': currentpos--;tk.type = MAIN; tk.lexeme[lexemecount++] ='\0' ; strcpy(tk.name,"MAIN"); return tk;
					default: state = 56; break;
				}
				break;
			case 38: //_funntion
				switch(ch){
					case 'a':
					case 'b':
					case 'c':
					case 'd':
					case 'e':
					case 'f':
					case 'g':
					case 'h':
					case 'i':
					case 'j':
					case 'k':
					case 'l':
					case 'm':
					case 'n':
					case 'o':
					case 'p':
					case 'q':
					case 'r':
					case 's':
					case 't':
					case 'u':
					case 'v':
					case 'w':
					case 'x':
					case 'y':
					case 'z': 
					case '0':
					case '1':
					case '2':
					case '3':
					case '4':
					case '5':
					case '6':
					case '7':
					case '8':
					case '9': state = 38; break;
					case '[':
					case '(': 
					case ' ':if (lexemecount<20){
								tk.type = FUNID; tk.lexeme[lexemecount++] ='\0' ; strcpy(tk.name,"FUNID"); return tk;
							 }
							 else{
								state=57;break;
							 }
					default: state = 56; break;
				}
				break;
			case 57:
				printf("\n %d: Lexical Error: Function Name '%s' exceeds the maximum length limit of 20",tk.lineno,tk.lexeme); return tk;
			case 10: // "
				switch(ch){
					case 'a':
					case 'b':
					case 'c':
					case 'd':
					case 'e':
					case 'f':
					case 'g':
					case 'h':
					case 'i':
					case 'j':
					case 'k':
					case 'l':
					case 'm':
					case 'n':
					case 'o':
					case 'p':
					case 'q':
					case 'r':
					case 's':
					case 't':
					case 'u':
					case 'v':
					case 'w':
					case 'x':
					case 'y':
					case 'z':
					case ' ': state=39; break;
					case '"': state = 51; break;  //"" length=2, no string
					default : state = 52; break;
				}
				break;
			case 39: //"a
				switch(ch){
					case 'a':
					case 'b':
					case 'c':
					case 'd':
					case 'e':
					case 'f':
					case 'g':
					case 'h':
					case 'i':
					case 'j':
					case 'k':
					case 'l':
					case 'm':
					case 'n':
					case 'o':
					case 'p':
					case 'q':
					case 'r':
					case 's':
					case 't':
					case 'u':
					case 'v':
					case 'w':
					case 'x':
					case 'y':
					case 'z':
					case ' ': state=39; break;
					case '"':{ 
						if (lexemecount<=20){
							tk.type = STR; tk.lexeme[lexemecount++] ='\0' ; strcpy(tk.name,"STR"); return tk;
						}
						else{
							state = 53; break;
						}
					}
					default: state = 52; break;
				}
				break;
			case 51:
				printf("\n %d: Lexical Error: Empty String",tk.lineno); return tk;
			case 52:
				printf("\n %d: Lexical Error: String contains invalid character",tk.lineno); return tk;
			case 53:
				printf("\n %d: Lexical Error: String '%s' exceeds the maximum length of 20",tk.lineno,tk.lexeme); return tk; //maybe add \0 in lexeme
			
			case 11: 
				switch(ch){
					case 'a':
					case 'b':
					case 'c':
					case 'd':
					case 'e':
					case 'f':
					case 'g':
					case 'h':
					case 'i':
					case 'j':
					case 'k':
					case 'l':
					case 'm':
					case 'n':
					case 'o':
					case 'p':
					case 'q':
					case 'r':
					case 's':
					case 't':
					case 'u':
					case 'v':
					case 'w':
					case 'x':
					case 'y':
					case 'z': state=11; break;
					case '0':
					case '1':
					case '2':
					case '3':
					case '4':
					case '5':
					case '6':
					case '7':
					case '8':
					case '9': 
					default : currentpos--;
						if (lexemecount<20){
							tk.lexeme[lexemecount-1] ='\0' ;
							char name[50];
							int type = isKeyword(tk,name);
							if(type!=-1){
								tk.type = (character)type; strcpy(tk.name,name);
							}
							else{
								tk.type = ID;  strcpy(tk.name,"ID");
							}
						}
						else{
							state=60; break;
						}
				}
				break;
			case 60:
				printf("\n %d: Lexical Error: ID '%s' exceeds the maximum length of 20",tk.lineno,tk.lexeme); return tk; 
	}
}


// We knoe the length error at 20 only, no need to go all through the string