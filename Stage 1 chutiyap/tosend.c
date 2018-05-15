#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
//#include "lexer.h"

#define MAX_SIZE 30
char buffer2[MAX_SIZE];
int buffersize;
int lineno = 1;

char* keywords[]={"end","int","real","string","matrix","main","if","else","endif","read","print","function"};
char* keywords2[]={"END","INT","REAL","STRING","MATRIX","MAIN","IF","ELSE","ENDIF","READ","PRINT","FUNCTION"};

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
	char name[60];
}tokenInfo;

int isKeyword(tokenInfo tk,char* name){
	int i;
	int noOfKeywords = 12;
	//printf("In keyword %s",tk.lexeme);
	for(i=0;i<noOfKeywords;i++){
		if(strcmp(keywords[i],tk.lexeme)==0){
			//printf("Matched");
			//name = keywords[i];
			strcpy(name,keywords2[i]);
			return i;
		}
	}
	return -1; //not a keyword
}

int currentpos=0;

// FILE *getStream(FILE *fp,int k) //k is buffersize
//     {
//         size_t file_size; 
//         int buffersize =50;  
//         int MAXIMUM=50;                             //size of the file
//         if (fp == NULL) {
// 			printf("No file");
// 			return fp;
// 		}
// 		file_size=fread(buffer2,sizeof(char),MAXIMUM,fp);   //allocating buffer2 with file contents
//         if(file_size==0){
// 			buffer2[0]=EOF;
// 		}
// 		else if(file_size<sizeof(char)*buffersize){
// 			buffer2[file_size] =EOF;
// 		}
// 		buffer2[buffersize]='\0';
// 		//if(feof(fp))B[file_size]='$';            //assigning bottom-marker of buffer2 with $
//         return fp;
//     }

FILE *getStream(FILE *fp,char* B,int k)
    {
        int file_size;                                //size of the file
        file_size=fread(B,sizeof(char),k,fp);   //allocating buffer2 with file contents
        if(feof(fp))B[file_size]='$';            //assigning bottom-marker of buffer2 with $
        return fp;
    }

int buffer_size = -1;

char getChar(FILE* f)
{
    if(buffer_size == -1 || currentpos == buffer_size)
    {
        buffer_size = fread(buffer2 , sizeof(char) , MAX_SIZE , f);
        currentpos = 0;
    }

    if(buffer_size == 0)
        return '\0';
    else
        return buffer2[currentpos++];
}

tokenInfo getNextToken(FILE *fp){
	tokenInfo tk;
	char ch;
	int state = 1;
	//currentpos = 40;
	int lexemecount = 0;
	int MAXIMUM=30;
	//fp=getStream(fp,buffer2,MAXIMUM);
	while(1){
		//fp = getStream(fp,30);

		// if(currentpos>MAXIMUM-1)
  //       {
  //       	currentpos=0;
  //           //index=0; k=1;
		// 	strcpy(buffer2,"");
  //           fp=getStream(fp,buffer2,MAXIMUM);
  //       }
		//printf("%s this read\n",buffer2);
		//ch = buffer2[currentpos++];
		ch = getChar(fp);  
		//printf("State %d -- %c->",state,ch);
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
					default : tk.lexeme[lexemecount++] = ch; tk.lexeme[lexemecount++] ='\0'; tk.type=ERROR; printf("\nLine No.%d: Lexical Error: Unknown Symbol '%s'. ",tk.lineno,tk.lexeme); return tk; 
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
					case '\0':  return;// error? break;
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
					currentpos--; tk.lexeme[lexemecount++] = ch; tk.lexeme[lexemecount++] ='\0'; printf("\nLine No.%d: Lexical Error: Invalid Relational operator '%s' ",tk.lineno,tk.lexeme);tk.type=ERROR; return tk;
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
					currentpos--; tk.lexeme[lexemecount++] = ch; tk.lexeme[lexemecount++] ='\0' ; printf("\nLine No.%d: Lexical Error: Invalid Logical operator '%s'",tk.lineno,tk.lexeme); tk.type=ERROR; return tk;
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
					currentpos--; tk.lexeme[lexemecount++] = ch; tk.lexeme[lexemecount++] ='\0' ; printf("\nLine No.%d: Lexical Error: Invalid Real Number '%s'",tk.lineno,tk.lexeme); tk.type=ERROR; return tk; // or maybe space not given
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
					currentpos--; tk.lexeme[lexemecount++] = ch; tk.lexeme[lexemecount++] ='\0' ; printf("\nLine No.%d: Lexical Error: Invalid Function Name '%s'. _ cannot be followed by a number",tk.lineno,tk.lexeme); tk.type=ERROR; return tk;
				case 56:
					currentpos--; tk.lexeme[lexemecount++] = ch; tk.lexeme[lexemecount++] ='\0' ; printf("\nLine No.%d: Lexical Error: Function '%s' contain invalid character. Only alphabets and 1 number(at the end,optional) is allowed.",tk.lineno,tk.lexeme); tk.type=ERROR; return tk;
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
					currentpos--; printf("\nLine No.%d: Lexical Error: Function Name '%s' exceeds the maximum length limit of 20",tk.lineno,tk.lexeme); tk.type=ERROR; return tk;
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
					 printf("\nLine No.%d: Lexical Error: Empty String",tk.lineno); tk.type=ERROR; return tk;
				case 52:
					 printf("\nLine No.%d: Lexical Error: String contains invalid character",tk.lineno); tk.type=ERROR; return tk;
				case 53:
					printf("\nLine No.%d: Lexical Error: String '%s' exceeds the maximum length of 20",tk.lineno,tk.lexeme); tk.type=ERROR; return tk; //maybe add \0 in lexeme
				
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
									tk.type = (character)type; strcpy(tk.name,name);
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
									tk.type = (character)type; strcpy(tk.name,name);
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
					currentpos--; printf("\nLine No.%d: Lexical Error: ID '%s' exceeds the maximum length of 20",tk.lineno,tk.lexeme); tk.type=ERROR; return tk;
		}
	}
}


int printtokens(FILE *fp)
{
  tokenInfo to;
  FILE *pFile;
  pFile = fp;
  printf("TOKEN  line  Name  LEXEME\n");
  to=getNextToken(pFile);
  while(to.type!=EOF1)
    {
    	//printf("here");
      
   //    if(to.flag==0)
   //    {
	  // printf("\n");
   //        int i;
   //        for(i=0; to.lexeme[i]!='\0'; ++i);
   //        i--;
   //        int k=0;
   //        printf("%8d%14s%6s",to.line_no,to.name," ");//print to screen
   //        for(k=0;k<=i;k++)
   //        {
   //            char ch=to.lexeme[k];
   //            if(isAllow(to.lexeme[k])&&!(isWhitespace(to.lexeme[k])))
   //            if(k==i&&to.lexeme[k]!='\0'){printf("%c",to.lexeme[k]);}
   //            else if(k<i){printf("%c",to.lexeme[k]);}
   //        }
   //    }
    	//if(to!=NULL)
    	if(to.type!=ERROR)
     		 printf("\n%d  %d  %s  %s\n",to.type,to.lineno,to.name,to.lexeme);
      to=getNextToken(pFile);
   }
   //printf("%s",to.name);
  fclose (pFile);
  return 0;
}
//int main(){
//	printf("yeha");
//	FILE *fp = fopen("a.txt","r");
//	printtokens(fp);
//}
