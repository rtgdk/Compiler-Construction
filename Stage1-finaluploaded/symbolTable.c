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

char* types[] = {"INTEGER" , "REAL" , "STRING","MATRIX"};

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

/*
Return type of constant in the code
*/
int getType(parsetree PT) // returns int with int,real,str and matrix, PT has noc =1
{
	tp type = PT->child[0]->tk.type;
	if(type == INT) return 1;
	if(type == REAL) return 2;
	if(type == STRING) return 3;
	if(type == MATRIX) return 4;
}


/* 
Returns width of the variable type
*/
int getWidth(int type){
	if (type==1) return 2;
	else if(type==2) return 4;
	else if(type==3) return 1;
	else if(type==4) return 2;
	else return 2;
}

/*
 Add Variable to the symboltable pointed by st
*/
void addVariable(variable v,SymbolTablePtr st){
	int hashvalue = hash4(v.name);
	variablenodeptr tempv = (variablenodeptr)malloc(sizeof(variablenode));
	tempv->next = NULL;
	tempv->v = v;
	if(st->VariableTable[hashvalue] != NULL)
	{
		variablenodeptr temp = st->VariableTable[hashvalue];
		variablenodeptr temp2 = st->VariableTable[hashvalue];
		while(temp != NULL)
		{
			if(strcmp(temp->v.name , v.name) == 0)
			{
				printf( "Line No.%d: Variable '%s' already declared at line %d. Variable redeclaration is not allowed.\n",v.linedec, v.name, temp->v.linedec, v.linedec );
				sterror = 1;
				return;
			}
			temp2 = temp;
			temp = temp->next;
		}
		temp2->next = tempv;
	}
	else{
		st->VariableTable[hashvalue] = tempv;
	}
}


/*
 Create new symbol tabe with the  function f and fill input and output varaible to the symbol table
*/
SymbolTablePtr createNewST(function f){
	SymbolTablePtr temp = (SymbolTablePtr)malloc(sizeof(SymbolTableNode));
	temp->type = 0;
	temp->noc = 0;
	temp->f = f;
	int i;
	for(i = 0 ; i < 100 ; i++)
		temp->VariableTable[i] = NULL;
	temp->parent = currentTable;
	temp->child = NULL;
	temp->start = 0;
	temp->end = 0;
	temp->counter = 0;
	variablenodeptr inp = f.inputList;
	while(inp != NULL)
	{
		addVariable(inp->v,temp);
		inp = inp->next;
	}
	variablenodeptr op = f.outputList;
	while(op != NULL)
	{
		addVariable(op->v,temp);
		op = op->next;
	}
	return temp;
}

/*
Initialise Symbol Table
*/
void initSymbolTable(){
	globalTable = (SymbolTableNode*)malloc(sizeof(SymbolTableNode));
	globalTable->type = 5; //main;
	globalTable->start = 0;
	globalTable->parent = NULL;
	globalTable->counter = 0;
	globalTable->end = 0;
	globalTable->noc = 0;
	globalTable->child = NULL;
	int i;
	for(i = 0 ; i < VariableTableSize ; i++)
		globalTable->VariableTable[i] = NULL;
	strcpy(globalTable->f.name,"PT");
	currentTable = globalTable;
	currentScope = 0;
	sterror = 0;
	totaloffset = 0;
}


/*
create to variable from node's data
*/
variable convertNodeToVariable(parsetree node){
	variable v;
	strcpy(v.name,node->tk.lexeme);
	v.linedec = node->tk.lineno;
	v.assigned = false;
	v.type = 0;
	v.scopeDepth = currentScope;
	v.width = 0;
	return v;
}

