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
//char buffer[BUFFER_SIZE];
/*
For hashing Non Terminals to bucket no in HashTable
*/
int hash2(char* str)
{
	unsigned long hash = 5381;
	int c;
	while (c = *str++)
	    hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
	return (int)(hash % hashtablesize);
}

/*
For printing HashTable
*/
void printHashTable(){
	int i;
	for (i=0;i<hashtablesize;i++){
		hashtable* temp = HashTable[i];
		printf("%d-->",i);
		while(temp!=NULL){
			printf("%s,",temp->name);
			temp=temp->next;
		}
		printf("\n");
	}
}

/*
Make a grammar node with specified parameters
*/
GrammarNode makenode(int type,int value,char* name){
	GrammarNode g = (GrammarNode)malloc(sizeof(struct grammarnode));
	g->type = type;
	//if(type==0) NO_OF_TERMINALS++;
	//else if(type==1) NO_OF_NONTERMINALS++;
	//g->value = value;
	strcpy(g->name,name);
	g->next = NULL;
	g->prev = NULL;
}

/*
Make a grammar rule with specified parameters
*/
Rule* makerule(int lineno){
	Rule* r = (Rule* )malloc(sizeof(Rule));
	r->lineno = lineno;
	r->head = NULL;
	r->tail = NULL;
	r->firstcalculated =0;
	r->followcalculated =0;
}

/*
List of terminals
*/
char* terminals[NO_OF_TERMINALS] = {"MAIN",
	"SQO",
	"SQC",
	"END",
	"SEMICOLON",
	"FUNCTION",
	"ASSIGNOP",
	"FUNID",
	"ID",
	"INT",
	"REAL",
	"STRING",
	"MATRIX",
	"COMMA",
	"SIZE",
	"IF",
	"OP",
	"CL",
	"ELSE",
	"ENDIF",
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
	"NE",
	"EOF",
	"EPSILON"};

/*
Initialise HashTable
*/
void initHashtable(){
	int i=0;
	for (i=0;i<hashtablesize;i++){
		HashTable[i] = NULL;
	}
}

/*
Insert a Node in hashtable using non terminal index (ruleNode)
*/
void insert(int hashvalue,int type,char* name, int ruleNo){
	if(HashTable[hashvalue]==NULL){
		HashTable[hashvalue] = (hashtable*)malloc(sizeof(hashtable));
		HashTable[hashvalue]->type = type;
		HashTable[hashvalue]->ruleNo = ruleNo;
		HashTable[hashvalue]->next = NULL;
		strcpy(HashTable[hashvalue]->name,name);
	}
	else{
		hashtable* temp = HashTable[hashvalue];
		while(temp->next!=NULL && strcmp(temp->name,name)!=0){
			temp=temp->next;
		}
		if(strcmp(temp->name,name)==0){
			return;
		}
		temp->next = (hashtable*)malloc(sizeof(hashtable));
		temp->next->type = type;
		temp->next->next = NULL;
		temp->next->ruleNo = ruleNo;
		strcpy(temp->next->name,name);
	}
	return;
}

/*
Create Grammar from grammarfile
*/
void createGrammar(char *grammarfile){
	FILE* fp = fopen(grammarfile,"r");
	if (fp==NULL){
		printf("Error opening grammar file");
	}
	int i;
	int hashvalue = 0;
	//A->Ba
	for (i=0;i<NO_OF_RULES;i++){
		fscanf(fp,"%s",buffer); //N.T. A
		//printf("%s",buffer);
		Rule* rule = makerule(i+1);
		hashvalue = hash2(buffer);
		GrammarNode newnode = makenode(1,hashvalue,buffer);
		insert(hashvalue,1,buffer,i);
		rule->head = newnode;
		rule->tail = newnode;
		GrammarNode temp = rule->head;
		fscanf(fp, "%s",buffer); //->
		//printf("%s",buffer);
		fscanf(fp, "%s",buffer); //B
		//printf("%s",buffer);
		while(buffer[0]!='.'){
			if(buffer[0]=='|'){
				GrammarNode newnode = makenode(2,-2,buffer);
				newnode->prev = temp;
				temp->next = newnode;
				temp = temp->next;
			}
			else if(buffer[0]!='<'){
				GrammarNode newnode = makenode(0,-1,buffer);
				hashvalue = hash2(buffer);
				//insert(hashvalue,0,buffer,NULL); //NULL coz T
				newnode->prev = temp;
				temp->next = newnode;
				temp = temp->next;
			}
			else if(strcmp(buffer,"EPSILON")==0){
				GrammarNode newnode = makenode(3,-3,buffer);
				newnode->prev = temp;
				temp->next = newnode;
				temp = temp->next;
			}
			else if(buffer[0]=='<'){
				hashvalue = hash2(buffer);
				GrammarNode newnode = makenode(1,hashvalue,buffer);
				//insert(hashvalue,1,buffer,);
				newnode->prev = temp;
				temp->next = newnode;
				temp = temp->next;
			}
			else{
				printf("Error at line %d while reading grammar" ,i+1);
				//exit
			}
			fscanf(fp, "%s",buffer); //B
			//printf("%s",buffer);
		}
		rule->tail = temp;
		grammar[i] = rule;
	}
	fclose(fp);
	return;
}

