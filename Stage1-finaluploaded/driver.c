/*
Rohit Lodha
2015A7PS0040P
*/
#include "lexer.h"
#include "parser.h"
#include "ast.h"
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
	//printParseTable();
	int ptdone=0;
	parsetree PT;
	AStree ast;
	printf("%s(a) FIRST and FOLLOW set automated\n(b) Both lexical and syntax analysis modules implemented and running.\n(c) Error recovery using synchronized set implemented.\n(d) Hashing used for keyword detection.\n",BLU);
	printf(RESET);
	while(1){
		printf("%s\nPress option for the defined task\n1 : For removal of comments ‐ print the comment free code on the console\n2 : For printing the token list (on the console) generated by the lexer.\n3 : For parsing to verify the syntactic correctness of the input source code\n4 : For printing the parse tree in the file appropriately\n5 : For printing AST\n6 : For printing ST\n7 : Quit\nOption:",YEL);
		printf(RESET);
		scanf("%d",&n);
		if(n==1){
			initialiseLexer();
			removeComments(argv[1]);
		}
		else if(n==2){
		    initialiseLexer();
			printtokens(argv[1]);
			//fclose(fp1);
		}
		else if(n==3){
			initialiseLexer();
			PT = parseInputSourceCode(argv[1],parsetable);
			ptdone = 1;
		}
		else if(n==4){
			initialiseLexer();
			FILE *fp;
	 		fp = fopen (argv[2],"w");
	 		fprintf(fp,"     lexCurNode lineno          token valIfNum          parentNodeSymbol isLeaf               NodeSymbol\n");
	 		fprintf(fp,"--------------------------------------------------------------------------------------------------------\n");
			if(ptdone==1){
				printParseTree(PT,fp);
			}
			else{
				PT = parseInputSourceCode(argv[1],parsetable);
				printParseTree(PT,fp);
			}
			fclose(fp);
			printf("%sParse Tree written in File\n",GRN);
			printf(RESET);
		}
		else if(n==5){
		    initialiseLexer();
		    PT = parseInputSourceCode(argv[1],parsetable);
		    ast = makeAST(PT);
		    printf("\n Inorder Traversal of AST\n\n");
		    printf("     lexCurNode lineno          token valIfNum          parentNodeSymbol isLeaf               NodeSymbol\n");
	 		printf("--------------------------------------------------------------------------------------------------------\n");
		    printASTree(ast);
		}
		else if(n==6){
		    initialiseLexer();
		    PT = parseInputSourceCode(argv[1],parsetable);
		    //printf("PT succesfull %s\n",PT->ruleNode->name);
		    makeST(PT);
            //printf("ST succesfull\n");
		    printST();
		}
		else{
			printf("%sQuiting\n",RED);
			printf(RESET);
			break;
		}
	}
	return 0;
}
