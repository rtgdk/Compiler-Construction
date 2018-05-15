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

saerror = 0;
char* types1[] = {"INTEGER" , "REAL" , "STRING","MATRIX"};


void initialiseSemantics(){
	saerror = 0;
}
/* 
Returns varaibel node ptr from the symbol table
*/
variablenodeptr getID(char* name , SymbolTablePtr st) // return variable nodeptr
{	
	if(st == NULL)
		return NULL;

	int hashvalue = hash4(name)%VariableTableSize;
	if(st->VariableTable[hashvalue] != NULL)
	{
		variablenodeptr temp = st->VariableTable[hashvalue];
		while(temp != NULL)
		{
			if(strcmp(temp->v.name , name) == 0)
			{
				return temp;
			}
			temp = temp->next;
		}
		return getID(name , st->parent);
	}
	else
	{
		return getID(name , st->parent);
	}
}


/*
Returns type of vraiable from symbol table
*/
int getTypeID(char* name , SymbolTablePtr st, int lineno)
{	
	if(st == NULL )
	{
		printf("Line No.%d : Variable '%s' not declared. \n",lineno,name);
		saerror = 1;
		return 5; //NOT Defined
	}
	int hashvalue = hash4(name)%VariableTableSize;
	if(st->VariableTable[hashvalue] != NULL)
	{
		variablenodeptr temp = st->VariableTable[hashvalue];
		while(temp != NULL)
		{
			if(strcmp(temp->v.name , name) == 0 ) 
			{
				if(temp->v.linedec <= lineno){
					return (temp->v.type);
				}
				else{
						printf("Line No.%d : Variable '%s' not declared yet. Declaration found later in line %d. Variable cannot be used before declaration.\n",lineno,name,temp->v.linedec);
						saerror = 1;
						return 5;
				}	
			}
			temp = temp->next;
		}
		return getTypeID(name , st->parent, lineno);
	}
	else
	{
		return getTypeID(name , st->parent, lineno);
	}
}



/*
Returns function with function name fname
*/
function checkFunction(char* fname, SymbolTablePtr st, int lineno) //Check if function recursion
{
	SymbolTablePtr temp = st; 
	int i;
	function f;
	f.noOfInput = -1;
	f.noOfOutput = -1;
	while(temp->parent!=NULL){
		if(strcmp(temp->f.name,fname)!=0){
			for(i = 0 ; i < temp->noc ; i++)
			{
				if(strcmp(temp->child[i]->f.name , fname) == 0)
				{
					if(temp->child[i]->f.linedec > lineno )
					{
						printf( "Line No.%d : Function '%s' not declared yet. Defination found later in line %d. Function cannot be called before defination.\n" , lineno, fname ,temp->child[i]->f.linedec);
						saerror = 1;
						return f;
					}
					else{
						return temp->child[i]->f;
					}
				}	
			}	
		}
		else{
			printf( "Line No.%d : Recursion call to function '%s' not allowed\n" , lineno, fname );
			saerror = 1;
			return f;
		}
		temp = temp->parent;
	}
	printf( "Line No.%d : Function '%s' not declared.\n" , lineno, fname );
	saerror = 1;
	return f;	
}

/*
Add output list to the called function f.outputlist
*/
bool outputType(AStree ast , function *f)
{
	int i;
	if(ast->ruleNode->type==0){
		if(ast->tk.type == ID)
		{
			variablenodeptr vptr = createVptrFromNode(ast);
			vptr->v.type = getTypeID(ast->tk.lexeme , ast->st, ast->tk.lineno); 
			f->noOfOutput++;
			if(vptr->v.type == 5)
			{
				return false;
			}	
			if(f->outputList != NULL)
				variablenodeptr temp = f->outputList;
				while(temp->next != NULL)
				{
					temp = temp->next;
				}
				temp->next = vptr;
			}
			else{
				f->outputList = vptr;
			}		
		}
	}
	for(i = 0 ; i < ast->noc ; i++)
	{
		if(outputType(ast->child[i] , f)==false)
			return false;
	}
	return true;
}