/*
Print Grammar
*/
void printGrammar(){
	int i;
	for (i=0;i<NO_OF_RULES;i++){
		Rule* rule = grammar[i];
		GrammarNode temp = rule->head;
		while(temp!=NULL){
			printf("%s ",temp->name);
			temp=temp->next;
		}
		printf("\n");
	}
}

/*
Initialise First and Follow Matrix
*/
void initialisefirstandfollowMatrix(){
	firstMatrix = (int **)malloc(sizeof(int*)*NO_OF_NONTERMINALS);
	followMatrix = (int **)malloc(sizeof(int*)*NO_OF_NONTERMINALS);
	int i,j;
	for(i=0;i<NO_OF_NONTERMINALS;i++){
		firstMatrix[i] = (int*)malloc(sizeof(int)*NO_OF_TERMINALS);
		followMatrix[i] = (int*)malloc(sizeof(int)*NO_OF_TERMINALS);
		for(j=0;j<NO_OF_TERMINALS;j++){
			firstMatrix[i][j]=0;
			followMatrix[i][j]=0;
		}
	}
}

/*
Find if name is present in HashTable
*/
hashtable* present(char* name){
	int hashvalue = hash2(name);
	if(HashTable[hashvalue]==NULL){
		return NULL;
	}
	else{
		hashtable* temp = HashTable[hashvalue];
		while(temp->next!=NULL && strcmp(temp->name,name)!=0){
			temp=temp->next;
		}
		if(strcmp(temp->name,name)==0){
			return temp;
		}
		else{
			return NULL;
		}
		
	}
}

/*
Find First of Name(Both Non terminal of terminal)
*/
void findFirst(char* name){ //,int parent,int* eps) //parent give rule no NT
	
	// if already found in hash
	// can keep local int* and send it to add as apar
	hashtable* hashNode;
	hashNode = present(name);
	//int parentnt = hashtable[parent];
	//printf("%d retuned from present in %s\n",hashNode->ruleNo,hashNode->name);
	if(hashNode==NULL){
		printf("Unrecognized node while creating FollowSet");
	}
	else if(grammar[hashNode->ruleNo]->firstcalculated==1){
		return;
	}
	else{
		//printf("In findFirstElse\n");
		int i,j;
		GrammarNode temp = grammar[hashNode->ruleNo]->head;
		temp=temp->next;
		while(temp!=NULL){
			//printf("In While\n");
			if(temp->type==2){
				temp=temp->next;
			}
			else if(temp->type==3){
				firstMatrix[hashNode->ruleNo][NO_OF_TERMINALS-1] = 1;
			}
			else{
				//printf("In While else\n");
				//int eps1=0;
				hashtable* hashNode2;
				//printf("%s\n",temp->name);
				//int newnt = present(temp->name,hashNode2);
				hashNode2 = present(temp->name);
				//printf("%d retuned from present\n",hashNode->ruleNo);
				if(hashNode2==NULL){
						int i=0;
						int flag=0;
						for(i=0;i<NO_OF_TERMINALS;i++){
							if (strcmp(terminals[i],temp->name)==0){
								flag=1;
								//printf("%d matched\n",i);
								//printf("%d initial value\n",firstMatrix[hashNode->ruleNo][i]);
								firstMatrix[hashNode->ruleNo][i] = 1;
								//printf("%d final value\n",firstMatrix[hashNode->ruleNo][i]);
								break;
							}
						}
						//printf("%d flag",flag);
						if(flag==0) printf("\nErrorr while creating FirstSet\n");
						temp=temp->next;
						while(temp!=NULL && !(temp->type==2)){
							//printf("here");
							temp=temp->next;
						}
						if (temp==NULL) {
							//printf("\nTemp is Null\n");
							break;
						}
					}
				else {
					findFirst(hashNode2->name);
					for (i=0;i<NO_OF_TERMINALS;i++){
						if(firstMatrix[hashNode2->ruleNo][i]==1){
							firstMatrix[hashNode->ruleNo][i]=1;
						}
					}
					if (firstMatrix[hashNode2->ruleNo][NO_OF_TERMINALS-1]!=1) {
						temp=temp->next;
						while(temp!=NULL && !(temp->type==2)){
							temp=temp->next;
						}
						if (temp==NULL) {
							//printf("\nTemp is Null2\n");
							break;
						}
					}
				}
//				else{
//					printf("Error sub rule\n");
//				}
				temp=temp->next;
			}
		}
		grammar[hashNode->ruleNo]->firstcalculated=1;
		return;
		//}
	}
}

