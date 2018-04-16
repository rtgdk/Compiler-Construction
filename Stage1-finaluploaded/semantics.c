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


/*Returns type of vraiable from symbol table
*/

int getTypeID(char* name , SymbolTablePtr b, int lineno)
{	
	if(b == NULL)
	{
		return 5; //NOT Defined
	}

	int pos = hash(name)%VariableTableSize;

	if(b->VariableTable[pos] == NULL)
	{
		return getTypeID(name , b->parent, lineno);
	}
	else
	{
		variablenodeptr temp = b->VariableTable[pos];

		while(temp != NULL)
		{
			if(strcmp(temp->v.name , name) == 0 && )
			{
				if(temp->v.linedecdec <= lineno)
					return (temp->v.type);
				else
					printf("ERROR: Line No %d : Variable %s not declared.",lineno,name);
			}
			temp = temp->next;
		}
		return getTypeID(name , b->parent, lineno);
	}
}

variablenodeptr getID(char* name , SymbolTablePtr b) // return variable nodeptr
{	
	if(b == NULL)
		return NULL;

	int pos = hash(name)%VariableTableSize;

	if(b->VariableTable[pos] == NULL)
	{
		return getID(name , b->parent);
	}
	else
	{
		variablenodeptr temp = b->VariableTable[pos];

		while(temp != NULL)
		{
			if(strcmp(temp->v.name , name) == 0)
			{
				return temp;
			}
			temp = temp->next;
		}
		return getID(name , b->parent);
	}
}

void markAssigned(variablenodeptr v , SymbolTablePtr b) // mark for assigned
{
	int pos = hash(v->v.name)%VariableTableSize;

	if(b->VariableTable[pos] == NULL)
	{
		markAssigned(v , b->parent);
	}
	else
	{
		variablenodeptr temp = b->VariableTable[pos];

		while(temp != NULL)
		{
			if(strcmp(temp->v.name , v->v.name) == 0)
			{
				temp->v.assigned = true;
				//printf("%s defined\n" , temp->v.name);
				return;
			}
			temp = temp->next;
		}
		return markAssigned(v , b->parent);
	}
}

function checkFunction(char* fname, SymbolTablePtr b, int lineno) //Check if function recursion and defined after use
// If found return the function
{
	SymbolTablePtr temp = b; 
	int i;

	while(temp->parent!=NULL){
		if(strcmp(temp->parent->f.name,fname)){
			printf( "ERROR : Recursion call to function %s in line %d not permitted\n" , fname, lineno );
			function f;
			f.noOfInput = -1;
			f.noOfOutput = -1;
			return f;
		}
		else{
			found = false;
			for(i = 0 ; i < temp->noc ; i++)
			{
				if(strcmp(temp->child[i]->f.name , fname) == 0)
				{
					if(lineno >= temp->child[i]->f.linedec)
					{
						return temp->child[i]->f;
					}
					else
					{
						printf( "ERROR : Function %s called on line %d not declared\n" , fname, lineno );
						function f;
						f.noOfInput = -1;
						f.noOfOutput = -1;
						return f;
					}
					
				}	
			}	
		}
		temp = temp->parent;
	}
	printf( "ERROR : Function %s called on line %d not declared\n" , fname, lineno );
	function f;
	f.noOfInput = -1;
	f.noOfOutput = -1;
	return f;	
}

bool outputType(astptr root , function *f)
{
	int i;

	if(root->ruleNode->type==0 && root->tk.type == ID)
	{
		variablenodeptr vptr = (variablenodeptr)malloc(sizeof(variablenode));
		vptr->v.type = getTypeID(root->tk.lexeme , root->st, root->tk.lineno); 
		f->noOfOutput++;
		
		if(vptr->v.type == 5)
		{
			printf( "ERROR : No such variable %s in line %d\n" , root->tk.lexeme , root->tk.lineno );
			//err = true;
			return false;
		}

		strcpy(vptr->v.name, root->tk.lexeme);
		vptr->v.linedec = root->tk.lineno;
		// if(vptr->v.type.isarr)
		// {
		// 	printf( "ERROR : Array %s cannot be returned in call to function at line %d\n",vptr->v.name,vptr->v.linedec );
		// 	err = true;
		// 	return false;
		// }

		vptr->next = NULL;

		if(f->outputType == NULL)
			f->outputType = vptr;
		else
		{
			variablenodeptr temp = f->outputType;
			while(temp->next != NULL)
			{
				temp = temp->next;
			}
			temp->next = vptr;
		}		
	}

	for(i = 0 ; i < root->noc ; i++)
	{
		bool res = outputType(root->child[i] , f);
		if(!res)
			return false;
	}
	return true;
}

