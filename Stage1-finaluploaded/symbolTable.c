/*
Rohit Lodha
2015A7PS0040P
*/
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <stdbool.h>

#include "symbolTable.h"
#include "ast.h"
#include "parser.h"
#include "lexer.h"

#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define RESET "\x1B[0m"

//TODO : Error and Matrix Implementation
int soffset;

char* types[] = {"INTEGER","REAL","STRING","MATRIX"};
/*
For hashing Allowed Terminals to bucket no in HashTable
*/
int hash4(char* str)
{
	unsigned long hash = 5381;
	int c;
	while (c = *str++)
	    hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
	return (int)(hash % VariableTableSize);
}

void initSymbolTable(){
	globalTable = (SymbolTableNode*)malloc(sizeof(SymbolTableNode));
	globalTable->type = 5; //main;
	globalTable->noc = 0;
	globalTable->parent = NULL;
	globalTable->child = NULL;
	// function f;
	// strcmp(f.name,"ROOT");
	strcpy(globalTable->f.name,"ROOT");
	//globalTable->count = 0;
	int i;
	for(i = 0 ; i < VariableTableSize ; i++)
		globalTable->VariableTable[i] = NULL;
	currentTable = globalTable;
	currentScope = 0;
}

int get_int(char *arr) /*str to int*/
{
  int num = 0;
  for(int i=0 ; arr[i]!='\0' ; i++)
  {
    num = num*10 + ((int)arr[i]-'0');
  }
  return num;
}

void addVariable(variable v,SymbolTablePtr st){
	int pos = hash4(v.name);

	if(st->VariableTable[pos] == NULL)
	{
		variablenodeptr vnode = (variablenodeptr)malloc(sizeof(variablenode));
		vnode->v = v;
		vnode->next = NULL;
		st->VariableTable[pos] = vnode;
	}
	else
	{
		variablenodeptr vnode = (variablenodeptr)malloc(sizeof(variablenode));
		vnode->v = v;
		vnode->next = NULL;
		variablenodeptr temp = st->VariableTable[pos];

		while(temp->next != NULL)
		{
			if(strcmp(temp->v.name , v.name) == 0 && temp->v.offset == v.offset)
			{
				//err = true;
				printf( "Line No.%d: Variable '%s' already declared at line %d. Variable redeclaration is not allowed.\n",v.linedec, v.name, temp->v.linedec, v.linedec );
				return;
			}
			temp = temp->next;
		}
		if((strcmp(temp->v.name , v.name) == 0) && (temp->v.offset == v.offset))
		{
				//err = true;
				printf( "Line No.%d: Variable '%s' already declared at line %d. Variable redeclaration is not allowed.\n",v.linedec, v.name, temp->v.linedec, v.linedec );
				return;
		}
		temp->next = vnode;
	}
}