/*
Find follow of Name(Non terminal)
*/
void findfollow(char* name){
	// if hashed
	// if main add $ (eps)
	//printf("Follow for %s",name);
	hashtable* hashNode;
	hashNode = present(name);
	//int parentnt = hashtable[parent];
	// if(nt==-1){
		// *eps=0;
		// firstMatrix[hashtable[parent]][hashtableterminal[value]] = 1;
		// return;
	// }
	if(hashNode==NULL){
		printf("Unrecognized node while creating FollowSet");
	}
	else if(grammar[hashNode->ruleNo]->followcalculated==1){
		// for (i=0;i<NO_OF_TERMINALS;i++){
			// if(followMatrix[nt][i]==1){
				// followMatrix[parentnt][i]=1;
			// }
		// }
		return;
		//nothing for epsilon
	}
	else{
		int i;
		for (i=0;i<NO_OF_RULES;i++){
		//	printf("Checking for rule %d",i);
			GrammarNode temp = grammar[i]->head->next;
			while(temp!=NULL){
			//	printf("Checking for %s matching with %s",temp->name,name);
				if(strcmp(temp->name,name)==0){
				//	printf("%s matched with %s in rulle no %d",temp->name,name,i);
					if((temp->next==NULL || temp->next->type==2) && (strcmp(grammar[i]->head->name,temp->name)!=0)) {// don't check for A->A recursion
//						if()==0){
//							break;
//						}
					//	printf("GOing for Follow -%s\n",grammar[i]->head->name);
						findfollow(grammar[i]->head->name);
//						hashtable* hashNode2;
//						hashNode2 = present(grammar[i]->head->name);
						int j=0;
						for (j=0;j<NO_OF_TERMINALS;j++){
							if(followMatrix[i][j]==1){
								followMatrix[hashNode->ruleNo][j]=1;
							}
						}
					}
					else if(temp->next!=NULL && temp->next->type!=2) {
						//printf("%s-->" ,temp->name);
						 hashtable* hashNode2;
						 hashNode2 = present(temp->next->name);
						 if (hashNode2==NULL){
						 	int flag = 0;
						 	int j=0;
						 	//printf("Found term in mext-%s\n",temp->next->name);
						 	for (j=0;j<NO_OF_TERMINALS;j++){
						 		if (strcmp(temp->next->name,terminals[j])==0){
						 		//	printf("Next matched with %s",terminals[j]);
						 			followMatrix[hashNode->ruleNo][j]=1;
						 			flag=1;
						 			break;
								 }
						 	}
						 	if (flag==0) printf("Unrecognized token while creating FollowSet");
						 }
						 else{
						 //	printf("GOing for First -%s\n",temp->next->name);
						 	findFirst(temp->next->name);
						 	int j=0;
							for (j=0;j<NO_OF_TERMINALS;j++){
								if(firstMatrix[hashNode2->ruleNo][j]==1){
									followMatrix[hashNode->ruleNo][j]=1;
								}
							}
							if (firstMatrix[hashNode2->ruleNo][39]==1){  //NO_OF_TERMINALS-1
								findfollow(temp->next->name);
								int j=0;
								for (j=0;j<NO_OF_TERMINALS;j++){
									if(followMatrix[hashNode2->ruleNo][j]==1){
										followMatrix[hashNode->ruleNo][j]=1;
									}
								}
							}
						}
						 
						
					}
				}
				temp=temp->next;
			}
		}
	}
	followMatrix[hashNode->ruleNo][39]=0;
	grammar[hashNode->ruleNo]->followcalculated=1;
}
/*
Find first set of all the non terminals of the grammar
*/
void findFirstSet(){
	int i,j;
	for(i=0;i<NO_OF_RULES;i++){
		//printf("Finding first set for %s\n",grammar[i]->head->name);
		findFirst(grammar[i]->head->name);
		// printf("First Set of %s\n",grammar[i]->head->name);
		// for(j=0;j<NO_OF_TERMINALS;j++){
		// 	if (firstMatrix[i][j]==1){
		// 		printf("%s, ", terminals[j]);
		// 	}
		// }
		// printf("\n");
	}
}