void inputType(astptr root , function *f)
{
	int i;

	if(root->ruleNode->type==0 && root->tk.type == ID)
	{
		int ty = getTypeID(root->tk.lexeme,root->st,root->tk.lineno);

		if(ty.type == 5)
		{
			printf( "ERROR : No such variable %s in line %d\n" , root->tk.lexeme , root->tk.lineno );
			//err = true;
			return;
		}
		
		// if(!ty.isarr)
		// {
			f->noOfInput++;
			variablenodeptr vptr = (variablenodeptr)malloc(sizeof(variablenode));
			strcpy(vptr->v.name, root->tk.lexeme);
			vptr->v.linedec = root->tk.lineno;
			vptr->v.type = getTypeID(root->tk.lexeme , root->st, root->tk.lineno);
			vptr->v.scope = 0;
			vptr->v.offset = 0; 
			vptr->next = NULL;

			if(f->inputType == NULL)
				f->inputType = vptr;
			else
			{
				variablenodeptr temp = f->inputType;
				while(temp->next != NULL)
				{
					temp = temp->next;
				}
				temp->next = vptr;
			}
		//}
		//for matrix down below
		// else
		// {
		// 	int j;
		// 	for(j = ty.low ; j <= ty.high ; j++)
		// 	{
		// 		f->noOfInput++;

		// 		variablenodeptr vptr = (variablenodeptr)malloc(sizeof(variablenode));
		// 		strcpy(vptr->v.name, root->tk.lexeme);
		// 		vptr->v.linedec = root->tk.lineno;
		// 		vptr->v.type = getTypeID(root->tk.lexeme , root->st, root->tk.lineno);
		// 		vptr->v.scope = 0; 
		// 		vptr->v.offset = j;
		// 		vptr->next = NULL;

		// 		if(f->inputType == NULL)
		// 			f->inputType = vptr;
		// 		else
		// 		{
		// 			variablenodeptr temp = f->inputType;
		// 			while(temp->next != NULL)
		// 			{
		// 				temp = temp->next;
		// 			}
		// 			temp->next = vptr;
		// 		}
		// 	}
		// }		
	}

	for(i = 0 ; i < root->noc ; i++)
		inputType(root->child[i] , f);
}

bool compatibleFunction(function f1, function f2, int line) //check if defined and called fn are type checked
{
	char * char_type[] = {"INTEGER" , "REAL" , "STRNG","MATRIX"};	

	if(f1.noOfInput != f2.noOfInput)
	{
		printf( "ERROR : Count of input variables passed to the call of function %s do not match on line %d\n" , f2.name , line );
		//err = true;
		return false;
	}

	if(f1.noOfOutput != f2.noOfOutput)
	{
		printf( "ERROR : Count of output variables returned from call to function %s do not match on line %d\n" , f2.name , line );
		//err = true;
		return false;
	}

	variablenodeptr tempi1 = f1.inputType;
	variablenodeptr tempo1 = f1.outputType;
	variablenodeptr tempi2 = f2.inputType;
	variablenodeptr tempo2 = f2.outputType;
	int arg_no = 1;
	while(tempi1 != NULL)
	{
		if(tempi1->v.type != tempi2->v.type)
		{
			printf( "ERROR : Input argument %d of function %s at line %d: Expected type %s but found type %s\n",arg_no,f2.name,line,char_type[(int)tempi2->v.type],char_type[(int)tempi1->v.type] );
			//err = true;
			return false;
		}
		arg_no++;
		tempi1 = tempi1->next;
		tempi2 = tempi2->next;
	}

	arg_no = 1;
	while(tempo1 != NULL)
	{
		if(tempo1->v.type != tempo2->v.type)
		{
			printf( "ERROR : Output argument %d of function %s at line %d : Expected type %s but found type %s\n",arg_no,f2.name,line,char_type[(int)tempo2->v.type],char_type[(int)tempo1->v.type] );
			//err = true;
			return false;
		}
		arg_no++;
		tempo1 = tempo1->next;
		tempo2 = tempo2->next;
	}


	return true;
}

//op is operator_low/high precendence
bool checkOperatorType(AStree op, AStree ex1, AStree ex2){
	if(op->child[0]->tk.type == PLUS){
		// check for strings and matrix ???
		if(ex1->type==ex2->type) return true;
		
	}
	else if(op->child[0]->tk.type == MINUS){
		if(ex1->type==ex2->type){
			if (ex1->type==1 || ex1->type==2 || ex1->type==4) return true;
			
		}
		
	}
	else if(op->child[0]->tk.type == MUL){
		if(ex1->type==ex2->type){
			if (ex1->type==1 || ex1->type==2) return true;
			
		}
	}
	else if(op->child[0]->tk.type == DIV){
		if(ex1->type==ex2->type){
			if (ex1->type==1 || ex1->type==2 ) return true;
			
		}
	}
	return false;
}

