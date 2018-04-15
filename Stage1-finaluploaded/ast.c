/*
Rohit Lodha
2015A7PS0040P
*/
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <stdbool.h>
#include "ast.h"
#include "parser.h"
#include "lexer.h"
#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define RESET "\x1B[0m"

char* allowedterminals[NO_OF_ALLOWED_TERMINALS] = {"MAIN",
	"END",
	"FUNCTION",
	"FUNID",
	"ID",
	"INT",
	"REAL",
	"STRING",
	"MATRIX",
	"READ",
	"PRINT",
	"PLUS",
	"MINUS",
	"MUL",
	"DIV",
	"NOT",
	"NUM",
	"RNUM",
	"STR",
	"AND",
	"OR",
	"LT",
	"LE",
	"EQ",
	"GT",
	"GE",
	"NE"
};

/*
For hashing Allowed Terminals to bucket no in HashTable
*/
int hash3(char* str)
{
	unsigned long hash = 5381;
	int c;
	while (c = *str++)
	    hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
	return (int)(hash % hashtablesize2);
}

/*
Initialise NO_OF_ALLOWED_TERMINALS HashTable
*/
void createHashtable3(){
	int i=0;
	for (i=0;i<hashtablesize2;i++){
		HashTable2[i] = NULL;
	}
	int hashvalue;
	for (i=0;i<NO_OF_ALLOWED_TERMINALS;i++){
		hashvalue = hash3(allowedterminals[i]);
		if(HashTable2[hashvalue]==NULL){
			HashTable2[hashvalue] = (hashtable2*)malloc(sizeof(hashtable2));
			HashTable2[hashvalue]->next = NULL;
			strcpy(HashTable2[hashvalue]->name,allowedterminals[i]);
		}
		else{
			hashtable2* temp = HashTable2[hashvalue];
			while(temp->next!=NULL){
				temp=temp->next;
			}
			temp->next = (hashtable2*)malloc(sizeof(hashtable2));
			temp->next->next = NULL;
			strcpy(temp->next->name,allowedterminals[i]);
		}
	}
	return;
}

/*
Find if name is present in HashTable2
*/
int present2(char* name){
    //printf("Searching for %s\n",name);
	int hashvalue = hash3(name);
	if(HashTable2[hashvalue]==NULL){
		return 0;
	}
	else{
		hashtable2* temp = HashTable2[hashvalue];
		while(temp->next!=NULL && strcmp(temp->name,name)!=0){
			temp=temp->next;
		}
		if(strcmp(temp->name,name)==0){
			return 1;
		}
		else{
			return 0;
		}
		
	}
}

/*
For printing HashTable2
*/
void printHashTable2(){
	int i;
	for (i=0;i<hashtablesize2;i++){
		hashtable2* temp = HashTable2[i];
		printf("%d-->",i);
		while(temp!=NULL){
			printf("%s,",temp->name);
			temp=temp->next;
		}
		printf("\n");
	}
}

/*
Update tree token and returns next
*/
AStree tokenizeTree2(AStree tree,tokenInfo tk){
	strcpy(tree->tk.name,tk.name);
	tree->tk.type = tk.type;
	tree->tk.lineno = tk.lineno;
	strcpy(tree->tk.lexeme,tk.lexeme);
	//tree = next2(tree);
	return tree;
}

/*
create parse tree
*/
AStree createASTree(parsetree pt){
    //printf("Creating %s\n",pt->ruleNode->name);
	AStree root = (AStree)malloc(sizeof(astnode));
	// strcpy(root->name,pt->ruleNode->name);
	root->ruleNode = (GrammarNode)malloc(sizeof(grammarnode));
	strcpy(root->ruleNode->name, pt->ruleNode->name);
	root->ruleNode->next = NULL;
	root->ruleNode->prev = NULL;
	root->noc = 0;
	root->parent = NULL;
	root->child = NULL;
	root->child = (struct astnode**)malloc((root->noc+1)*sizeof(struct astnode*));
	root->child[0] =NULL;
	root->next = NULL;
	if(root->ruleNode->name[0]!='<'){
		tokenizeTree2(root,pt->tk);
		root->ruleNode->type = 0; //coz NT
	}
	else{
		root->ruleNode->type = 1; //coz NT
	}
	return root;
}