/*
Add function to tree list of currentTable with type and start and end line
*/
void addFunction(function f, int start, int end,int type) 
{	
	
	if(currentTable->noc > 0)
	{
		int i;
		for(i = 0 ; i < currentTable->noc ; i++)
		{
			if(currentTable->child[i]->type==2 && strcmp(currentTable->child[i]->f.name , f.name) == 0)
			{
				printf( "Line No.%d: Function %s already declared in line %d.Function Overloading is not allowed.\n" ,f.linedec, f.name,currentTable->child[i]->f.linedec );
				sterror = 1;
			}
		}

	}
	if(type==2){
		SymbolTablePtr temp2 = currentTable;
		while(temp2->parent!=NULL){
			if(temp2->type==2 && strcmp(temp2->f.name , f.name) == 0)
			{
				printf( "Line No.%d: Function %s already declared in line %d.Function Overloading is not allowed.\n" ,f.linedec, f.name,temp2->f.linedec );
				sterror = 1;
			}
			temp2 = temp2->parent;
		}
	}
	SymbolTablePtr temp = createNewST(f);
	temp->type = type;
	temp->start = start;
	temp->end = end;
	currentTable->child = (SymbolTablePtr*)realloc(currentTable->child,(currentTable->noc+1)*sizeof(SymbolTablePtr));
	currentTable->noc++;
	currentTable->child[currentTable->noc-1] = temp;
	currentTable = currentTable->child[currentTable->noc-1];
	return;
}


/*
 createa varaible pointer from node's data
*/
variablenodeptr createVptrFromNode(parsetree node){
	variablenodeptr vptr = (variablenodeptr)malloc(sizeof(variablenode));
	strcpy(vptr->v.name,node->tk.lexeme);
	vptr->v.linedec = node->tk.lineno;
	vptr->v.assigned = false;
	vptr->v.type = 0;
	vptr->v.width = 0;
	vptr->v.scopeDepth = currentScope; 
	vptr->next = NULL;
	return vptr;
}