void getASTtype(AStree ast) // type checking in expression
{
	// if(ast->ruleNode->type==0)
	//  	printf("%s %d\n",ast->tk.lexeme,ast->tk.lineno);
	// else
	//  	printf("%s\n",str_characters[(int)ast->name.c]);
	char * char_type[] = {"INTEGER" , "REAL" , "STRNG","MATRIX"};	
	if(strcmp(ast->name,"<rightHandSide_type1>")==0)
	{
		getASTtype(ast->child[0]);
		ast->type = ast->child[0]->type;
		return;
	}
	else if(strcmp(ast->name,"<arithmeticExpression>")==0)
	{
		getASTtype(ast->child[0]);
		if(ast->child[0]->type == 5){
			ast->type = 5;
			return;
		}
		if(ast->child[1]->noc==0){
			ast->type = ast->child[0]->type;
			return;
		}
		getASTtype(ast->child[1]);
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
	else if(strcmp(ast->name,"<arithmeticTerm>")==0)
	{
		getASTtype(ast->child[0]);
		if(ast->child[0]->type == 5){
			ast->type = 5;
			return;
		}
		if(ast->child[1]->noc==0){
			ast->type = ast->child[0]->type;
			return;
		}
		getASTtype(ast->child[1]);
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
	else if(strcmp(ast->name,"<arithmeticFactor>")==0)
	{
		getASTtype(ast->child[0]);
		ast->type = ast->child[0]->type;
		return;
	}
	else if(strcmp(ast->name,"<moreTerms_two>")==0)
	{
		getASTtype(ast->child[1]);
		if(ast->child[1]->type==5){
			ast->type = 5;
			return
		}
		if(ast->child[2]->noc==0){ // epsilon
			ast->type = ast->child[1]->type
			return;
		}
		else{
			getASTtype(ast->child[2]);
			if(ast->child[2]->type==5){
				ast->type = 5;
				return
			}
			if(checkOperatorType(ast->child[2]->child[0],ast->child[1],ast->child[2]->child[1])){
				ast->type = ast->child[1]->type;
				return;
			}
		}
	}
	
	else if(strcmp(ast->name,"<moreTerms_one>")==0){
		getASTtype(ast->child[1]);
		if(ast->child[1]->type==5){
			ast->type = 5;
			return
		}
		if(ast->child[2]->noc==0){ // epsilon
			ast->type = ast->child[1]->type
			return;
		}
		else{
			getASTtype(ast->child[2]);
			if(ast->child[2]->type==5){
				ast->type = 5;
				return
			}
			if(checkOperatorType(ast->child[2]->child[0],ast->child[1],ast->child[2]->child[1])){
				ast->type = ast->child[1]->type;
				return;
			}
		}
		return;
	}

	else if(strcmp(ast->name,"<sizeExpression>")==0)
	{
		getASTtype(ast->child[0]); // if not considering SIZE as node
		if (ast->child[0]->type==3){
			ast->type = 1;
		}
		else(ast->child[0]->type=4){
			ast->type = 6 //int x int // but it should show error
		}

	}
	else if(strcmp(ast->name,"<funCallStmt>")==0)
	{

	}
	
	else if(strcmp(ast->name,"<var>")==0){

		if(ast->child[0]->tk.type == NUM)
		{
			ast->type = 1;
			//printf("int %s\n",ast->child[0]->tk.lexeme);
		}
		else if(ast->child[0]->tk.type == RNUM)
		{
			ast->type = 2;
			//printf("real\n");
		}
		else if(ast->child[0]->tk.type == STR)
		{
			ast->type = 3;
			//printf("real\n");
		}
		else if(strcmp(ast->child[0]->name,"<matrix>"))
		{
			// FUnction call for matrix type check;
			ast->type = 4;

			//printf("real\n");
		}
		else if(ast->child[0]->tk.type == ID)
		{
			// fucntion check for matrix element
			int vt = getTypeID(ast->child[0]->tk.lexeme,ast->child[0]->scope,ast->child[0]->tk.linenono);
			if(vt == 5)
			{
				ast->type = 5;
				printf( "ERROR : No such variable %s in line %d\n" , ast->child[0]->tk.lexeme , ast->child[0]->tk.lineno );
				//err = true;
			}
			else
				ast->type = vt;
		}	
	}
}

// CHECK FOR IF ELSE BLOCK
void semanticAnalysis(astptr root)
{
	// if(root->ruleNode->type==0)
	// 	printf("%s %d\n",root->tk.lexeme,root->tk.lineno);
	// else
	// 	printf("%s\n",str_characters[(int)root->name.c]);
	char * char_type[] = {"INTEGER" , "REAL" , "STRING","MATRIX"};

	if(root->ruleNode->type==0 && root->tk.type == ID)// && root->parent->name.c != module && root->parent->name.c != moduleReuseStmt && root->parent->name.c != moduleDeclaration)
	{
		// int vt = getTypeID(root->tk.lexeme , root->st, root->tk.lineno);
		
		// if(vt == 5)
		// {
		// 	printf("ERROR --: No such variable %s in line %d\n" , root->tk.lexeme , root->tk.lineno);
		// 	return;
		// }
		int vt = getTypeID(root->tk.lexeme , root->st, root->tk.lineno);
		root->type = vt;

		if(strcmp(root->parent->name,"<leftHandSide_singleVar>")
		{
		
			if(vt == 5)
			{
				printf( "ERROR : No such variable %s in line %d\n" , root->tk.lexeme , root->tk.lineno );
				//err = true;
			}

			if(strcmp(root->parent->parent->child[1]->name,"<rightHandSide_type1>"))
			{
				getASTtype(root->parent->parent->child[1]);
				typ t = root->parent->parent->child[1]->type;
				if(t == 5)
				{
					printf( "ERROR : Expression on right side at line %d is not well formed\n",root->tk.lineno );
					//err = true;
					return;
				}
				if(vt != t)
				{
					printf( "ERROR : Type mismatch for ID %s at line %d\n",root->tk.lexeme,root->tk.lineno );
					err = true;
					return;
				}
			}
			//Code for matrix assignment
			// else
			// {	
			// 	getASTtype(root->parent->child[1]->child[0]->child[1]);
			// 	typ t = root->parent->child[1]->child[0]->child[1]->type;
			// 	if(t == 5)
			// 	{
			// 		printf( "ERROR : Expression on right side at line %d is not well formed\n",root->tk.lineno );
			// 		err = true;
			// 		return;
			// 	}
			// 	if(vt != t)
			// 	{
			// 		printf( "ERROR : Type mismatch for ID %s at line %d\n",root->tk.lexeme,root->tk.lineno );
			// 		err = true;
			// 		return;
			// 	}
			// }

			variablenodeptr vpt = getID(root->tk.lexeme , root->st);
			markAssigned(vpt , root->st);
		}
		else
		{
			if(vt == 5)
			{
				printf( "ERROR : No such variable %s in line %d\n" , root->tk.lexeme , root->tk.lineno );
				//err = true;
				return;
			}
		}	
	}
	else if(root->ruleNode->type==0 && root->tk.type == FUNID && strcmp(root->parent->name,"<funCallStmt>"))
	{

		function fn = checkFunction(root->tk.lexeme , root->st, root->tk.lineno);
		if(fn.noOfInput == -1)
			return;
		function f;
		f.noOfOutput = 0;
		f.outputType = NULL;
		f.noOfInput = 0;
		f.inputType = NULL;
		bool out_res;
		out_res = outputType(root->parent->child[0]->child[0] , &f);
		// if(!out_res)
		//  	return;
	

		// if(root->parent->child[2]->child[0]->name.c == eps)
		// {
		// 	f.noOfInput = 0;
		// 	f.inputType = NULL;
		// }
		// else
		inputType(root->parent->child[2] , &f);

		bool compat = compatibleFunction(f , fn, root->tk.lineno);

		// if(!compat)
		// 	return;

		//variablenodeptr vpt = getID(root->tk.lexeme , root->st);
		variablenodeptr vpt = f.outputType;

		while(vpt != NULL)
		{
			markAssigned(vpt , root->st);
			vpt = vpt->next;
		}

		// function func = f; 
		// variablenodeptr tempi = func.inputType;

		// printf("INPUT : %d\n",func.noOfInput);
		// while(tempi != NULL)
		// {
		// 	printf("%s %s %d %d %d\n" , tempi->v.name , char_type[(int)tempi->v.type] , tempi->v.linedec , tempi->v.scope, tempi->v.offset);
		// 	tempi = tempi->next;
		// }
		// variablenodeptr tempo = func.outputType;

		// printf("OUTPUT : %d\n",func.noOfOutput);
		// while(tempo != NULL)
		// {
		// 	printf("%s %s %d %d %d\n" , tempo->v.name , char_type[(int)tempo->v.type] , tempo->v.linedec , tempo->v.scope, tempo->v.offset);
		// 	tempo = tempo->next;
		// }

	}
	else if(root->ruleNode->type==0 && (root->tk.type == END) && (strcmp(root->parent->name,"<functionDef>")))
	{
		function f = root->st->f;
		variablenodeptr out_ptr = f.outputType;
		while(out_ptr != NULL)
		{
			variablenodeptr t = getID(out_ptr->v.name,root->st);
			if(t != NULL && !t->v.assigned)
			{
				printf( "ERROR : Output parameter %s of function %s is not defined\n",out_ptr->v.name,f.name );
				err = true;
			}
			out_ptr = out_ptr->next;
		}
	}
	// CHECK FOR IF ELSE BLOCK


	// else if(root->ruleNode->type==0 && (root->tk.type == START) && (root->parent->name.c == iterativeStmt))
	// {
	// 	if(root->parent->child[0]->tk.type == FOR)
	// 	{
	// 		insidefor = true;
	// 		strcpy(forvar , root->parent->child[1]->tk.lexeme);
	// 	}
	// 	else
	// 	{
	// 		getASTtype(root->parent->child[1]);
	// 		if(root->parent->child[1]->type != BOOL)
	// 		{
	// 			printf( "ERROR : Conditional expression of while loop in line %d must be of boolean type\n",root->parent->child[0]->tk.lineno );
	// 			err = true;
	// 		}
	// 	}		
	// }
	// else if(root->ruleNode->type==0 && (root->tk.type == START) && (root->parent->name.c == conditionalStmt))
	// {
	// 	insideswitch = true;
	// 	int vt = getTypeID(root->parent->child[0]->tk.lexeme,root->st,root->parent->child[0]->tk.lineno);
	// 	if(vt == 5)
	// 	{
	// 		printf( "ERROR : No such variable %s in line %d\n" , root->parent->child[0]->tk.lexeme , root->parent->child[0]->tk.lineno );
	// 		err = true;
	// 		return;
	// 	}
	// 	sw_type = vt;
	// 	if(sw_type == RE)
	// 	{
	// 		printf( "ERROR : Switch case variable cannot be real at line %d\n",root->parent->child[0]->tk.lineno );
	// 		err = true;
	// 	}
	// 	else if(vt.isarr)
	// 	{
	// 		printf( "ERROR : Switch case variable %s cannot be of array type at line %d\n",root->parent->child[0]->tk.lexeme,root->parent->child[0]->tk.lineno );
	// 		err = true;
	// 	}
	// 	else if(sw_type == INT)
	// 	{
	// 		if(root->parent->child[3]->child[0]->tk.type == eps)
	// 		{
	// 			printf( "ERROR : Default case statement required in switch statement starting from line %d\n",root->parent->child[0]->tk.lineno );
	// 			err = true;
	// 		}	
	// 	}
	// 	else if(sw_type == BOOL)
	// 	{
	// 		if(root->parent->child[3]->child[0]->tk.type != eps)
	// 		{
	// 			printf( "ERROR : Default case statement not allowed in switch statement starting from line %d\n",root->parent->child[0]->tk.lineno );	
	// 			err = true;
	// 		}
	// 	}
	// }

	// else if(!root->ruleNode->type==0 && strcmp(root->name, "value"))
	// {
	// 	if(sw_type == INT && root->child[0]->tk.type != NUM)
	// 	{
	// 		printf( "ERROR : Expected integer case label at line %d\n",root->child[0]->tk.lineno );
	// 		err = true;
	// 	}	
	// 	else if(sw_type == BOOL && root->child[0]->tk.type == NUM)
	// 	{
	// 		printf( "ERROR : Expected boolean case label at line %d\n",root->child[0]->tk.lineno );
	// 		err = true;
	// 	}	
	// }

	// else if(root->ruleNode->type==0 && (root->tk.type == END) && (root->parent->name.c == iterativeStmt))
	// {
	// 	insidefor = false;
	// }
	// else if(root->ruleNode->type==0 && (root->tk.type == END) && (root->parent->name.c == conditionalStmt))
	// {
	// 	insideswitch = false;
	// }
	else if(root->ruleNode->type==0 && root->tk.type == NUM)
		root->type = 1;
	else if(root->ruleNode->type==0 && root->tk.type == RNUM)
		root->type = 2;


	int i;
	for(i = 0 ; i < root->noc ; i++)
		semanticAnalysis(root->child[i]);
}