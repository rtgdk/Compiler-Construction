#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

ttypedef struct grammarnode{
	int type;  //type =1 for non terminal, type=0 for terminal, type=2 for or1, type=3 for epsilon
	int value;
	char name[40];
	int epsilon;  // epsilon =1 for epsilon, epsilon=0 for others
	struct grammernode *next;
	struct grammernode *prev;
} grammarnode;

typedef grammarnode *GrammarNode;

typedef struct rulenode{
	int lineno;
	GrammarNode head;
	GrammarNode tail;
} Rule;

Rule* grammer[NO_OF_RULES];

GrammarNode makenode(int type,int value,char* name){
	GrammarNode g = (GrammarNode)malloc(sizeof(struct grammarnode));
	g->type = type;
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
			printf("%d-%s ",temp->type,temp->name);
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