void addFunction(function f, int start, int end,int type) /* add f to MAIN, s start e end, li is line of dec. */
{
	//printf("reached here nochild->%d\n",currentTable->noc);
	
	if(currentTable->noc > 0)
	{
		int i;

		for(i = 0 ; i < currentTable->noc ; i++)
		{
			if(currentTable->child[i]->type==2 && strcmp(currentTable->child[i]->f.name , f.name) == 0)
			{
				// if(!currentTable->child[i]->f.defined)
				// {
				// 	currentTable->child[i]->f = f;
				// 	currentTable = currentTable->child[i];
				// 	variablenodeptr inp = f.inputList;
				// 	while(inp != NULL)
				// 	{
				// 		addVariable(inp->v,currentTable);
				// 		inp = inp->next;
				// 	}
				// 	variablenodeptr op = f.outputList;
				// 	while(op != NULL)
				// 	{
				// 		addVariable(op->v,currentTable);
				// 		op = op->next;
				// 	}
				// 	currentTable->child[i]->start = start;
				// 	currentTable->child[i]->end = end;
				// 	return;
				// }	
				// else
				// {
					//err = true;
				printf( "Line No.%d: Function %s already declared in line %d.Function Overloading(Redefination of function) is not allowed within the same scope.\n" ,f.linedec, f.name,currentTable->child[i]->f.linedec );
				//}	
				
			}
		}

	}
	// Added new by me
	if(type==2){
		SymbolTablePtr temp = currentTable;
		while(temp->parent!=NULL){
			if(temp->type==2 && strcmp(temp->f.name , f.name) == 0)
			{
				printf( "Line No.%d: Function %s already declared in line %d.Function Overloading(Redefination of function) is not allowed within the same scope.\n" ,f.linedec, f.name,temp->f.linedec );
			}
			temp = temp->parent;
		}
	}

	SymbolTablePtr t = (SymbolTablePtr)malloc(sizeof(SymbolTableNode));
	t->type = type;
	int i;
	t->f = f;
	t->noc = 0;
	for(i = 0 ; i < 100 ; i++)
		t->VariableTable[i] = NULL;
	t->parent = currentTable;
	t->child = NULL;
	//t->counter = 0;

	variablenodeptr inp = f.inputList;
	while(inp != NULL)
	{
		addVariable(inp->v,t);
		inp = inp->next;
	}
	variablenodeptr op = f.outputList;
	while(op != NULL)
	{
		addVariable(op->v,t);
		op = op->next;
	}
	t->start = start;
	t->end = end;
	//printf("reached here\n");
	currentTable->child = (SymbolTablePtr*)realloc(currentTable->child,(currentTable->noc+1)*sizeof(SymbolTablePtr));
	currentTable->child[currentTable->noc] = t;
	currentTable->noc++;
	currentTable = currentTable->child[currentTable->noc-1];
	
}

int getType(parsetree pt) // returns int with int,real,str and matrix, pt has noc =1
{
	tp type = pt->child[0]->tk.type;
	if(type == INT) return 1;
	if(type == REAL) return 2;
	if(type == STRING) return 3;
	if(type == MATRIX) return 4;
}


int getWidth(int type){
	if (type==1) return 2;
	else return 2;
}

void addVariableID(parsetree node, int type, SymbolTablePtr b)
{
	if(node->ruleNode->type==0 && node->tk.type == ID)
	{
		variable v;
		strcpy(v.name,node->tk.lexeme);
		v.linedec = node->tk.lineno;
		v.assigned = false;
		v.type = type;
		v.scopeDepth = currentScope;
		v.offset = 0;
		v.width = getWidth(type);
		//for matrix something else
		addVariable(v, b);
		return;
	}
	int i;
	for(i = 0; i < node->noc ; i++)
		addVariableID(node->child[i],type,b);
}

void addFunctionID(parsetree node)
{
	function f;
	int i;
	strcpy(f.name,node->tk.lexeme);
	f.noOfInput = 0;
	f.noOfOutput = 0;
	f.defined = false;
	f.inputList = NULL;
	f.outputList = NULL;
	f.linedec = node->tk.lineno;
	SymbolTablePtr t = (SymbolTablePtr)malloc(sizeof(SymbolTableNode));
	t->type = 2;
	t->f = f;
	for(i = 0 ; i < 100 ; i++)
		t->VariableTable[i] = NULL;
	t->noc = 0;
	t->parent = currentTable;
	t->child = NULL;

	if(currentTable->noc > 0)
	{
		SymbolTablePtr temp = currentTable->child[0];
		int i;

		for(i = 0 ; i < currentTable->noc ; i++)
		{
			if(strcmp(currentTable->child[i]->f.name , f.name) == 0)
			{
				//err = true;
				printf( "ERROR : Redeclaration of function %s\n" , f.name );
				return;
			}
		}
	}
	currentTable->child = (SymbolTablePtr*)realloc(currentTable->child,(currentTable->noc+1)*sizeof(SymbolTablePtr));
	currentTable->child[currentTable->noc] = t;
	//t->counter = 0;
	currentTable->noc++;

}

