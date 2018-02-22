#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "lexer.h"

#define MAX_SIZE 30
char buffer[MAX_SIZE];
int buffersize

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
	
}

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
19. -
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

**/
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
			case 1:
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
			
			case 2:
				switch(ch){
					case '\n': lineno++;
					case ' ':
                    case '\t': break;   // just break, capture 2 spaces/tabs/lines
					case '\0': tk.type = EOF; strcpy(tk.lexeme , "EOF"); return tk;
					default: currentpos--; state=1; break;
				}
				break;
			
			case 3:
				switch(ch){
					case '\n': lineno++; state =1; break;
					case '\0':  // error? break;
					default : state = 3; break;
				}
				break;
			case 4: // =
				switch(ch){
					case '=': tk.type = EQ; tk.lexeme[lexemecount++] ='\0' ; tk.name="EQ"; return tk;
					case '/': state = 23; break;
					case '\0': //error ? 
					default : currentpos--; tk.type = ASSIGNOP; tk.lexeme[lexemecount-1] ='\0' ; tk.name="ASSIGNOP"; return tk;
				}
				break;
			case 5: // .
				switch(ch){
					case 'a': state=24; break;
					case 'o': state=27; break;
					case 'n': state=29; break;
					default : //error?
				}
				break;
			case 24:
				switch(ch){
					case 'n': state=25; break;
					default : //error?
				}
				break;
			case 25:
				switch(ch){
					case 'd': state=26; break;
					default : //error?
				}
				break;
			case 26:
				switch(ch){
					case '.': tk.type = AND; tk.lexeme[lexemecount++] ='\0' ; tk.name="AND"; return tk;
					default : //error?
				}
				break;
			case 27:
				switch(ch){
					case 'r': state=28; break;
					default : //error?
				}
				break;
			case 28:
				switch(ch){
					case 'r': tk.type = OR; tk.lexeme[lexemecount++] ='\0' ; tk.name="OR"; return tk;
					default : //error?
				}
				break;
			case 29:
				switch(ch){
					case 'o': state=29; break;
					default : //error?
				}
				break;
			case 30:
				switch(ch){
					case 't': state=30; break;
					default : //error?
				}
				break;
			case 31:
				switch(ch){
					case '.': tk.type = NOT; tk.lexeme[lexemecount++] ='\0' ; strcpy(tk.name,"NOT"); return tk;
					default : //error?
				}
				break;
			case 6: // <
				switch(ch){
					case '=': tk.type = LE; tk.lexeme[lexemecount++] ='\0' ; strcpy(tk.name,"LE"); return tk;
					case '\0': // error
					default : currentpos--; tk.type = LT; tk.lexeme[lexemecount-1] ='\0' ; strcpy(tk.name,"LT"); return tk;
				}
				break;
			case 7: // >
				switch(ch){
					case '=': tk.type = GE; tk.lexeme[lexemecount++] ='\0' ; strcpy(tk.name,"GE"); return tk;
					case '\0': // error
					default : currentpos--; tk.type = GT; tk.lexeme[lexemecount-1] ='\0' ; strcpy(tk.name,"GT"); return tk;
				}
				break;
			case 8:
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
					default : //error ?
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
				}
				break;
			case 9:
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
					case '9': //error ; break
					default : //error? // case for [,] (, )
				}
				break;
			case 34:
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
					case '(': tk.type = FUNID; tk.lexeme[lexemecount++] ='\0' ; strcpy(tk.name,"FUNID"); return tk;
					default: //error?
				}
				break;
			case 35:
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
					case '(': tk.type = FUNID; tk.lexeme[lexemecount++] ='\0' ; strcpy(tk.name,"FUNID"); return tk;
					default: //error?
				}
				break;
			case 36:
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
					case '(': tk.type = FUNID; tk.lexeme[lexemecount++] ='\0' ; strcpy(tk.name,"FUNID"); return tk;
					default: //error?
				}
				break;
			case 37:
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
					case '(': tk.type = MAIN; tk.lexeme[lexemecount++] ='\0' ; strcpy(tk.name,"MAIN"); return tk;
					default: //error?
				}
				break;
			case 38:{
				if (lexemecount<20){
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
						case '(': tk.type = FUNID; tk.lexeme[lexemecount++] ='\0' ; strcpy(tk.name,"FUNID"); return tk;
						default: //error?
					}
				}
					else{
						//error
					}
			}
			break;
			case 10:
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
					case 'z': state=39; break;
					default : //error
				}
				break;
			case 39:
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
					case 'z': state=40; break;
					default: //error
				}
			case 40:
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
					case 'z': state=39; break;
					case '"':{ 
						if (lexemecount<=20){
							tk.type = STR; tk.lexeme[lexemecount++] ='\0' ; strcpy(tk.name,"STR"); return tk;
						}
						else{
							//error
						}
					}
					default: //error
		`		}
				break;
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
				default : {
					if (lexemecount<20){
						tk.type = ID; tk.lexeme[lexemecount++] ='\0' ; strcpy(tk.name,"ID"); return tk;
					}
					else{
						//error
					}
					}
				}
				break;
	}
}