/*
Add "allowed" children from the parent parse tree node
*/
AStree addChildren2(AStree tree, parsetree pt){ 
	tree->child = (struct astnode**)realloc(tree->child,(tree->noc+1)*sizeof(struct astnode*));
	tree->child[tree->noc] = createASTree(pt);
	tree->child[tree->noc]->parent = tree;
	tree->noc++;
	if(tree->noc > 1){
		tree->child[tree->noc-2]->next = tree->child[tree->noc-1];
	}
	return tree;
}

/*
returns next of the tree
*/
AStree next2(AStree tree){
	if(tree->next!=NULL){
		return tree->next;
	}
	else{
		if(tree->parent==NULL) return tree; //main function
		else{
			while(tree->parent!=NULL){
				tree = tree->parent;
				if(tree->next!=NULL) return tree->next;
			}
			return tree;
		}
	}
}

/*
Convert Parse to AST, called only for non terminals (cannot be null)
*/
AStree parseToAST(parsetree PT){
	//if (PT==NULL) return NULL;
	AStree ast =  createASTree(PT);
	//printf("First child is %s\n", PT->child[0]->ruleNode->name);
	AStree astChild;
	int flag = 0;
	int i;
	if(PT->child[0]!=NULL){
		for(i =0;i<PT->noc;i++){
			if(PT->child[i]->ruleNode->name[0]=='<'){
			    //printf("Going for child %s\n",PT->child[i]->ruleNode->name); 
				astChild = parseToAST(PT->child[i]);
				if(astChild!=NULL){ // to avoid more_ids->epsilon situation
					flag = 1;
					ast->child = (struct astnode**)realloc(ast->child,(ast->noc+1)*sizeof(struct astnode*));
					ast->child[ast->noc] = astChild;
					ast->child[ast->noc]->parent = ast;
					ast->noc++;
					if(ast->noc > 1){
						ast->child[ast->noc-2]->next = ast->child[ast->noc-1];
					}
				}
			}
			else if (present2(PT->child[i]->ruleNode->name)){
				flag = 1;
				ast = addChildren2(ast,PT->child[i]);
			}
		}
	}
	if(flag==0) {
	    //printf("Returnign Null from parsetoast for %s",ast->ruleNode->name);
	    return NULL; // to avoid more_ids->epsilon situation
    }
	else return ast;
}

/*
Called only if ast has only 1 child with a non terminal node 
which can be pulled up, childNo is the child no of ast from 
its parent node
*/
void pruneAST(AStree ast,int childNo){
	AStree tree;
	tree = ast;
	if(tree->noc==0) return; // For terminals (Check for exceptions##)
	if (tree->parent!=NULL){
		if(tree->child[0]->noc==1){
			pruneAST(tree->child[0],0);
		}
		tree->parent->child[childNo] = tree->child[0];
		tree->parent->child[childNo]->parent = tree->parent;
		tree->parent->child[childNo]->next = tree->next;
		if(childNo!=0){
			tree->parent->child[childNo-1]->next = tree->parent->child[childNo];
		}
		free(tree);
	}
	return;
}

/*
 To pull up the operator 
*/
void operatorAST(AStree ast,int childNo){
	AStree tree = ast;
	AStree tree2;
	int i=0;
	//tokenizeTree2(tree->parent, tree->tk);
	for(i=0;i<ast->parent->parent->noc;i++){
		if(strcmp(ast->parent->parent->child[i]->ruleNode->name,ast->parent->ruleNode->name)==0){
			break;
		}
	}
	ast->parent->parent->child[i] = ast;
	tree = ast->parent->parent;
	ast->next = ast->parent->next;
	ast->noc = ast->parent->noc - 1;
	for(i=0;i < ast->noc;i++){
		ast->child[i] = ast->parent->child[i+1];
	}
	ast->parent = tree;
	tree2 = ast->parent;
	//ast = tree->parent;
	return ast;
	free(tree2);
}

int isOperator(AStree ast){
    if(strcmp(ast->ruleNode->name,"PLUS")==0 || strcmp(ast->ruleNode->name,"MINUS")==0 || strcmp(ast->ruleNode->name,"MUL")==0 || strcmp(ast->ruleNode->name,"DIV")==0){
        return 1;
    }
    else{
        return 0;
    }
}
/* To modify the AST
*/
AStree modifyAST(AStree ast){
	if (ast==NULL) return NULL;
	int i=1;
	if(ast->child[0]!=NULL){
		if(ast->child[0]->noc==1){
			pruneAST(ast->child[0],0);
		}
		AStree tree = ast->child[0]->next;
		
		// if(isOperator(ast->child[0])){
		// 	ast = operatorAST(ast->child[0],0);

		// }
		// else{
			modifyAST(ast->child[0]);
		//}
		while(tree!=NULL){
			tree = tree->next;
			if(ast->child[i]->noc==1){
				pruneAST(ast->child[i],i);
			}
			// if(isOperator(ast->child[i])){
			// 	operatorAST(ast->child[i],i);
			// }
			// else{
				modifyAST(ast->child[i]);
			//}
			i++;
		}
	}
	return ast;
	
}