/*
Add function's input list to function's symbol table
*/
void addInputList(parsetree node, function *f)
{
	if(node->ruleNode->type==0 && (node->tk.type == ID))
	{
		int vtype;
		if(strcmp(node->parent->ruleNode->name,"<parameter_list>")==0)
			vtype = getType(node->parent->child[0]);
		else
			vtype = getType(node->parent->child[1]);
		variablenodeptr vptr = createVptrFromNode(node);
		vptr->v.type = vtype;
		vptr->v.width = getWidth(vtype);
		if(vtype == 4){
			vptr->v.row = 0;
			vptr->v.col = 0;
		}
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


/*
If variable exist and is a matrix
*/
bool existAndMat(char* name,int lineno){
	if(matTable == NULL )
	{
		return false; //NOT Defined
	}
	int hashvalue = hash4(name)%VariableTableSize;
	if(matTable->VariableTable[hashvalue] == NULL)
	{
		matTable = matTable->parent;
		return existAndMat(name ,lineno);
	}
	else
	{
		variablenodeptr temp = matTable->VariableTable[hashvalue];

		while(temp != NULL)
		{
			if(temp->v.type == 4 && strcmp(temp->v.name , name) == 0 ) //anthing else??
			{
				if(temp->v.linedec <= lineno){
					matVar = temp;
					return true;
				}
				else{
					return false;
				}
			}
			temp = temp->next;
		}
		matTable = matTable->parent;
		return existAndMat(name , lineno);
	}
}


/*
Add node's variable node ptr to symboltable st
*/
void addVariableID(parsetree node, int type, SymbolTablePtr st)
{
	if(node->ruleNode->type==0 && node->tk.type == ID)
	{
		variable v= convertNodeToVariable(node);
		v.type = type;
		if(type==4){
			v.row = 0;
			v.col = 0;
		}
		v.width = getWidth(type);
		addVariable(v, st);
		return;
	}
	int i;
	for(i = 0; i < node->noc ; i++)
		addVariableID(node->child[i],type,st);
}



/*
If variable exist and is a string
*/
bool existAndStr(char* name,int lineno){
	if(strTable == NULL )
	{
		return false; //NOT Defined
	}
	int hashvalue = hash4(name)%VariableTableSize;
	if(strTable->VariableTable[hashvalue] == NULL)
	{
		strTable = strTable->parent;
		return existAndStr(name ,lineno);
	}
	else
	{
		variablenodeptr temp = strTable->VariableTable[hashvalue];

		while(temp != NULL)
		{
			if(temp->v.type == 3 && strcmp(temp->v.name , name) == 0 ) //anthing else??
			{
				if(temp->v.linedec <= lineno){
					strVar = temp;
					return true;
				}
				else{
					return false;
				}
					
			}
			temp = temp->next;
		}
		strTable = strTable->parent;
		return existAndStr(name , lineno);
	}
}

/*
Create a function with name "name"
*/
function createFunction(char* name){
	function f;
	f.noOfInput = 0;
	f.noOfOutput = 0;
	strcpy(f.name,name);
	f.inputList = NULL;
	f.outputList = NULL;
	f.linedec = 0; //can be done outside function
	return f;
}

/*
Add function's output list to function's symbol table
*/
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
		variablenodeptr vptr = createVptrFromNode(node);
		vptr->v.type = vtype;
		vptr->v.width = getWidth(vtype);
		if(vtype == 4){
			vptr->v.row = 0;
			vptr->v.col = 0;
		}
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



/*
Calculate columns of the matrix row rhs
*/
void calculateCol(parsetree rhs,int* count2){
	if(strcmp(rhs->child[0]->ruleNode->name,"EPSILON")==0){
		return;
	}
	else{
		*count2 = *count2+ 1;
		return calculateCol(rhs->child[2],count2);
	}
}

/*
Calculate rows of the matrix
*/
bool calculateRow(parsetree rhs,int* count1,int* count2){
	if(strcmp(rhs->child[0]->ruleNode->name,"EPSILON")==0){
		return true;
	}
	else{
		*count1 = *count1 + 1;
		int count3 = 1;
		calculateCol(rhs->child[1]->child[1],&count3);
		if (count3!=(*count2)) {
			return false;
		}
		return calculateRow(rhs->child[2],count1,count2);
	}
}

/*
Update matrix  no of rows and columns
*/
void updateMatSize(parsetree rhs) // rhs is RHS TYPE1
{
	if(strcmp(rhs->child[0]->ruleNode->name,"<arithmeticExpression>")!=0){
		return;
	}
	else{
		if(strcmp(rhs->child[0]->child[0]->child[0]->child[0]->ruleNode->name,"<var>")!=0){ //TYPE1->AE->AT->AF->var
			return;
		}
		else{
			if(strcmp(rhs->child[0]->child[0]->child[0]->child[0]->child[0]->ruleNode->name,"<matrix>")!=0){ //TYPE1->AE->AT->AF->var->matrix
				return;
			}
			else{
				int nor = 1;
				int noc = 1;
				calculateCol(rhs->child[0]->child[0]->child[0]->child[0]->child[0]->child[1]->child[0]->child[1],&noc); //matrix->rows->row->remainingcol
				if (noc>10){
					printf("Line No. %d: Matrix %s can have maximum 10 columns\n",rhs->child[0]->child[0]->child[0]->child[0]->child[0]->child[1]->child[0]->child[0]->tk.lineno,matVar->v.name);
					return;
				}
				bool cr = calculateRow(rhs->child[0]->child[0]->child[0]->child[0]->child[0]->child[1]->child[1],&nor,&noc); //matrix->rows->moreRows
				if(!cr) {
					printf("Line No. %d: Mismatch no of columns while assigning matrix %s\n",rhs->child[0]->child[0]->child[0]->child[0]->child[0]->child[1]->child[0]->child[0]->tk.lineno,matVar->v.name);
					return;
				}
				if (nor>10){
					printf("Line No. %d: Matrix %s can have maximum 10 rows\n",rhs->child[0]->child[0]->child[0]->child[0]->child[0]->child[1]->child[0]->child[0]->tk.lineno,matVar->v.name);
					return;
				}
				matVar->v.col = noc;
				matVar->v.row = nor;
				matVar->v.width = noc*nor;
				matVar->v.assigned = true;
				return;
			}
		}
	}
}

/*
Update string size with length of STR constant in the code
*/
void updateStrSize(parsetree rhs) // rhs is RHS TYPE1
{
	if(strcmp(rhs->child[0]->ruleNode->name,"<arithmeticExpression>")!=0){
		return;
	}
	else{
		if(strcmp(rhs->child[0]->child[0]->child[0]->child[0]->ruleNode->name,"<var>")!=0){ //TYPE1->AE->AT->AF->var
			return;
		}
		else{
			if(rhs->child[0]->child[0]->child[0]->child[0]->child[0]->tk.type!=STR){ //TYPE1->AE->AT->AF->var->STR
				return;
			}
			else{
				strVar->v.width = strlen(rhs->child[0]->child[0]->child[0]->child[0]->child[0]->tk.lexeme)-2; // coz "" also included in str
				strVar->v.assigned = true;
				return;
			}
		}
	}
}



/*
Utility function for making Symbol Table
*/
void makeST(parsetree PT)
{
	if(PT->ruleNode->type==1 && strcmp(PT->ruleNode->name,"<var_list>")==0)
	{
		if(strcmp(PT->parent->ruleNode->name,"<declarationStmt>")==0)
		{
			int vtype = getType(PT->parent->child[0]);
			addVariableID(PT, vtype , currentTable);
		}
	}
	else if(PT->ruleNode->type==1 && strcmp(PT->ruleNode->name,"<leftHandSide_singleVar>")==0)
	{	
		matTable = currentTable;;
		strTable = currentTable;
		if (existAndMat(PT->child[0]->tk.lexeme,PT->child[0]->tk.lineno)){ //ID
			updateMatSize(PT->parent->child[2]); //RHSType1
		}
		else if(existAndStr(PT->child[0]->tk.lexeme,PT->child[0]->tk.lineno)){
			updateStrSize(PT->parent->child[2]); //RHSType1
		}
	}
	else if(PT->ruleNode->type==1 && strcmp(PT->ruleNode->name,"<parameter_list>")==0)
	{
		if(strcmp(PT->next->next->tk.name,"ASSIGNOP")==0)
		{
			function f = createFunction(PT->parent->child[5]->tk.lexeme);
			f.linedec = PT->parent->child[5]->tk.lineno; //can be done outside function
			currentScope++;
			addInputList(PT->parent->child[7] , &f);
			addOutputList(PT->parent->child[2],&f);
			int start = PT->parent->child[0]->tk.lineno;
			int end = PT->parent->child[11]->tk.lineno;
			addFunction(f , start , end,2);	
		}
	}
	else if(PT->ruleNode->type==0 && PT->tk.type==MAIN)
	{
		function f = createFunction("MAIN");
		f.linedec = PT->tk.lineno;
		int start = PT->tk.lineno;
		int end = PT->parent->child[4]->tk.lineno;
		addFunction(f,start,end,1);
	}	
	else if(PT->ruleNode->type==0 && PT->tk.type==IF){
		function f = createFunction("IF");
		f.linedec = PT->tk.lineno;
		currentScope++;
		int start = PT->tk.lineno;
		int end = PT->parent->child[6]->child[0]->tk.lineno;
		addFunction(f,start,end,3);
	}
	else if(PT->ruleNode->type==0 && PT->tk.type==ELSE){
		currentTable = currentTable->parent;
		function f = createFunction("ELSE");
		f.linedec = PT->tk.lineno;
		int start = PT->tk.lineno;
		int end = PT->parent->child[3]->tk.lineno;
		addFunction(f,start,end,4);
	}
	else if(PT->ruleNode->type==0 && PT->tk.type==ENDIF){
		currentTable = currentTable->parent;
		currentScope--;
	}
	else if(PT->ruleNode->type==0 && PT->tk.type == END )
	{
		currentTable = currentTable->parent;
		currentScope--;
	}
		
	int i;
	for(i = 0; i < PT->noc ; i++){
		makeST(PT->child[i]);	
	}
}

/*
To print Symbol Table
*/
void printST(SymbolTablePtr st)
{
	int i;

	if(st->type != 5)
	{
		for(i = 0 ; i < VariableTableSize ; i++)
		{
			variablenodeptr temp = st->VariableTable[i]; 
			while(temp != NULL)
			{
				printf("%8s " , temp->v.name); 
				char arname[50];
				strcpy(arname , temp->v.name);
				SymbolTablePtr st1 = st;
				int width = 1;
				if(temp->v.type == 1)
					width = 2;
				else if(temp->v.type == 2)
					width = 4;
				else if(temp->v.type == 3)
					width = temp->v.width;
				else if(temp->v.type == 4)
					width = temp->v.width*2 ; /// coz in has size 2
				printf("%20s      %8d" , st1->f.name, temp->v.scopeDepth+1);
				if(st1->parent!=NULL){
					printf("%20s",st1->parent->f.name);
				}
				else{
					printf("------------------");
				}
				printf("%13s        %5d    %5d\n",types[temp->v.type-1], width, totaloffset);
				totaloffset += width;
				temp = temp->next;
			}
		}
	}
	for(i = 0 ; i < st->noc ; i++)
		printST(st->child[i]);	

}

/*
Utility function for printing symbol table
*/
void printSTUtility()
{
	totaloffset = 0;
	printf("Identifier             Scope 	 nesting_level     scope_Parent         type         width    offset\n" );
	printST(globalTable);
}