void addInputList(parsetree node, function *f)
{
	if(node->ruleNode->type==0 && (node->tk.type == ID))
	{
		int vtype;
		if(strcmp(node->parent->ruleNode->name,"<parameter_list>")==0)
			vtype = getType(node->parent->child[0]);
		else
			vtype = getType(node->parent->child[1]);

		variablenodeptr vptr = (variablenodeptr)malloc(sizeof(variablenode));
		// printf("%s\n" , node->tk.lexeme);
		strcpy(vptr->v.name,node->tk.lexeme);
		vptr->v.linedec = node->tk.lineno;
		vptr->v.assigned = false;
		vptr->v.type = vtype;
		vptr->v.scopeDepth = currentScope; 
		vptr->next = NULL;
		vptr->v.offset = 0;
		f->noOfInput++;
		if(f->inputList == NULL)
			f->inputList = vptr;
		else
		{
			variablenodeptr temp = f->inputList;
			while(temp->next != NULL)
			{
				temp = temp->next;
			}
			temp->next = vptr;
		}
	}
	int i;
	for(i = 0; i < node->noc ; i++)
		addInputList(node->child[i],f);
}

void addOutputList(parsetree node,function *f)
{
	int i;
	if(node->ruleNode->type==0 && (node->tk.type == ID))
	{
		int vtype;
		if(strcmp(node->parent->ruleNode->name,"<parameter_list>")==0)
			vtype = getType(node->parent->child[0]);
		else
			vtype = getType(node->parent->child[1]);

		variablenodeptr vptr = (variablenodeptr)malloc(sizeof(variablenode));
		strcpy(vptr->v.name,node->tk.lexeme);
		vptr->v.linedec = node->tk.lineno;
		vptr->v.assigned = false;
		vptr->v.type = vtype; 
		vptr->next = NULL;
		vptr->v.offset = 0;
		vptr->v.scopeDepth = currentScope; 
		f->noOfOutput++;
		if(f->outputList == NULL)
			f->outputList = vptr;
		else
		{
			variablenodeptr temp = f->outputList;
			while(temp->next != NULL)
			{
				temp = temp->next;
			}
			temp->next = vptr;
		}		
	}

	for(i = 0; i < node->noc ; i++)
		addOutputList(node->child[i],f);
}