/*
Find follow set of all the non terminals of the grammar
*/
void findFollowSet(){ //Don't print EPSILON
	int i,j;
	for(i=0;i<NO_OF_RULES;i++){
		findfollow(grammar[i]->head->name);
		// printf("Follow Set of %s\n",grammar[i]->head->name);
		// for(j=0;j<NO_OF_TERMINALS;j++){
		// 	if (followMatrix[i][j]==1){
		// 		printf("%s, ", terminals[j]);
		// 	}
		// }
		// printf("\n");
	}
}


/*
Initialise Parse Table
*/
void initParseTable(){
	parsetable = (GrammarNode**)malloc(sizeof(GrammarNode*)*NO_OF_RULES);
	int i,j;
	for (i=0;i<NO_OF_RULES;i++){
		parsetable[i] = (GrammarNode)malloc(sizeof(grammarnode)*(NO_OF_TERMINALS));
		for(j=0;j<NO_OF_TERMINALS;j++){
			parsetable[i][j]=NULL;
		}
	}
}

/*
Find first of all the rules on rhs till the specified condition of epsilon
*/
int* firstofRHS(GrammarNode rhs){
	int i,j;
	int* result = (int*)malloc(sizeof(int)*NO_OF_TERMINALS);
	for(i=0;i<NO_OF_TERMINALS;i++) result[i]=0;
	while(rhs!=NULL){
		if (rhs->type==2) {
			break;
		}
		hashtable* hashNode;
		hashNode = present(rhs->name);
		if(hashNode==NULL){
			for(i=0;i<NO_OF_TERMINALS;i++){
				if(strcmp(rhs->name,terminals[i])==0){
					result[i]=1;
					break;
				}
			}		
			result[NO_OF_TERMINALS-1] = 0;
			break;
		}
		else{
			for(i=0;i<NO_OF_TERMINALS;i++){
				if(firstMatrix[hashNode->ruleNo][i]==1){
					result[i]=1;
				}
			}
			result[NO_OF_TERMINALS-1] = 0;
			if (firstMatrix[hashNode->ruleNo][NO_OF_TERMINALS-1]!=1) break;
		}
		rhs = rhs->next;
	}
	if(rhs==NULL || rhs->type==2)  result[NO_OF_TERMINALS-1]=1;
	return result;
}


/*
Create Parse Table
*/
void createParseTable(){
	int i,j;
	int* p;
	for (i=0;i<NO_OF_RULES;i++){
		GrammarNode temp = grammar[i]->head->next;
		GrammarNode start = grammar[i]->head->next;
		while(temp!=NULL){
			if(strcmp(temp->name,"EPSILON")==0){
				//printf("Epsilon found!!");
				for(j=0 ; j<NO_OF_TERMINALS; j++) { //remove eps from followset
	                if(followMatrix[i][j]==1)
	                	if (parsetable[i][j]==NULL) {
	                		parsetable[i][j] = start;
						}
	                    
	            }
			}
			p = firstofRHS(temp);
			for(j=0;j<NO_OF_TERMINALS-1;j++){ //ignoring epsilon
				if(p[j]==1){  // can be without ==
					if (parsetable[i][j]!=NULL) printf("grammar not LL1") ;
					parsetable[i][j] = start;
				}
			}
			if (p[NO_OF_TERMINALS-1]==1) {// can be without ==
				for(j=0 ; j<NO_OF_TERMINALS-1; j++) { //remove eps from followset
	                if(followMatrix[i][j]==1)
	                	if (parsetable[i][j]!=NULL) printf("grammar not LL1") ;
	                    parsetable[i][j] = start;
	            }
			}
			if(temp==NULL) break;
			else if(temp->type==2) {  // temp->type==2
				temp = temp->next;
				start = temp;
				//start = 
			}
			else{ //agar bech mein se return
				while(temp!=NULL && temp->type!=2){
					temp=temp->next;
				}
				if (temp==NULL){
					break;
				}
				if(temp->type==2) {
					temp = temp->next;
					start = temp;
				}
			}
		}
		parsetable[i][39]=NULL;	
	}
	
}