AStree makeAST(parsetree PT){
    //printf("Here %s\n",PT->ruleNode->name);
    AStree ast = parseToAST(PT);
    //printf("Here %s\n",ast->ruleNode->name);
    ast = modifyAST(ast);
    return ast;
}

void printAST(AStree ast) // Prefix
{
	if (ast==NULL) return;
	if(ast->parent!=NULL)
	    printf("%s-->%s\n",ast->parent->ruleNode->name,ast->ruleNode->name);
	else
	    printf("ROOT-->%s\n",ast->ruleNode->name);
	if(ast->child[0]!=NULL){
		printAST(ast->child[0]);
		AStree temp = ast->child[0]->next;
		//printToFile(PT,fp);
		while(temp!=NULL){
			printAST(temp);
			temp= temp->next;
		}
	}
	printf("\n");
	return;
}

void printAST2(AStree ast) // Prefix
{
	if (ast==NULL) return;
	printf("%s-->",ast->ruleNode->name);
	int i;
	for(i=0;i<ast->noc;i++){
	    printf("%s->",ast->child[i]->ruleNode->name);
	}
	printf("\n");
	for(i=0;i<ast->noc;i++){
		if(ast->child[i]->noc>0)
	    	printAST2(ast->child[i]);
	}
	//printf("\n");
	return;
}


/*
Print AST tree inorder traversal
*/
void printASTree(AStree PT, FILE* fp)
{
	if (PT==NULL) return;
	if(PT->child[0]!=NULL){
		printASTree(PT->child[0],fp);
		AStree temp = PT->child[0]->next;
		printToFile2(PT,fp);
		while(temp!=NULL){
			printASTree(temp,fp);
			temp= temp->next;
		}
	}
	else{
		printToFile2(PT,fp);
	}
	
	return;
}


/*
print a tree at a time in file 
*/
void printToFile2(AStree tr,FILE *fp)
{
	//printf("Tree %s\n",tr->tk.name);
    if((tr->ruleNode)->type==0)
        {
            //printf("%15s",tr->tk.lexeme);
            //printf("%7d",tr->tk.lineno);
            //printf("%15s",tr->tk.name);
            printf("%15s",tr->tk.lexeme);
            printf("%7d",tr->tk.lineno);
            printf("%15s",tr->tk.name);
        }
	else
        {
            //printf("      ---------");
            //printf("%7d",0);
            //printf("%s"," --------------");
            printf("      ---------");
            printf("%7d",0);
            printf("%s"," --------------");
        }
    if((tr->ruleNode)->type==0)
    {
        if(strcmp(tr->tk.name,"NUM")==0){
        	//printf("%9d",atoi(tr->tk.lexeme));
        	printf("%9d",atoi(tr->tk.lexeme));
		}
        else if(strcmp(tr->tk.name,"RNUM")==0){
        	//printf("%9f",atof(tr->tk.lexeme));
        	printf("%9f",atof(tr->tk.lexeme));
		}
        else {
        	//printf("%9s","----");
        	printf("%9s","----");
		}
    }
	else {
		//printf("%9s","----");
		printf("%9s","----");
		
	}
    if(tr->parent!=NULL){
    	//printf("%25s",tr->parent->ruleNode->name);
    	printf("%25s",tr->parent->ruleNode->name);
	}
    else {
    	//printf("%25s","ROOT");
    	printf("%25s","ROOT");
	}
    if((tr->ruleNode)->type==0){
    	//printf("%7s","YES");
    	printf("%7s","YES");
	}
    else {
    	//printf("%7s","NO");
    	printf("%7s","NO");
	}
	
    if(tr->ruleNode->type==1){
		//printf("%25s",(tr->ruleNode)->name);
		printf("%25s",(tr->ruleNode)->name);
	}
	else if(tr->ruleNode->type==3){
    	//printf("%25s","EPSILON");
    	printf("%25s","EPSILON");
	}
    else {
    	//printf("%25s","---------");
    	printf("%25s","---------");
	}
    //printf("\n");
    printf("\n");
}
