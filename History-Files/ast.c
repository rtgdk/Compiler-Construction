/*
Rohit Lodha
2015A7PS0040P
*/
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <stdbool.h>
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
		hashvalue = hash3(allow[i]);
		if(HashTable2[hashvalue]==NULL){
			HashTable2[hashvalue] = (hashtable2*)malloc(sizeof(hashtable2));
			HashTable2[hashvalue]->next = NULL;
			strcpy(HashTable2[hashvalue]->name,allow[i]);
		}
		else{
			hashtable2* temp = HashTable2[hashvalue];
			while(temp->next!=NULL){
				temp=temp->next;
			}
			temp->next = (hashtable2*)malloc(sizeof(hashtable2));
			temp->next->next = NULL;
			strcpy(temp->next->name,allow[i]);
		}
	}
	return;
}

/*
Find if name is present in HashTable
*/
int present(char* name){
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
AStree tokenizeTree(AStree tree,tokenInfo tk){
	strcpy(tree->tk.name,tk.name);
	tree->tk.type = tk.type;
	tree->tk.lineno = tk.lineno;
	strcpy(tree->tk.lexeme,tk.lexeme);
	//tree = next(tree);
	return tree;
}

/*
create parse tree
*/
AStree createASTree(parsetree pt){
	AStree root = (AStree)malloc(sizeof(astnode));
	strcpy(root->name,pt->ruleNode->name);
	// root->ruleNode = (GrammarNode)malloc(sizeof(grammarnode));
	// strcpy(root->ruleNode->name,"<mainFunction>");
	// root->ruleNode->next = NULL;
	// root->ruleNode->prev = NULL;
	root->noc = 0;
	root->parent = NULL;
	root->child = NULL;
	root->child = (struct treenode**)malloc((root->noc+1)*sizeof(struct treenode*));
	root->child[0] =NULL;
	root->next = NULL;
	if(root->name[0]!='<'){
		tokenizeTree(root,pt->tk);
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
AStree addChildren(AStree tree, parsetree pt){ 
	tree->child = (struct treenode**)realloc(tree->child,(tree->noc+1)*sizeof(struct treenode*));
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
AStree next(AStree tree){
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
	AStree ast =  createASTree(parsetree PT);
	AStree astChild;
	int flag = 0;
	if(PT->child[0]!=NULL){
		for(i =0;i<PT->noc;i++){
			if(PT->child[i]->name[0]=='<'){
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
			else if (present(PT->child[0]->name)){
				flag = 1;
				ast = addChildren(ast,PT->child[0]);
			}
		}
	}
	if(flag==0) return NULL; // to avoid more_ids->epsilon situation
	else return ast;
}

/*
Called only if ast has only 1 child with a non terminal node 
which can be pulled up, childNo is the child no of ast from 
its parent node
*/
void pruneAST(AStree ast,int childNo){
	ASTree tree;
	tree = ast;
	if(tree->noc==0) return tree; // For terminals (Check for exceptions##)
	if (tree->parent!=NULL){
		if(tree->child[0]->noc==1){
			pruneAST(tree->child[0],0);
		}
		tree->parent->child[childNo] = tree->child[0];
		tree->parent->child[childNo]->parent = tree->parent
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
	AStree tree = ast
	tokenizeTree(tree->parent, tree->tk);
	strcpy(tree->parent->name,tree->name);
	for(i=childNo;i < tree->parent->noc - 1;i++){
		tree->parent->child[i] = tree->parent->child[i+1];
	}
	ast = tree->parent;
	free(tree);
}
/* To modify the AST
*/
AStree modifyAST(AStree ast){
	if (ast==NULL) return;
	int i=1;
	if(ast->child[0]!=NULL){
		if(ast->child[0]->noc==1){
			pruneAST(ast->child[0],0)
		}
		AStree tree = ast->child[0]->next;
		if(isOperator(ast->child[0])){
			operatorAST(ast->child[0]),0)
		}
		else{
			modifyAST(ast->child[0]);
		}
		while(tree!=NULL){
			tree = tree->next;
			if(ast->child[i]->noc==1){
				pruneAST(ast->child[i],i)
			}
			if(isOperator(ast->child[i])){
				operatorAST(ast->child[i]),i)
			}
			else{
				modifyAST(ast->child[i]);
			}
			i++;
		}
	}
	return ast;
	
}

