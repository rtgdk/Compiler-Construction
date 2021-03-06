/*
Rohit Lodha
2015A7PS0040P
*/
#include "lexer.h"
#include "parser.h"
#include "ast.h"
#include "symbolTable.h"
#include "semantics.h"
#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define BLU   "\x1B[34m"
#define RESET "\x1B[0m"

int main(int argc, char *argv[])
{
	if(argc!=3)
	{
		printf("%sProper Arguments not given\n",RED);
		printf(RESET);
		return 0;
	}
	int n;
	initialiseKeywordHash();
	initHashtable();
	createGrammar("grammar.txt");
	initialisefirstandfollowMatrix();
	findFirstSet();
	findFollowSet();
	initParseTable();
	createParseTable();
	createHashtable3();
	initSymbolTable();
	initialiseSemantics();
	parsetree PT;
	AStree ast1,ast2;
	printf("%sLEVEL 4: Symbol Table, Type Checking, Semantic Rules\n",BLU);
	printf(RESET);
	int n1=0;
	int n2=0;
	int n3=0;
	int n4=0;
	int n5=0;
	int n6=0;
	int pterror1 = 0;
	while(1){
		printf("%s\nPress option for the defined task\n0 : To quit\n1 : For printing the token list (on the console) generated by the lexer.\n2 : For parsing to verify the syntactic correctness of the input source code\n3 : For printing AST on the console\n4 : For printing compression ratio\n5 : For printing Symbol table\n6 : For verifying syntactic and semantic correctness\n7 : Code Generation\nOption:",YEL);
		printf(RESET);
		scanf("%d",&n);
		if(n==1){
			initialiseLexer();
			printtokens(argv[1]);
			n1 = 1;
		}
		else if(n==2){
		    if(n1==1){
			    PT = parseInputSourceCode(argv[1],parsetable,&pterror1);
			    printf("     lexCurNode lineno          token valIfNum          parentNodeSymbol isLeaf               NodeSymbol\n");
	     		printf("--------------------------------------------------------------------------------------------------------\n");
			    printParseTree(PT);
			    n2=1;
			    if(pterror1==1){
	                printf("%sSyntax Error(s) found! Printed above the Parse tree\n",RED);
	            }
			    }
			else{
			    printf("%sFirst run Lexer using option 1\n",RED);
			    printf(RESET); 
			}
		}
		else if(n==3){
		    if(n2==1){
		        ast1 = makeAST(PT,0);
		        printf("\n%sInorder Traversal of AST\n\n",GRN);
		        printf(RESET);
		        printf("     lexCurNode lineno          token valIfNum          parentNodeSymbol isLeaf               NodeSymbol\n");
	     		printf("--------------------------------------------------------------------------------------------------------\n");
		        printASTree(ast1);
		        n3=1;
	        }
	        else{
	             printf("%sFirst create ParserTree using option 2\n",RED);
			     printf(RESET);  
	        }
		}
		else if(n==4){
		    if(n3==1){
		        
			    calculateCompression();
			    n4=1;
		    }
		    else{
		        printf("%sFirst create AST using option 3\n",RED);
			    printf(RESET); 
		    }
		}
		else if(n==5){
            initSymbolTable();
            if(n3==1){
                makeST(PT);
                printf("%sSymbol Table \n\n",GRN);
                printf(RESET);
		        printSTUtility();
		        n5=1;
	        }
	        else{
	            printf("%sFirst create AST using option 3\n",RED);
			    printf(RESET); 
	        }
		}
		else if(n==6){
		    initialiseKeywordHash();
	        initHashtable();
	        createGrammar("grammar.txt");
	        initialisefirstandfollowMatrix();
	        findFirstSet();
	        findFollowSet();
	        initParseTable();
	        createParseTable();
	        createHashtable3();
	        initSymbolTable();
	        initialiseLexer();
	        initSymbolTable();
	        initialiseSemantics();
	        pterror1=0;
	        PT = parseInputSourceCode(argv[1],parsetable,&pterror1);
	        if(pterror1==1){
	            printf("%sSyntax Error(s) found!\n",RED);
	        }
	        else{
	            makeST(PT);
	            ast2 = makeAST(PT,1);
	            bool b = semanticAnalysisUtil(ast2);
	            if(b==true){
	                printf("%sInput Source Code is Semantically correct\n",GRN);
	                printf("%sCode compiles successfully..........\n",GRN);
	                }
                else
                     printf("%Semantic Error(s) found!\n",RED);
             }
             printf(RESET);
		}
		else if(n==7){
		    printf("%sCode Generation not implemented\n",RED);
		    printf(RESET);
		}
		else{
			printf("%sQuiting\n",RED);
			printf(RESET);
			break;
		}
	}
	return 0;
}