/*
Mark the variable assigned
*/
void markAssigned(variablenodeptr v , SymbolTablePtr st) // mark for assigned
{
	int hashvalue = hash4(v->v.name)%VariableTableSize;

	if(st->VariableTable[hashvalue] != NULL)
	{
		variablenodeptr temp = st->VariableTable[hashvalue];

		while(temp != NULL)
		{
			if(strcmp(temp->v.name , v->v.name) == 0)
			{
				temp->v.assigned = true;
				return;
			}
			temp = temp->next;
		}
		return markAssigned(v , st->parent);
	}
	else{
		markAssigned(v , st->parent);
	}
}

/*
Check whether given operator can be applied to the two types1
*/
bool checkOperatorType(AStree op, AStree ex1, AStree ex2){
	if(op->child[0]->tk.type == PLUS){
		if(ex1->type==ex2->type) {
			if(ex1->type==4){
				AStree temp = ex1;
				while(strcmp(temp->ruleNode->name,"<var>")!=0){
					temp = temp->child[0];
				}
				temp = temp->child[0];
				AStree temp2 = ex2;
				while(strcmp(temp2->ruleNode->name,"<var>")!=0){
					temp2 = temp2->child[0];
				}
				temp2 = temp2->child[0];
				variablenodeptr v1 = getID(temp->tk.lexeme,temp->st);
				variablenodeptr v2 = getID(temp2->tk.lexeme,temp2->st);
				if(v1->v.row!=v2->v.row || v1->v.col!=v2->v.col){
					saerror = 1;
					printf( "Line No. %d: Matrix Size mismatch for '%s' operator. Got %dx%d(%s) on left and %dx%d rows(%s) on right side\n",op->child[0]->tk.lineno, op->child[0]->tk.lexeme, v1->v.row,v1->v.col,v1->v.name,v2->v.row,v2->v.col,v2->v.name);
					return false;
				}
			}
			return true;
		}

		else {
			saerror = 1;
			printf( "Line No. %d: Type mismatch for '%s' operator. Got %s on left and %s on right side\n",op->child[0]->tk.lineno, op->child[0]->tk.lexeme, types1[ex1->type-1],types1[ex2->type-1]);
		}
		
	}
	else if(op->child[0]->tk.type == MINUS){
		if(ex1->type==ex2->type){
			if (ex1->type==1 || ex1->type==2 || ex1->type==4) {
				if(ex1->type==4){
					AStree temp = ex1;
					while(strcmp(temp->ruleNode->name,"<var>")!=0){
						temp = temp->child[0];
					}
					temp = temp->child[0];
					AStree temp2 = ex2;
					while(strcmp(temp2->ruleNode->name,"<var>")!=0){
						temp2 = temp2->child[0];
					}
					temp2 = temp2->child[0];
					variablenodeptr v1 = getID(temp->tk.lexeme,temp->st);
					variablenodeptr v2 = getID(temp2->tk.lexeme,temp2->st);
					if(v1->v.row!=v2->v.row || v1->v.col!=v2->v.col){
						saerror = 1;
						printf( "Line No. %d: Matrix Size mismatch for '%s' operator. Got %dx%d(%s) on left and %dx%d rows(%s) on right side\n",op->child[0]->tk.lineno, op->child[0]->tk.lexeme, v1->v.row,v1->v.col,v1->v.name,v2->v.row,v2->v.col,v2->v.name);
						return false;
					}
				}
				return true;
			}
			else {
				saerror = 1;
				printf( "Line No. %d: Type mismatch for '%s' operator. '%s' does not support '%s' operation.\n",op->child[0]->tk.lineno, op->child[0]->tk.lexeme,types1[ex1->type-1],op->child[0]->tk.lexeme);
			}
			
		}
		else{
			saerror = 1;
			printf( "Line No. %d: Type mismatch for '%s' operator. Got %s on left and %s on right side\n",op->child[0]->tk.lineno, op->child[0]->tk.lexeme, types1[ex1->type-1],types1[ex2->type-1]);
		}
		
	}
	else if(op->child[0]->tk.type == MUL){
		if(ex1->type==ex2->type){
			if (ex1->type==1 || ex1->type==2) return true;
			else {
				saerror = 1;
				printf( "Line No. %d: Type mismatch for '%s' operator. '%s' does not support '%s' operation.\n",op->child[0]->tk.lineno, op->child[0]->tk.lexeme,types1[ex1->type-1],op->child[0]->tk.lexeme);
			}
		}
		else{
			saerror = 1;
			printf( "Line No. %d: Type mismatch for '%s' operator. Got %s on left and %s on right side\n",op->child[0]->tk.lineno, op->child[0]->tk.lexeme, types1[ex1->type-1],types1[ex2->type-1]);
		}
	}
	else if(op->child[0]->tk.type == DIV){
		if(ex1->type==ex2->type){
			if (ex1->type==1 || ex1->type==2 ) return true;
			else {
				saerror = 1;
				printf( "Line No. %d: Type mismatch for '%s' operator. '%s' does not support '%s' operation.\n",op->child[0]->tk.lineno, op->child[0]->tk.lexeme,types1[ex1->type-1],op->child[0]->tk.lexeme);
			}
			
		}
		else{
			saerror = 1;
			printf( "Line No. %d: Type mismatch for '%s' operator. Got %s on left and %s on right side\n",op->child[0]->tk.lineno, op->child[0]->tk.lexeme, types1[ex1->type-1],types1[ex2->type-1]);
		}
	}
	return false;
}


