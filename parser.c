#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#define hashtablesize 100 
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
int hashtable[hashtablesize];
int hashtableterminal[hashtablesize];
int hash(char* str)//hash from string to bucket number
{
	unsigned long hash = 5381;
	int c;
	while (c = *str++)
	    hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
	return (int)(hash % hashtablesize);
}

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
	//r->firsthead = r->firsttail = r->followhead = r->followtail=NULL;
	r->firstcalculated =0;
	r->followcalculated =0;
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
		hashvalue = hash(buffer);
		hashtable[hashvalue] = i;
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
				GrammarNode newnode = makenode(2,-2,buffer);
				newnode->prev = temp;
				temp->next = newnode;
				temp = temp->next;
			}
			else if(buffer[0]!='<'){
				GrammarNode newnode = makenode(0,-1,buffer);
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
				hashvalue = hash(buffer);
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

char* terminals = ["MAIN",
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
	"EPSILON"];

void populateHashtableTerminal(){
	int i=0;
	for (i=0;i<NO_OF_TERMINALS;i++){
		hashtableterminal[hash(terminals[i])] = i;
	}
}

void populateHashtableNonTerminal(){
	int i=0;
	for (i=0;i<NO_OF_NONTERMINALS;i++){
		hashtable[hash(grammer[i]->head->name)] = i;
	}
}
void findFirst(int value){ //,int parent,int* eps)
	
	// if already found in hash
	// can keep local int* and send it to add as apar
	int nt = hashtable[value];
	//int parentnt = hashtable[parent];
	if(nt==-1){
		*eps=0;
		firstMatrix[nt][hashtableterminal[value]] = 1;
		return;
	}
	else if(grammer[nt]->firstcalculated==1){
		return;
	}
		// for (i=0;i<NO_OF_TERMINALS;i++){
			// if(firstMatrix[nt][i]==1){
				// firstMatrix[parentnt][i]=1;
			// }
		// }
		// *eps=0;
		// if(firstMatrix[nt][hashtableterminal[hash("EPSILON")]]==1){
			// *eps=1;
			// findfollow(nt,nt); //check
			// for (i=0;i<NO_OF_TERMINALS;i++){
				// if(followMatrix[nt][i]==1){
					// firstMatrix[parentnt][i]=1;
				// }
			// }
		// }
		// return;
	// }
	else{
		int i,j;
		GrammarNode temp = grammer[nt]->head;
		// if (temp->next->type==3){
			// *eps = 1;
			// firstMatrix[nt][hashtableterminal[hash("EPSILON")]]=1;
			// grammer[nt]->firstcalculated=1;
			//firstMatrix[parentnt][hashtableterminal[hash("EPSILON")]]=1;
			//should we calculate follow?
			// return;
		// }
		//else 
		temp=temp->next;
		while(temp!=NULL){
			if(temp->next->type==2)){
				temp=temp->next;
			}
			else if(temp->next->type==3){
				firstMatrix[nt][hashtableterminal[hash("EPSILON")]] = 1;
			}
			else{
				//int eps1=0;
				int newnt = hash(temp->name);
				findFirst(newnt);
				for (i=0;i<NO_OF_TERMINALS;i++){
					if(firstMatrix[newnt][i]==1){
						firstMatrix[nt][i]=1;
					}
				}
				if (firstMatrix[newnt][hashtableterminal[hash("EPSILON")]]!=1) {
					temp=temp->next;
					while(!(temp->type==2) && temp!=NULL){
						temp=temp->next;
					}
					if (temp==NULL) break;
				}
				temp=temp->next;
		}
		grammer[nt]->firstcalculated=1;
		return;
		//}
	}
}

int count=0;
void findfollow(int value){
	// if hashed
	// if main add $ (eps)
	int nt = hashtable[value];
	//int parentnt = hashtable[parent];
	// if(nt==-1){
		// *eps=0;
		// firstMatrix[hashtable[parent]][hashtableterminal[value]] = 1;
		// return;
	// }
	if(grammer[nt]->followcalculated==1){
		// for (i=0;i<NO_OF_TERMINALS;i++){
			// if(followMatrix[nt][i]==1){
				// followMatrix[parentnt][i]=1;
			// }
		// }
		return;
		//nothing for epsilon
	}
	else{
		for (i=0;i<NO_OF_RULES;i++){
			GrammarNode temp = grammer[i]->head->next;
			while(temp!=NULL){
				if(hash(temp->value)==value){
					if(temp->next->value==NULL) {// don't check for A->A recursion
						int = newntvalue = hash(grammer[i]->head)
						findfollow(newntvalue);
						for (i=0;i<NO_OF_TERMINALS;i++){
							if(followMatrix[newntvalue][i]==1){
								followMatrix[nt][i]=1;
							}
						}
					}
					else {
						int newntvalue = hash(temp->next);
						findfirst(newntvalue);
						for (i=0;i<NO_OF_TERMINALS;i++){
							if(firstMatrix[newntvalue][i]==1){
								followMatrix[nt][i]=1;
							}
						}
						
					}
				}
			}
		}
	}
}

void calculateFirstSet(){
	
}

void printFirstSet(){
	
}

void calculateFollowSet(){
	
}

void printFollowSet(){
	
}