/*
Print Parse Table
*/
void printParseTable(){
	int i,j;
	printf("\n parse table %d\n",NO_OF_TERMINALS);
	for (i=0;i<NO_OF_RULES;i++){
		printf("\n%s-->",grammar[i]->head->name);
		for(j=0;j<NO_OF_TERMINALS;j++){
			if(parsetable[i][j]!=NULL){
				printf("%s--> %s\n",terminals[j],parsetable[i][j]->name); //linked to %s
			}			
		}
	}
}

/*
create stack
*/
Stack createStack(){
	Stack st = (Stack)malloc(sizeof(Stack));
	st->bottom = (GrammarNode)malloc(sizeof(grammarnode));
	st->top = (GrammarNode)malloc(sizeof(grammarnode));
	strcpy(st->bottom->name,"EOF");
	strcpy(st->top->name,"<mainFunction>");
	st->top->next=NULL;
	st->top->prev=st->bottom;
	st->top->type=1; //coz NT
	//st->size=;
	st->bottom->next=st->top;
	st->bottom->prev=NULL;
	st->bottom->type=0; //coz term
	return st;
}

/*
return top
*/
GrammarNode top(Stack* st){
	return (*st)->top;
}


/*
push node into stack
*/
void push(Stack* st,GrammarNode gn){ // A-> BCD assuming gn is separate from the real grammar node
	GrammarNode temp = gn;
	while(temp->next!=NULL && temp->next->type!=2){
		temp=temp->next;
	}
	while(temp!=gn){
		GrammarNode temp2 = (GrammarNode)malloc(sizeof(grammarnode));
		strcpy(temp2->name,temp->name);
		temp2->type=temp->type;
		temp2->next = NULL;
		temp2->prev = (*st)->top;
		(*st)->top = temp2;
		temp=temp->prev;
	}
	GrammarNode temp3 = (GrammarNode)malloc(sizeof(grammarnode));
	strcpy(temp3->name,temp->name);
	temp3->type=temp->type;
	temp3->next = NULL;
	temp3->prev = (*st)->top;
	(*st)->top = temp3;
}

/*
pop out a node from stack
*/
void pop(Stack* st){
	GrammarNode temp = (*st)->top;
	(*st)->top = (*st)->top->prev;
	(*st)->top->next = NULL;
	free(temp);
}