/*
Check compatibility of the two functions
*/
bool compatibleFunction(function f1, function f2, int line) //check if defined and called fn are type checked
{	
	variablenodeptr inp1 = f1.inputList;
	if(f1.noOfInput != f2.noOfInput)
	{
		saerror = 1;
		printf( "Line No.%d : Count of input arguments(%d) passed to the function call '%s' do not match with count of input parameters(%d) in function defination\n" , line,f1.noOfInput, f2.name,f2.noOfInput );
		return false;
	}
	variablenodeptr out1 = f1.outputList;
	if(f1.noOfOutput != f2.noOfOutput)
	{
		saerror = 1;
		printf( "Line No.%d : Count of output variables(%d) passed to the function call '%s' do not match with count of output parameters(%d) in function defination\n" , line, f1.noOfOutput, f2.name, f2.noOfOutput );
		return false;
	}
	variablenodeptr inp2 = f2.inputList;
	int count = 1;
	variablenodeptr out2 = f2.outputList;
	while(inp1 != NULL)
	{
		if(inp1->v.type != inp2->v.type)
		{
			saerror = 1;
			printf( "Line No.%d : Mismatch in input argument %d of function call '%s' : Expected type %s but found type %s\n",f1.linedec,count,f2.name,types1[inp2->v.type-1],types1[inp1->v.type-1] );
			return false;
		}
		count++;
		inp1 = inp1->next;
		inp2 = inp2->next;
	}

	count = 1;
	while(out1 != NULL)
	{
		if(out1->v.type != out2->v.type)
		{
			saerror = 1;
			printf( "Line No.%d : Mismatch in output argument %d of function call '%s' : Expected type %s but found type %s\n",line,count,f2.name,types1[out2->v.type-1],types1[out1->v.type-1] );
			return false;
		}
		count++;
		out1 = out1->next;
		out2 = out2->next;
	}
	return true;
}

