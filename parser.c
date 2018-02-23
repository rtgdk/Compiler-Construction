#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

// typedef struct grammarnode{
	// int type;  //type =1 for non terminal, type=0 for terminal, type=2 for or1, type=3 for epsilon
	// int value;
	// char name[40];
	// struct grammernode *next;
	// struct grammernode *prev;
// } grammarnode;

// typedef grammarnode *GrammarNode;

// typedef struct rulenode{
	// int lineno;
	// GrammarNode head;
	// GrammarNode tail;
// } Rule;

// Rule* grammer[NO_OF_RULES];

int NO_OF_TERMINALS=0;
int NO_OF_NONTERMINALS=1;

GrammarNode makenode(int type,int value,char* name){
	GrammarNode g = (GrammarNode)malloc(sizeof(struct grammarnode));
	g->type = type;
	if(type==0) NO_OF_TERMINALS++;
	else if(type==1) NO_OF_NONTERMINALS++;
	g->value = value;
	strcpy(g->name,name);
	// if(type==1){
		
	// }
	// else{
		// strcpy(g->name,name);
	// }
	g->next = NULL;
	g->prev = NULL;
}


Rule* makerule(int lineno){
	Rule* r = (Rule* )malloc(sizeof(Rule));
	r->lineno = lineno;
	r->head = NULL;
	r->tail = NULL;
}

char buffer[200];

void createGrammar(char *grammerfile){
	FILE* fp = fopen(grammerfile,"r");
	if (fp==NULL){
		printf("Error opening file");
	}
	int i;
	int hashvalue = 0;
	//A->Ba
	for (i=0;i<NO_OF_RULES;i++){
		fscanf(fp,"%s",buffer); //N.T. A
		//printf("%s",buffer);
		Rule* rule = makerule(i+1);
		GrammarNode newnode = makenode(1,hashvalue,buffer);
		rule->head = newnode;
		rule->tail = newnode;
		GrammarNode temp = rule->head;
		fscanf(fp, "%s",buffer); //->
		//printf("%s",buffer);
		fscanf(fp, "%s",buffer); //B
		//printf("%s",buffer);
		while(buffer[0]!='.'){
			if(buffer[0]=='|'){
				GrammarNode newnode = makenode(2,hashvalue,buffer);
				newnode->prev = temp;
				temp->next = newnode;
				temp = temp->next;
			}
			else if(buffer[0]!='<'){
				GrammarNode newnode = makenode(0,hashvalue,buffer);
				newnode->prev = temp;
				temp->next = newnode;
				temp = temp->next;
			}
			else if(strcmp(buffer,"EPSILON")==0){
				GrammarNode newnode = makenode(3,hashvalue,buffer);
				newnode->prev = temp;
				temp->next = newnode;
				temp = temp->next;
			}
			else if(buffer[0]=='<'){
				GrammarNode newnode = makenode(1,hashvalue,buffer);
				newnode->prev = temp;
				temp->next = newnode;
				temp = temp->next;
			}
			else{
				printf("Error at line %d while reading grammer" ,i+1);
				//exit
			}
			fscanf(fp, "%s",buffer); //B
			//printf("%s",buffer);
		}
		rule->tail = temp;
		grammer[i] = rule;
	}
	fclose(fp);
	return;
}

void printGrammar(){
	int i;
	for (i=0;i<NO_OF_RULES;i++){
		Rule* rule = grammer[i];
		GrammarNode temp = rule->head;
		while(temp!=NULL){
			printf("%s ",temp->name);
			temp=temp->next;
		}
		printf("\n");
	}
}
// int main(){
	// printf("Started");
	// createGrammar("grammer.txt");
	// printf("Done");
	// printGrammar();
// }

int** firstMatrix = (int **)malloc(sizeof(int*)*NO_OF_NONTERMINALS);
int** followMatrix = (int **)malloc(sizeof(int*)*NO_OF_NONTERMINALS);

void initialisefirstandfollowMatrix(){
	int i,j;
	for(i=0;i<NO_OF_TERMINALS;i++){
		firstMatrix[i] = (int*)malloc(sizeof(int)*NO_OF_TERMINALS);
		followMatrix[i] = (int*)malloc(sizeof(int)*NO_OF_TERMINALS);
		for(j=0;j<NO_OF_TERMINALS;j++){
			firstMatrix[i][j]=0;
			followMatrix[i][j]=0;
		}
	}
}

void findFirst(int value,int parent,int* eps){
	// for (i=0;i<NO_OF_TERMINALS;i++){
		// if
	// }
	// if already found in hash
	// can keep local int* and send it to add as apar
	int i,j;
	for (i=0;i<NO_OF_RULES;i++){
		if (strcmp(grammer[i]->head->value,value){
			// if hashed present
			if (grammer[i]->head->next->type==3){ //epsilon
				*eps = 1;
				findFirst[value][NO_OF_TERMINALS-1]=1;
				findFirst[parent][NO_OF_TERMINALS-1]=1;
				// check for follow
				// add(parent,epsilon);
			}
			else{
				GrammarNode temp = grammer[i]->head->next;
				while (temp!=NULL){
					eps = 0; //epsilon
					int eps1;
					findFirst(hash(temp->name),parent,&eps1)
					if(eps1!=1) break;
					else temp=temp->next;
				}
			}
		}
	}
}

int count=0;
void findfollow(int value,int parent,int* eps){
	// if hashed
	// if main add $ (eps)
	for (i=0;i<NO_OF_RULES;i++){
		GrammarNode temp = grammer[i]->head->next;
		while(temp!=NULL){
			if(temp->value==value){
				if(temp->next->value==NULL) {// don't check for A->A recursion
					int eps;
					findfollow(hash(grammer[i]->head),parent,&eps);
				}
				else {
					findfirst(hash(temp->next),parent,&eps);
				}
			}
		}
	}
}