/*
create parse tree
*/
parsetree createParseTree(){
	parsetree root = (parsetree)malloc(sizeof(treenode));
	root->ruleNode = (GrammarNode)malloc(sizeof(grammarnode));
	strcpy(root->ruleNode->name,"<mainFunction>");
	root->ruleNode->next = NULL;
	root->ruleNode->prev = NULL;
	root->ruleNode->type = 1; //coz NT
	root->noc = 0;
	root->parent = NULL;
	root->child = (struct treenode**)malloc((root->noc+1)*sizeof(struct treenode*));
	root->child[0] =NULL;
	root->next = NULL;
	return root;
}
/*
returns next of the tree
*/
parsetree next(parsetree tree){
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
Update tree token and returns next
*/
parsetree tokenizeTree(parsetree tree,tokenInfo tk){
	strcpy(tree->tk.name,tk.name);
	tree->tk.type = tk.type;
	tree->tk.lineno = tk.lineno;
	strcpy(tree->tk.lexeme,tk.lexeme);
	tree = next(tree);
	return tree;
}

/*
Make a node form a grammar rule
*/
parsetree makeTreeFromgrammarNode(GrammarNode node){
	parsetree new = (parsetree)malloc(sizeof(treenode));
	new->ruleNode = (GrammarNode)malloc(sizeof(grammarnode));
	strcpy(new->ruleNode->name, node->name);
	new->ruleNode->next = NULL; //not needed
	new->ruleNode->prev = NULL; //not needed
	new->ruleNode->type = node->type; //coz NT
	new->next = NULL;
	new->noc = 0;
	new->child = (struct treenode**)malloc((new->noc+1)*sizeof(struct treenode*));
	new->child[0] = NULL;
	return new;
}

/*
Add children from the grammar rule
*/
parsetree addChildren(parsetree tree, GrammarNode ruleNode){ // A->BCD ruleNode begins from B
	if (ruleNode==NULL) return tree;
	GrammarNode temp = ruleNode;
	while(temp!=NULL && temp->type!=2){
		tree->child = (struct treenode**)realloc(tree->child,(tree->noc+1)*sizeof(struct treenode*));
		tree->child[tree->noc] = makeTreeFromgrammarNode(temp);
		tree->child[tree->noc]->parent = tree;
		temp = temp->next;
		tree->noc++;
	}
	int i;
	for (i=0;i<tree->noc-1;i++){
		tree->child[i]->next = tree->child[i+1];
	}
	return tree->child[0];
}

/*
Check whether stack is empty or not
*/
bool isStackEmpty(Stack* st){
	//printf("in stack empty---> %s -- %s",(*st)->top->name, (*st)->bottom->name);
	if ((*st)->top->prev==NULL) return true;
	GrammarNode gn = top(st);
	hashtable* hashNode;
	hashNode = present(gn->name);
	while(hashNode!=NULL && firstMatrix[hashNode->ruleNo][NO_OF_TERMINALS-1]==1){
		pop(st);
		gn = top(st);
		hashNode = present(gn->name);
	}
	if ((*st)->top->prev==NULL) return true;
	else return false;
}

/*
Intialises Sync Matrix
*/
void initialiseSynchMatrix(){
	synchMatrix = (int **)malloc(sizeof(int*)*NO_OF_NONTERMINALS);
	int i,j;
	for(i=0;i<NO_OF_NONTERMINALS;i++){
		synchMatrix[i] = (int*)malloc(sizeof(int)*NO_OF_TERMINALS);
		for(j=0;j<NO_OF_TERMINALS;j++){
			synchMatrix[i][j] = followMatrix[i][j];
			if(j==4){
				synchMatrix[i][j] = 1;
			}
		}
	}
}

/*
Returns expected tokens for Node <name>
*/
void expectedToken(char* name,char* str){
	hashtable* hashNode;
	hashNode = present(name);
	//char str[200];
	strcpy(str,""); 
	int i;
	if(firstMatrix[hashNode->ruleNo][NO_OF_TERMINALS-1]==1){
		for(i=0;i<NO_OF_TERMINALS-1;i++){
			if(followMatrix[hashNode->ruleNo][i]==1 || firstMatrix[hashNode->ruleNo][i]==1 ){
				strcat(str,terminals[i]); 
				strcat(str,",");
			}
		}
	}
	else{
		for(i=0;i<NO_OF_TERMINALS-1;i++){
			if(firstMatrix[hashNode->ruleNo][i]==1){
				strcat(str,terminals[i]); 
				strcat(str,",");
			}
		}
	}
	//printf("%s--", str);
	//return str;
}


/*
Parse Input from the source file
*/
parsetree parseInputSourceCode(char *testcaseFile, GrammarNode** parsetable){
	FILE* fp = fopen(testcaseFile,"r+");
	if (fp==NULL) {
		printf("Error opening file");
		return NULL;
	}
	Stack st = createStack();
	parsetree ParseTree = createParseTree();
	parsetree ParseTree2 = ParseTree;
	pop(&st);
	push(&st,grammar[0]->head->next); //main rule
	ParseTree = addChildren(ParseTree,grammar[0]->head->next);
	initialiseSynchMatrix();
	tokenInfo tk = getNextToken(fp,0);
	tokenInfo tk2;
	GrammarNode gn;
	hashtable* hashNode;
	int error = 0;
	int i;
	int prevlineno;
	//printf("Going in parseInputWhile");
	while(tk.type!=EOF1){
		//printf("%s--%s found\n",tk.name,tk.lexeme);
		if (tk.type==ERROR){
			tk = getNextToken(fp,0);
			continue;
		}
		//printf("%s type\n",ParseTree->ruleNode->name);
		gn = top(&st);
		//printf("on top %s\n",gn->name);
		if(gn->type==1){ // 
			//printf("NT on top %s\n",gn->name);
			hashNode = present(gn->name);
			for(i=0;i<NO_OF_TERMINALS;i++){
				if(strcmp(tk.name,terminals[i])==0) break;
			}
			GrammarNode newnode= parsetable[hashNode->ruleNo][i];
			if(newnode==NULL){
				char expectedTk[100];
				expectedToken(gn->name,expectedTk);
				//printf("%s expected Token\n",gn->name);
				printf("\nLine No.%d: Syntax Error: The token '%s' for lexeme '%s' does not match at line %d. The expected token here is one of <%s>.\n",tk.lineno,tk.name,tk.lexeme,tk.lineno,expectedTk);
				error++;
				//error recovery
				for(i=0;i<NO_OF_TERMINALS;i++){
					if(strcmp(tk.name,terminals[i])==0) break;
				}
				if (synchMatrix[hashNode->ruleNo][i]==1){
					printf("Error recovery: Found %s in synchronized set.\n",tk.name);
					pop(&st);
					ParseTree = next(ParseTree);
				}
				else{
					printf("Error recovery: Skiping" );
					char* name;
					while(tk.type!=EOF1 && synchMatrix[hashNode->ruleNo][i]==0 ){ //&& isKeyword(tk,name)==-1
						printf(",%s",tk.name);
						tk = getNextToken(fp,0);
						for(i=0;i<NO_OF_TERMINALS;i++){
							if(strcmp(tk.name,terminals[i])==0) break;
						}
					}
					printf(". ");
					if(tk.type==EOF1 ) { //|| isKeyword(tk,name)!=-1
						printf(" Missing semicolon at line %d\n",prevlineno);
						break; //if(tk.type==EOF1) 
					}
					pop(&st);
					ParseTree = next(ParseTree);
					printf("Found match for %s in synchronized set.",tk.name);
				}
//					tk = getNextToken(fp,0);
//					int flag=1;
//					while(tk.type!=EOF1 && flag==1){
//						for(i=0;i<NO_OF_TERMINALS;i++){
//							if(strcmp(tk.name,terminals[i])==0) break;
//						}
//						if(synchMatrix[hashNode->ruleNo][i]==1){
//							printf("Found Synch for %s",tk.name);
//							flag=0;
//							pop(&st);
//							while()
//							//ParseTree = retractTree(ParseTree,tk);
//							break;
//						}
//						else{
//							printf("Skiping %s",tk.name);
//							tk = getNextToken(fp,0);
//						}
//					}
				//if(tk.type==EOF1) break;
			}
			else{
				pop(&st);
				push(&st,newnode);
				ParseTree = addChildren(ParseTree,newnode);
			}
				
			//}
			
		}
		else{ // check whether to chekc for EPSILON and |
			//printf("T on top %s\n",gn->name);
			if(strcmp(gn->name,"EPSILON")==0){
				//printf("here");
				strcpy(tk2.name,"EPSILON");
				strcpy(tk2.lexeme,"EMPTY");
				tk2.lineno = -1;
				tk2.type = 3;
				pop(&st);
				ParseTree = tokenizeTree(ParseTree,tk2);
			}
			else if(strcmp(gn->name,tk.name)==0){
				pop(&st);
				ParseTree = tokenizeTree(ParseTree,tk);
				prevlineno = tk.lineno;
				tk = getNextToken(fp,0);
			}
			else{
				printf("\nLine No.%d: Syntax Error: The token '%s' for lexeme '%s' does not match at line %d. The expected token here is '%s'.\n",tk.lineno,tk.name,tk.lexeme,tk.lineno,gn->name);
				error++;
				printf("Error recovery: Token '%s' inserted.\n",gn->name);
				pop(&st);
				ParseTree = next(ParseTree);
				prevlineno = tk.lineno;
				tk = getNextToken(fp,0);
			}
		}
	}
	if (!isStackEmpty(&st)){
		printf("%s\nStack not empty. Input source code is not syntactically correct. Total %d errors found.\n",RED,error);
	}
	else if(error==0){ //tk.type==EOF && 
		printf("\n%sInput source code is syntactically correct\n",GRN);
	}
	else{
		printf("\n%sInput source code is not syntactically correct. Total %d errors found.\n",RED,error);
	}
	printf(RESET);
	return ParseTree2;
}


/*
Print parse tree inorder traversal
*/
void printParseTree(parsetree PT, FILE* fp)
{
	if (PT==NULL) return;
	if(PT->child[0]!=NULL){
		printParseTree(PT->child[0],fp);
		parsetree temp = PT->child[0]->next;
		printToFile(PT,fp);
		while(temp!=NULL){
			printParseTree(temp,fp);
			temp= temp->next;
		}
	}
	else{
		printToFile(PT,fp);
	}
	
	return;
}


/*
print a tree at a time in file 
*/
void printToFile(parsetree tr,FILE *fp)
{
	//printf("Tree %s\n",tr->tk.name);
    if((tr->ruleNode)->type==0)
        {
            //printf("%15s",tr->tk.lexeme);
            //printf("%7d",tr->tk.lineno);
            //printf("%15s",tr->tk.name);
            fprintf(fp,"%15s",tr->tk.lexeme);
            fprintf(fp,"%7d",tr->tk.lineno);
            fprintf(fp,"%15s",tr->tk.name);
        }
	else
        {
            //printf("      ---------");
            //printf("%7d",0);
            //printf("%s"," --------------");
            fprintf(fp,"      ---------");
            fprintf(fp,"%7d",0);
            fprintf(fp,"%s"," --------------");
        }
    if((tr->ruleNode)->type==0)
    {
        if(strcmp(tr->tk.name,"NUM")==0){
        	//printf("%9d",atoi(tr->tk.lexeme));
        	fprintf(fp,"%9d",atoi(tr->tk.lexeme));
		}
        else if(strcmp(tr->tk.name,"RNUM")==0){
        	//printf("%9f",atof(tr->tk.lexeme));
        	fprintf(fp,"%9f",atof(tr->tk.lexeme));
		}
        else {
        	//printf("%9s","----");
        	fprintf(fp,"%9s","----");
		}
    }
	else {
		//printf("%9s","----");
		fprintf(fp,"%9s","----");
		
	}
    if(tr->parent!=NULL){
    	//printf("%25s",tr->parent->ruleNode->name);
    	fprintf(fp,"%25s",tr->parent->ruleNode->name);
	}
    else {
    	//printf("%25s","ROOT");
    	fprintf(fp,"%25s","ROOT");
	}
    if((tr->ruleNode)->type==0){
    	//printf("%7s","YES");
    	fprintf(fp,"%7s","YES");
	}
    else {
    	//printf("%7s","NO");
    	fprintf(fp,"%7s","NO");
	}
	
    if(tr->ruleNode->type==1){
		//printf("%25s",(tr->ruleNode)->name);
		fprintf(fp,"%25s",(tr->ruleNode)->name);
	}
	else if(tr->ruleNode->type==3){
    	//printf("%25s","EPSILON");
    	fprintf(fp,"%25s","EPSILON");
	}
    else {
    	//printf("%25s","---------");
    	fprintf(fp,"%25s","---------");
	}
    //printf("\n");
    fprintf(fp,"\n");
}


// int main(){
// 	 printf("Started");
// 	 createGrammar("grammar.txt");
// 	 printf("Done");
// 	 //printGrammar();
// 	 //printHashTable();
// 	 initialisefirstandfollowMatrix();
// 	 //printFirstSet();
// 	 printf("\nIntialised\n");
// 	 findFirstSet();
// 	 printf("\nIntialised2\n");
// 	 findFollowSet();
// 	 printf("Firstand FollowDOne");
// 	 //printFirstSet();
// 	 initParseTable();
// 	 createParseTable();
// 	 //printParseTable();
	  
// 	 parsetree PT = parseInputSourceCode("testcase5.txt",parsetable);
// 	 FILE *fp;
// 	 fp = fopen ("output1.txt","w");
// 	 printf("\n      lexCurNode lineno          token valIfNum          parentNodeSymbol isLeaf               NodeSymbol\n");
// 	 fprintf(fp,"     lexCurNode lineno          token valIfNum          parentNodeSymbol isLeaf               NodeSymbol\n");
// 	 printParseTree(PT,fp);
// 	 fclose(fp);	 
// }