/*
Add input list to the called function f.inputlist
*/
bool inputType(AStree ast , function *f)
{
	int i;
	if(ast->ruleNode->type==0){
		if(ast->tk.type == ID)
		{
			int vt = getTypeID(ast->tk.lexeme,ast->st,ast->tk.lineno);
			if(vt == 5)
			{
				return false;
			}
			f->noOfInput++;
			variablenodeptr vptr = createVptrFromNode(ast);
			vptr->v.scopeDepth = 0;
			vptr->v.type = vt; 
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
		else if(ast->tk.type == NUM){
			f->noOfInput++;
			variablenodeptr vptr = createVptrFromNode(ast);
			vptr->v.scopeDepth = 0;
			vptr->v.type = 1; 
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
		else if(ast->tk.type == RNUM){
			f->noOfInput++;
			variablenodeptr vptr = createVptrFromNode(ast);
			vptr->v.scopeDepth = 0;
			vptr->v.type = 2; 
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
		else if(ast->tk.type == STR){
			f->noOfInput++;
			variablenodeptr vptr = createVptrFromNode(ast);
			vptr->v.scopeDepth = 0;
			vptr->v.type = 3; 
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
	}
	for(i = 0 ; i < ast->noc ; i++){
		bool res = inputType(ast->child[i] , f);
		if(!res)
			return false;
	}
	return true;
}

/*
Check <rightHandSide_type2> of the grammer
*/
bool checkRightSideType2(AStree ast,function* f){
	if(strcmp(ast->ruleNode->name,"<sizeExpression>")==0)
	{
		int vt = getTypeID(ast->child[0]->tk.lexeme,ast->child[0]->st,ast->child[0]->tk.lineno);
		if (vt==4){
			// variablenodeptr vptr = getID(ast->child[0]->tk.lexeme,ast->child[0]->st);
			// function f1;
			// f1.noOfOutput = 2;
			// variablenodeptr vptr1 = (variablenodeptr)malloc(sizeof(variablenode));
			// variablenodeptr vptr2 = (variablenodeptr)malloc(sizeof(variablenode));
			// vptr1->v.type = 1;
			// vptr2->v.type = 1;
			// vptr1->next = vptr2;
			// vptr2->next = NULL;
			// f1.outputList = vptr1;
			if(f->noOfOutput!=2){
				saerror = 1;
				printf("Line No.%d: Size Operation on '%s' returns 2 values but %d output values given.\n",ast->child[0]->tk.lineno,types1[vt-1],f->noOfOutput);
				return false;
			}
			if(f->outputList->v.type!=1){
				saerror = 1;
				printf("Line No.%d: Type Mismatch- Size Operation on '%s' returns 2 INTEGER values but %s(%s) output type given.\n",ast->child[0]->tk.lineno,types1[vt-1],types1[f->outputList->v.type-1],f->outputList->v.name);
				return false;
			}
			if(f->outputList->next->v.type!=1){
				saerror = 1;
				printf("Line No.%d: Type Mismatch- Size Operation on '%s' returns 2 INTEGER values but %s(%s) output type given.\n",ast->child[0]->tk.lineno,types1[vt-1],types1[f->outputList->next->v.type-1],f->outputList->next->v.name);
				return false;
			}
			variablenodeptr temp = f->outputList;
			while(temp != NULL)
			{
				markAssigned(temp , ast->child[0]->st);
				temp = temp->next;
			}
			return true;
		}
		else if(vt==1 || vt==2){
			saerror = 1;
			printf("Line No.%d: Size Operation on '%s' is invalid.\n",ast->child[0]->tk.lineno,types1[vt-1]);
			ast->type = 5;
			return false;
		}
		else if(vt==3){
			//ariablenodeptr vptr = getID(ast->child[0]->tk.lexeme,ast->child[0]->st);
			// f->noOfOutput = 1;
			// variablenodeptr vptr1 = (variablenodeptr)malloc(sizeof(variablenode));
			// vptr1->v.type = 1;
			// vptr1->next = NULL;
			// f->outputList = vptr1;
			if(f->noOfOutput!=1){
				saerror = 1;
				printf("Line No.%d: Size Operation on '%s' returns 1 value but %d output values given.\n",ast->child[0]->tk.lineno,types1[vt-1],f->noOfOutput);
				return false;
			}
			if(f->outputList->v.type!=1){
				saerror = 1;
				printf("Line No.%d: Type Mismatch- Size Operation on '%s' returns an INTEGER value but %s(%s) output type given.\n",ast->child[0]->tk.lineno,types1[vt-1],types1[f->outputList->v.type-1],f->outputList->v.name);
				return false;
			}
			variablenodeptr temp = f->outputList;
			while(temp != NULL)
			{
				markAssigned(temp , ast->child[0]->st);
				temp = temp->next;
			}
			return true;
		}
		else{
			ast->type = 5;
			return false;
		}

	}
	else if(strcmp(ast->ruleNode->name,"<funCallStmt>")==0)
	{
		function fn = checkFunction(ast->child[0]->tk.lexeme , ast->child[0]->st, ast->child[0]->tk.lineno);
		if(fn.noOfInput == -1){
			ast->type=5;
			return false;
		}
		if(inputType(ast->child[1] , f)==false){
			ast->type=5;
			return false;
		}
		if(compatibleFunction(*f , fn, ast->child[0]->tk.lineno)==false){
			ast->type = 5; // not defined
			return false;
		}
		variablenodeptr temp = f->outputList;
		while(temp != NULL)
		{
			markAssigned(temp , ast->child[0]->st);
			temp = temp->next;
		}
		return true;
	}
}

/*
Check <ioStmt> of the grammer
*/
bool checkIOStmt(AStree ast){
	if(strcmp(ast->child[0]->ruleNode->name,"READ")==0)
	{	
		int vt = getTypeID(ast->child[1]->tk.lexeme,ast->child[1]->st,ast->child[1]->tk.lineno);
		if(vt==5){
			return false;
		}
		else if(vt==3 || vt==4){
			saerror = 1;
			printf( "Line No.%d : Inavlid Type '%s' for READ function. Only INTEGER and REAL types1 allowed.\n" , ast->child[1]->tk.lineno, types1[vt-1]);
			return false;
		}
		return true;
	}
	else if(strcmp(ast->child[0]->ruleNode->name,"PRINT")==0){
		int vt = getTypeID(ast->child[1]->tk.lexeme,ast->child[1]->st,ast->child[1]->tk.lineno);
		if(vt==5){
			return false;
		}
		return true;
	}
	return false;
}

/*
Check <rightHandSide_type1> of the grammer
*/
void checkRightSideType1(AStree ast) // type checking in expression
{
	//printf("-----%s\n",ast->ruleNode->name);
	if(strcmp(ast->ruleNode->name,"<rightHandSide_type1>")==0)
	{
		checkRightSideType1(ast->child[0]);
		ast->type = ast->child[0]->type;
		return;
	}
	else if(strcmp(ast->ruleNode->name,"<arithmeticExpression>")==0)
	{
		checkRightSideType1(ast->child[0]);
		if(ast->child[0]->type == 5){
			ast->type = 5;
			return;
		}
		//printf("AE\n");
		if(ast->noc==1){
			ast->type = ast->child[0]->type;
			return;
		}
		checkRightSideType1(ast->child[1]);
		if(ast->child[1]->type == 5){
			ast->type = 5;
			return;
		}
		if(checkOperatorType(ast->child[1]->child[0],ast->child[0],ast->child[1]->child[1])){
			ast->type = ast->child[0]->type;
			return;
		}
		else{
			ast->type = 5;
			return;
		}
		return;
	}
	else if(strcmp(ast->ruleNode->name,"<arithmeticTerm>")==0)
	{
		checkRightSideType1(ast->child[0]);
		if(ast->child[0]->type == 5){
			ast->type = 5;
			return;
		}
		//printf("AT %d\n",ast->noc);
		if(ast->noc==1){
			//printf("AT2\n");
			ast->type = ast->child[0]->type;
			return;
		}
		//printf("AT3\n");
		checkRightSideType1(ast->child[1]);
		if(ast->child[1]->type == 5){
			ast->type = 5;
			return;
		}
		if(checkOperatorType(ast->child[1]->child[0],ast->child[0],ast->child[1]->child[1])){
			if(ast->child[1]->child[0]->child[0]->tk.type==DIV && ast->child[0]->type==1){
				ast->type = 2;
			}
			else {
				ast->type = ast->child[0]->type;
			}

			return;
		}
		else{
			ast->type = 5;
			return;
		}
		return;
	}
	else if(strcmp(ast->ruleNode->name,"<arithmeticFactor>")==0)
	{
		checkRightSideType1(ast->child[0]);
		//printf("AF\n");
		if(ast->child[0]->type==5){
			ast->type = 5;
		}
		else{
			ast->type = ast->child[0]->type;
		}
		return;
	}
	else if(strcmp(ast->ruleNode->name,"<moreTerms_two>")==0)
	{
		checkRightSideType1(ast->child[1]);
		if(ast->child[1]->type==5){
			ast->type = 5;
			return;
		}
		if(ast->noc==2){ // epsilon
			ast->type = ast->child[1]->type;
			return;
		}
		else{
			checkRightSideType1(ast->child[2]);
			if(ast->child[2]->type==5){
				ast->type = 5;
				return;
			}
			//printf("In here %s\n",ast->child[2]->child[0]->child[0]->ruleNode->name);
			if(checkOperatorType(ast->child[2]->child[0],ast->child[1],ast->child[2]->child[1])){

				if(ast->child[2]->child[0]->child[0]->tk.type==DIV && ast->child[1]->type==1){
					ast->type = 2;
				}
				else {
					ast->type = ast->child[1]->type;
				}
				return;
			}
			else{
				ast->type=5;
				return;
			}
		}
	}
	
	else if(strcmp(ast->ruleNode->name,"<moreTerms_one>")==0){
		checkRightSideType1(ast->child[1]);
		if(ast->child[1]->type==5){
			ast->type = 5;
			return;
		}
		if(ast->noc==2){ // epsilon
			ast->type = ast->child[1]->type;
			return;
		}
		else{
			checkRightSideType1(ast->child[2]);
			if(ast->child[2]->type==5){
				ast->type = 5;
				return;
			}
			if(checkOperatorType(ast->child[2]->child[0],ast->child[1],ast->child[2]->child[1])){
				ast->type = ast->child[1]->type;
				return;
			}
			else{
				ast->type=5;
				return;
			}
		}
		return;
	}

	else if(strcmp(ast->ruleNode->name,"<sizeExpression>")==0)
	{
		int vt = getTypeID(ast->child[0]->tk.lexeme,ast->child[0]->st,ast->child[0]->tk.lineno);
		if (vt==3){
			ast->type = 1;
			return;
		}
		else if(vt==1 || vt==2){
			saerror = 1;
			printf("Line No.%d: Size Operation on '%s' is invalid.\n",ast->child[0]->tk.lineno,types1[vt-1]);
			ast->type = 5;
			return;
		}
		else if(vt==4){
			saerror = 1;
			printf("Line No.%d: Size Operation on '%s' returns 2 values, only 1 given on left hand side\n",ast->child[0]->tk.lineno,types1[vt-1]);
			ast->type = 5;
			return;

		}
		else{
			ast->type = vt;
			return;
		}
	}
	else if(strcmp(ast->ruleNode->name,"<funCallStmt>")==0)
	{
		function fn = checkFunction(ast->child[0]->tk.lexeme , ast->child[0]->st, ast->child[0]->tk.lineno);
		if(fn.noOfInput == -1){
			ast->type=5;
			return;
		}
		function f = createFunction("FunctionCall");
		f.linedec = ast->child[0]->tk.lineno;
		if (outputType(ast->parent->parent->child[0], &f)==false){
			ast->type=5;
			return;
		}
		if(inputType(ast->child[1] , &f)==false){
			ast->type=5;
			return;
		}
		int type;
		if(compatibleFunction(f , fn, ast->child[0]->tk.lineno)==false){
			ast->type = 5; // not defined
			return;
		}
		variablenodeptr temp = f.outputList;
		while(temp != NULL)
		{
			markAssigned(temp , ast->child[0]->st);
			temp = temp->next;
		}
		ast->type = f.outputList->v.type;
		return;
	}
	
	else if(strcmp(ast->ruleNode->name,"<var>")==0){

		if(ast->child[0]->tk.type == NUM)
		{
			ast->type = 1;
		}
		else if(ast->child[0]->tk.type == RNUM)
		{
			ast->type = 2;
		}
		else if(ast->child[0]->tk.type == STR)
		{
			//printf("IN STR\n");
			ast->type = 3;
		}
		else if(strcmp(ast->child[0]->ruleNode->name,"<matrix>")==0)
		{
			ast->type = 4;
		}
		else if(ast->child[0]->tk.type == ID)
		{
			int vt = getTypeID(ast->child[0]->tk.lexeme,ast->child[0]->st,ast->child[0]->tk.lineno);
			if(vt == 5)
			{
				ast->type = 5;
				return;
			}
			else if(vt==4){
				if(ast->noc==2 && ast->child[1]->child[0]->tk.type==NUM){
					ast->type = 1;
					return;
				}
				ast->type = vt;
				return;
			}
			else{
				ast->type = vt;
				return;
			}
		}
		return;
	}
	
}

/*
Check <relationalOp> of the grammer
*/
bool checkRelationalOp(AStree ast){
	tp c1 = ast->parent->child[0]->child[0]->tk.type;
	tp c2 = ast->parent->child[2]->child[0]->tk.type;
	if(c1==ID){
		int vt = getTypeID(ast->parent->child[0]->child[0]->tk.lexeme,ast->parent->child[0]->child[0]->st,ast->parent->child[0]->child[0]->tk.lineno);
		if(vt == 5)
		{
			ast->type = 5;
			ast->parent->child[0]->type =  5;
			return false;
		}
		else if(vt==3 || vt == 4){
			saerror = 1;
			printf( "Line No.%d : Invalid Type '%s'(%s) for '%s' operation. Only INTEGER and REAL types1 allowed for relational operations.\n" , ast->child[0]->tk.lineno, types1[vt-1],ast->parent->child[0]->child[0]->tk.lexeme, ast->child[0]->tk.lexeme);
			ast->type = vt;
			return false;
		}
		else {
			ast->parent->child[0]->type =  vt;
		}
	}
	else if(c1 == NUM){
		ast->parent->child[0]->type = 1; //INT
	}
	else if(c1 == RNUM){
		ast->parent->child[0]->type = 2; //REAL
	}

	if(c2==ID){
		int vt = getTypeID(ast->parent->child[2]->child[0]->tk.lexeme,ast->parent->child[2]->child[0]->st,ast->parent->child[2]->child[0]->tk.lineno);
		if(vt == 5)
		{
			ast->type = 5;
			return false;
		}
		else if(vt==3 || vt == 4){
			saerror = 1;
			printf( "Line No.%d : Invalid Type '%s'(%s) for '%s' operation. Only INTEGER and REAL types1 allowed for relational operations.\n" , ast->child[0]->tk.lineno, types1[vt-1],ast->parent->child[2]->child[0]->tk.lexeme, ast->child[0]->tk.lexeme);
			ast->type = vt;
			return false;
		}
		else {
			ast->parent->child[2]->type =  vt;
		}


	}
	else if(c2 == NUM){
		ast->parent->child[2]->type = 1; //INT
	}
	else if(c2 == RNUM){
		ast->parent->child[2]->type = 2; //REAL
	}
	if(ast->parent->child[0]->type!= ast->parent->child[2]->type){
		saerror = 1;
		printf( "Line No.%d : Type Mismatch for '%s' operation. Got %s(%s) on left and %s(%s) on right.\n" , ast->child[0]->tk.lineno,ast->child[0]->tk.lexeme ,types1[ast->parent->child[0]->type-1],ast->parent->child[0]->child[0]->tk.lexeme,types1[ast->parent->child[2]->type-1],ast->parent->child[2]->child[0]->tk.lexeme);
		ast->type = 5;
		return false;
	}
	return true;

}

/*
Utility function for semantic analysis
*/

void semanticAnalysis(AStree ast)
{
	//printf("----%s\n",ast->ruleNode->name);
	if(ast->ruleNode->type==0 && ast->tk.type == ID)
	{
		if(strcmp(ast->parent->ruleNode->name,"<leftHandSide_singleVar>")==0)
		{
			int vt = getTypeID(ast->tk.lexeme , ast->st, ast->tk.lineno);
			ast->type = vt;
			if(vt == 5)
			{
				return;
			}
			variablenodeptr temp = getID(ast->tk.lexeme , ast->st);
			markAssigned(temp , ast->st);
			if(strcmp(ast->parent->parent->child[1]->ruleNode->name,"<rightHandSide_type1>")==0)
			{
				//printf("Here\n");
				checkRightSideType1(ast->parent->parent->child[1]);
				//printf("Here\n");
				int t = ast->parent->parent->child[1]->type;
				if (vt==5){
					return ;
				}
				if(t == 5)
				{
					return;
				}
				if(vt != t)
				{
					saerror = 1;
					printf( "Line No.%d : Type mismatch for ID '%s' .Expected %s got result as %s.\n",ast->tk.lineno,ast->tk.lexeme, types1[vt-1],types1[t-1] );
					return;
				}
			}
		}
	}
	else if(ast->ruleNode->type==1){
		if (strcmp(ast->ruleNode->name,"<leftHandSide_listVar>")==0)
		{

			if(strcmp(ast->parent->child[1]->ruleNode->name,"<rightHandSide_type2>")==0)
			{
				function f = createFunction("RHSType2");
				if(outputType(ast->child[0],&f)==false){
					ast->type=5;
					return false;
				}
				f.linedec = ast->child[0]->child[0]->tk.lineno;
				bool a = checkRightSideType2(ast->parent->child[1]->child[0],&f);
				return;
			}
		}
		else if(strcmp(ast->ruleNode->name,"<relationalOp>")==0){
			bool cro = checkRelationalOp(ast);
		}
		else if(strcmp(ast->ruleNode->name,"<ioStmt>")==0){
			if(checkIOStmt(ast)){
				variablenodeptr temp = getID(ast->child[1]->tk.lexeme , ast->child[1]->st);
				markAssigned(temp , ast->child[1]->st);
			}
		}
	}
	else if(ast->ruleNode->type==0){
		if ((ast->tk.type == END) && (strcmp(ast->parent->ruleNode->name,"<functionDef>")==0))
		{
			variablenodeptr out_ptr = ast->parent->child[1]->st->f.outputList;
			while(out_ptr != NULL)
			{
				variablenodeptr t = getID(out_ptr->v.name,ast->parent->child[1]->st);
				if(t != NULL && !t->v.assigned)
				{
					printf( "Line No.%d : Output parameter '%s' of function '%s' is not defined in the function.\n",ast->tk.lineno, out_ptr->v.name,ast->parent->child[1]->st->f.name );
					saerror = 1;
				}
				out_ptr = out_ptr->next;
			}
		}
		else if(ast->tk.type == NUM)
			ast->type = 1;
		else if(ast->tk.type == RNUM)
			ast->type = 2;
		else if(ast->tk.type == STR)
			ast->type = 3;
	}

	int i;
	for(i = 0 ; i < ast->noc ; i++)
		semanticAnalysis(ast->child[i]);
}

/*
Semantic Analysis Utility
*/
bool semanticAnalysisUtil(AStree ast){
	semanticAnalysis(ast);
	if(saerror==1){
		return false;
	}
	return true;
}