void makeST(parsetree root)
{
	//printf("Current Node %s--Type%d\n",root->ruleNode->name,root->ruleNode->type);

	//printf("PT->%s ST->%s\n",root->ruleNode->name,currentTable->f.name);
	if(root->ruleNode->type==1 && strcmp(root->ruleNode->name,"<var_list>")==0)
	{
		if(strcmp(root->parent->ruleNode->name,"<declarationStmt>")==0)
		{
			int vtype = getType(root->parent->child[0]);
			addVariableID(root, vtype , currentTable);
		}
	}
	else if(root->ruleNode->type==1 && strcmp(root->ruleNode->name,"<parameter_list>")==0)
	{
		if(strcmp(root->next->next->tk.name,"ASSIGNOP")==0)
		{
			function f;
			f.noOfInput = 0;
			f.noOfOutput = 0;
			strcpy(f.name,root->parent->child[5]->tk.lexeme);
			f.defined = true;
			f.inputList = NULL;
			f.outputList = NULL;
			f.linedec = root->parent->child[5]->tk.lineno; //can be done outside function
			currentScope++;
			addInputList(root->parent->child[7] , &f);
			//printf("Input list added\n");
			addOutputList(root->parent->child[2],&f);
			//printf("Output list added %s\n",f.outputList->v.name);
			int start = root->parent->child[0]->tk.lineno;
			int end = root->parent->child[11]->tk.lineno;
			addFunction(f , start , end,2);
			//printf("Function added\n");
			
		}
	}
	else if(root->ruleNode->type==0 && root->tk.type==MAIN)
	{
		function f;
		f.noOfInput = 0;
		f.noOfOutput = 0;
		strcpy(f.name,"MAIN");
		f.defined = true;
		f.inputList = NULL;
		f.outputList = NULL;
		f.linedec = root->tk.lineno;
		int start = root->tk.lineno;
		int end = root->parent->child[4]->tk.lineno;
		addFunction(f,start,end,1);
	}	
	else if(root->ruleNode->type==0 && root->tk.type==IF){
		function f;
		f.noOfInput = 0;
		f.noOfOutput = 0;
		strcpy(f.name,"IF");
		f.defined = true;
		f.inputList = NULL;
		f.outputList = NULL;
		f.linedec = root->tk.lineno;
		currentScope++;
		int start = root->tk.lineno;
		int end = root->parent->child[6]->child[0]->tk.lineno;
		addFunction(f,start,end,3);
	}
	else if(root->ruleNode->type==0 && root->tk.type==ELSE){
		currentTable = currentTable->parent;
		function f;
		f.noOfInput = 0;
		f.noOfOutput = 0;
		strcpy(f.name,"ELSE");
		f.defined = true;
		f.inputList = NULL;
		f.outputList = NULL;
		f.linedec = root->tk.lineno;
		int start = root->tk.lineno;
		int end = root->parent->child[3]->tk.lineno;
		addFunction(f,start,end,4);
	}
	else if(root->ruleNode->type==0 && root->tk.type==ENDIF){
		currentTable = currentTable->parent;
		currentScope--;
	}
	// else if(root->ruleNode->type==0 && root->tk.type == IF)
	// {
	// 	SymbolTablePtr t = (SymbolTablePtr)malloc(sizeof(SymbolTableNode));
	// 	int i;
	// 	if(root->parent->child[0]->name.c == WHILE)
	// 		t->type = WH;
	// 	else if(root->parent->child[0]->name.c == FOR)
	// 		t->type = FR;
	// 	else
	// 		t->type = SW;

	// 	t->noc = 0;
	// 	t->parent = currentTable;
	// 	for(i = 0 ; i < 100 ; i++)
	// 		t->VariableTable[i] = NULL;
	// 	t->child = NULL;
	// 	currentTable->child = (SymbolTablePtr*)realloc(currentTable->child,(currentTable->noc+1)*sizeof(SymbolTablePtr));
	//     currentTable->child[currentTable->noc] = t;
	// 	currentTable->noc++;	
	// 	currentTable = t;
	// 	t->counter = 0;
	// 	currentScope++;	
	// }

	else if(root->ruleNode->type==0 && root->tk.type == END )
	{
		currentTable = currentTable->parent;
		currentScope--;
	}
		
	int i;
	for(i = 0; i < root->noc ; i++){
		//printf("Going for child %d",root->ruleNode->name);
		makeST(root->child[i]);	
	}
}


void print_symboltable(SymbolTablePtr br)
{
	int i;

	if(br->type != 5)
	{
		for(i = 0 ; i < VariableTableSize ; i++)
		{
			variablenodeptr temp = br->VariableTable[i]; 
			while(temp != NULL)
			{
				printf("%8s " , temp->v.name); 
				char arname[50];
				strcpy(arname , temp->v.name);
				//printf("%d" , temp->v.type);

				SymbolTablePtr btemp = br;

				// while(btemp->type != 2)
				// 	btemp = btemp->parent;

				int width = 1;

				if(temp->v.type == 1)
					width = 2;
				else if(temp->v.type == 2)
					width = 4;
				else if(temp->v.type == 3)
					width = 20;

				printf("%12s      %8d" , btemp->f.name, temp->v.scopeDepth+1);
				if(btemp->parent!=NULL){
					printf("%13s",btemp->parent->f.name);
				}
				else{
					printf("------------------");
				}
				printf("%13s        %d    %d\n",types[temp->v.type-1], width, soffset);
				soffset += width;

				temp = temp->next;
				//sno++;

			}
		}
	}
	for(i = 0 ; i < br->noc ; i++)
		print_symboltable(br->child[i]);	

}

void printST()
{
	printf("Identifier       Scope 	 nesting_level  scope_Parent     type     width    offset\n" );
	//sno = 1;
	print_symboltable(globalTable);
	printf( "\nSymbol table generated successfully\n" );
}