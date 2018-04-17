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
	
	if(b == NULL )
	{
		//printf("Returning\n");
		return 5; //NOT Defined
	}
	//printf("get typeIDDDD  %s-%d-%s\n ",name,lineno,b->f.name);
	int pos = hash4(name)%VariableTableSize;
	//printf("pos %d\n",pos);

	if(b->VariableTable[pos] == NULL)
	{
		return getTypeID(name , b->parent, lineno);
	}
	else
	{
		//printf("get typeIDDDD\n");
		variablenodeptr temp = b->VariableTable[pos];

		while(temp != NULL)
		{
			//printf("%s-%d--%s-%d\n",temp->v.name,temp->v.linedec, name,lineno);
			//printf("kuch bi\n");
			//printf("%s\n");
			if(strcmp(temp->v.name , name) == 0 ) //anthing else??
			{
				if(temp->v.linedec <= lineno){
					//printf("Here-returning\n");
					return (temp->v.type);
				}
				else
					printf("Line No.%d : Variable %s not declared. yet. Declartion found later in line %d. Variable cannot be used before declaration.\n ",lineno,name,temp->v.linedec);
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

	int pos = hash4(name)%VariableTableSize;

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
	int pos = hash4(v->v.name)%VariableTableSize;

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
		//printf("F %s\n",temp->f.name);
		if(strcmp(temp->parent->f.name,fname)==0){
			printf( "Line No.%d : Recursion call to function %s not permitted\n" , lineno, fname );
			function f;
			f.noOfInput = -1;
			f.noOfOutput = -1;
			return f;
		}
		else{
			//printf("ine lse\n");
			for(i = 0 ; i < temp->noc ; i++)
			{
				//printf("ine lse %d\n",i);
				if(strcmp(temp->child[i]->f.name , fname) == 0)
				{
					//printf("ine lse %d--%d\n",temp->child[i]->f.linedec,lineno);
					if(lineno >= temp->child[i]->f.linedec)
					{
						return temp->child[i]->f;
					}
					else
					{
						printf( "Line No.%d : Function %s not declared yet. Defination found later in line %d. Function cannot be called before defination.\n" , lineno, fname ,temp->child[i]->f.linedec);
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
	printf( "Line No.%d : Function %s not declared.\n" , lineno, fname );
	function f;
	f.noOfInput = -1;
	f.noOfOutput = -1;
	return f;	
}

bool outputType(AStree root , function *f)
{
	int i;

	if(root->ruleNode->type==0 && root->tk.type == ID)
	{
		variablenodeptr vptr = (variablenodeptr)malloc(sizeof(variablenode));
		vptr->v.type = getTypeID(root->tk.lexeme , root->st, root->tk.lineno); 
		f->noOfOutput++;
		
		if(vptr->v.type == 5)
		{
			printf( "Line No.%d : Variable1 %s not declared.\n" , root->tk.lineno, root->tk.lexeme );
			//err = true;
			return false;
		}

		strcpy(vptr->v.name, root->tk.lexeme);
		vptr->v.linedec = root->tk.lineno;
		// if(vptr->v.type.isarr)
		// {
		// 	printf( "Line No.%d : Array %s cannot be returned in call to function at line %d\n",vptr->v.name,vptr->v.linedec );
		// 	err = true;
		// 	return false;
		// }

		vptr->next = NULL;

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

	for(i = 0 ; i < root->noc ; i++)
	{
		bool res = outputType(root->child[i] , f);
		if(!res)
			return false;
	}
	return true;
}

void inputType(AStree root , function *f)
{
	int i;
	//printf("onpit %s\n",root->ruleNode->name);
	if(root->ruleNode->type==0 && root->tk.type == ID)
	{
		int ty = getTypeID(root->tk.lexeme,root->st,root->tk.lineno);

		if(ty == 5)
		{
			printf( "Line No.%d : Variable2 %s not declared.\n" , root->tk.lineno, root->tk.lexeme );
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
			vptr->v.scopeDepth = 0;
			vptr->v.offset = 0; 
			vptr->next = NULL;

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
		// 		vptr->v.scopeDepth = 0; 
		// 		vptr->v.offset = j;
		// 		vptr->next = NULL;

		// 		if(f->inputList == NULL)
		// 			f->inputList = vptr;
		// 		else
		// 		{
		// 			variablenodeptr temp = f->inputList;
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
		printf( "Line No.%d  : Count of input variables passed to the call of function %s do not match\n" , line, f2.name );
		//err = true;
		return false;
	}

	if(f1.noOfOutput != f2.noOfOutput)
	{
		printf( "Line No.%d  : Count of output variables passed to the call of function %s do not match\n" , line, f2.name );
		//err = true;
		return false;
	}

	variablenodeptr tempi1 = f1.inputList;
	variablenodeptr tempo1 = f1.outputList;
	variablenodeptr tempi2 = f2.inputList;
	variablenodeptr tempo2 = f2.outputList;
	int arg_no = 1;
	while(tempi1 != NULL)
	{
		if(tempi1->v.type != tempi2->v.type)
		{
			printf( "Line No.%d : Mismatch in input argument %d of function %s : Expected type %s but found type %s\n",line,arg_no,f2.name,char_type[tempi2->v.type-1],char_type[tempi1->v.type-1] );
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
			printf( "Line No.%d : Mismatch in input argument %d of function %s : Expected type %s but found type %s\n",line,arg_no,f2.name,char_type[tempo2->v.type-1],char_type[tempo1->v.type-1] );
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
	//  	printf("%s\n",str_characters[(int)ast->ruleNode->name.c]);
	//printf("getASTtype----%s\n",ast->ruleNode->name);
	char * char_type[] = {"INTEGER" , "REAL" , "STRNG","MATRIX"};	
	if(strcmp(ast->ruleNode->name,"<rightHandSide_type1>")==0)
	{
		//printf("got type4 %s\n",ast->tk.lexeme);
		getASTtype(ast->child[0]);
		//printf("got type4 %s\n",ast->tk.lexeme);
		ast->type = ast->child[0]->type;
		return;
	}
	else if(strcmp(ast->ruleNode->name,"<arithmeticExpression>")==0)
	{
		getASTtype(ast->child[0]);
		if(ast->child[0]->type == 5){
			ast->type = 5;
			return;
		}
		if(ast->child[1]==NULL || ast->child[1]->noc==0){
			ast->type = ast->child[0]->type;
			return;
		}
		//printf("Here in AE1/2\n");
		getASTtype(ast->child[1]);
		if(ast->child[1]->type == 5){
			ast->type = 5;
			return;
		}
		if(checkOperatorType(ast->child[1]->child[0],ast->child[0],ast->child[1]->child[1])){
			ast->type = ast->child[0]->type;
			//printf("Here in AE\n");
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
		getASTtype(ast->child[0]);
		if(ast->child[0]->type == 5){
			ast->type = 5;
			return;
		}
		//printf("Here in AT1 without chuil1d\n");
		if(ast->child[1]==NULL || ast->child[1]->noc==0){
			//printf("Here in AT1 without chuild\n");
			ast->type = ast->child[0]->type;
			return;
		}
		//printf("Here in AT1/2\n");
		getASTtype(ast->child[1]);
		if(ast->child[1]->type == 5){
			ast->type = 5;
			return;
		}
		if(checkOperatorType(ast->child[1]->child[0],ast->child[0],ast->child[1]->child[1])){
			ast->type = ast->child[0]->type;
			////printf("Here in AT\n");
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
		getASTtype(ast->child[0]);

		ast->type = ast->child[0]->type;
		//printf("Here in AF %d\n",ast->type);
		return;
	}
	else if(strcmp(ast->ruleNode->name,"<moreTerms_two>")==0)
	{
		getASTtype(ast->child[1]);
		if(ast->child[1]->type==5){
			ast->type = 5;
			return;
		}
		if(ast->child[2]==NULL || ast->child[2]->noc==0){ // epsilon
			ast->type = ast->child[1]->type;
			return;
		}
		else{
			getASTtype(ast->child[2]);
			if(ast->child[2]->type==5){
				ast->type = 5;
				return;
			}
			if(checkOperatorType(ast->child[2]->child[0],ast->child[1],ast->child[2]->child[1])){
				ast->type = ast->child[1]->type;
				return;
			}
		}
	}
	
	else if(strcmp(ast->ruleNode->name,"<moreTerms_one>")==0){
		getASTtype(ast->child[1]);
		if(ast->child[1]->type==5){
			ast->type = 5;
			return;
		}
		if(ast->child[2]==NULL || ast->child[2]->noc==0){ // epsilon
			ast->type = ast->child[1]->type;
			return;
		}
		else{
			getASTtype(ast->child[2]);
			if(ast->child[2]->type==5){
				ast->type = 5;
				return;
			}
			if(checkOperatorType(ast->child[2]->child[0],ast->child[1],ast->child[2]->child[1])){
				ast->type = ast->child[1]->type;
				return;
			}
		}
		return;
	}

	else if(strcmp(ast->ruleNode->name,"<sizeExpression>")==0)
	{
		getASTtype(ast->child[0]); // if not considering SIZE as node
		if (ast->child[0]->type==3){
			ast->type = 1;
		}
		else if (ast->child[0]->type==4){
			ast->type = 6; //int x int // but it should show Line No.%d
		}

	}
	else if(strcmp(ast->ruleNode->name,"<funCallStmt>")==0)
	{

	}
	
	else if(strcmp(ast->ruleNode->name,"<var>")==0){

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
		else if(strcmp(ast->child[0]->ruleNode->name,"<matrix>")==0)
		{
			// FUnction call for matrix type check;
			ast->type = 4;

			//printf("real\n");
		}
		else if(ast->child[0]->tk.type == ID)
		{
			// fucntion check for matrix element
			//printf("Here in ID check %s -%s\n",ast->child[0]->tk.lexeme,ast->child[0]->st->f.name);
			int vt = getTypeID(ast->child[0]->tk.lexeme,ast->child[0]->st,ast->child[0]->tk.lineno);
			//printf("Here in ID check again%s -%s\n",ast->child[0]->tk.lexeme,ast->child[0]->st->f.name);
			if(vt == 5)
			{
				ast->type = 5;
				//printf("In Vt 5");
				printf( "Line No.%d : Variable3 %s not declared\n" ,  ast->child[0]->tk.lineno,ast->child[0]->tk.lexeme );
				return;
				//err = true;
			}
			else{
				//printf("Here-returning-getAST\n");
				ast->type = vt;
				return;
			}
		}
		return;
	}
	
}

// CHECK FOR IF ELSE BLOCK
void semanticAnalysis(AStree root)
{
	// if(root->ruleNode->type==0)
	// 	printf("%s %d\n",root->tk.lexeme,root->tk.lineno);
	// else
	// 	printf("%s\n",str_characters[(int)root->name.c]);
	//printf("----%s\n",root->ruleNode->name);
	char * char_type[] = {"INTEGER" , "REAL" , "STRING","MATRIX"};

	if(root->ruleNode->type==0 && root->tk.type == ID)// && root->parent->name.c != module && root->parent->name.c != moduleReuseStmt && root->parent->name.c != moduleDeclaration)
	{
		// int vt = getTypeID(root->tk.lexeme , root->st, root->tk.lineno);
		
		// if(vt == 5)
		// {
		// 	printf("Line No.%d --: No such variable %s in line %d\n" , root->tk.lexeme , root->tk.lineno);
		// 	return;
		// }
		//printf("got type\n");
		if(strcmp(root->parent->ruleNode->name,"<leftHandSide_singleVar>")==0)
		{
			int vt = getTypeID(root->tk.lexeme , root->st, root->tk.lineno);
			root->type = vt;
			if(vt == 5)
			{
				printf( "Line No.%d : Variable4 %s not declared.\n" ,root->tk.lineno, root->tk.lexeme );
				return; //added by me
				//err = true;
			}
			//printf("got type2\n");
			if(strcmp(root->parent->parent->child[1]->ruleNode->name,"<rightHandSide_type1>")==0)
			{
				//printf("got type2 %s\n",root->tk.lexeme);
				getASTtype(root->parent->parent->child[1]);
				//printf("got type2\n");
				//printf("got type2 %s\n",root->tk.lexeme);
				int t = root->parent->parent->child[1]->type;
				if(t == 5)
				{
					printf( "Line No.%d : Expression on right side is not well formed, expected expression result as %s\n",root->tk.lineno,char_type[vt-1]);
					//err = true;
					return;
				}
				if(vt != t)
				{
					printf( "Line No.%d : Type mismatch for ID %s.Expected %s got %s.\n",root->tk.lineno,root->tk.lexeme, char_type[vt-1],char_type[t-1] );
					//err = true;
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
			// 		printf( "Line No.%d : Expression on right side at line %d is not well formed\n",root->tk.lineno );
			// 		err = true;
			// 		return;
			// 	}
			// 	if(vt != t)
			// 	{
			// 		printf( "Line No.%d : Type mismatch for ID %s at line %d\n",root->tk.lexeme,root->tk.lineno );
			// 		err = true;
			// 		return;
			// 	}
			// }
			//printf("got type3\n");
			variablenodeptr vpt = getID(root->tk.lexeme , root->st);
			markAssigned(vpt , root->st);
		}
		// else
		// {
		// 	if(vt == 5)
		// 	{
		// 		printf( "Line No.%d : No such variable5 %s in line %d\n" , root->tk.lexeme , root->tk.lineno );
		// 		//err = true;
		// 		return;
		// 	}
		// }	
	}
	else if(root->ruleNode->type==0 && root->tk.type == FUNID && strcmp(root->parent->ruleNode->name,"<funCallStmt>")==0)
	{

		function fn = checkFunction(root->tk.lexeme , root->st, root->tk.lineno);
		//printf("Function check successfull\n");
		if(fn.noOfInput == -1)
			return;
		function f;
		f.noOfOutput = 0;
		f.outputList = NULL;
		f.noOfInput = 0;
		f.inputList = NULL;
		bool out_res;
		out_res = outputType(root->parent->parent->parent->child[0], &f);
		if (out_res==false){
			printf("Line No. %d: Type Mismatch for Output Type of Function call %s\n",root->tk.lineno, root->tk.lexeme);
		}
		//printf("Function check successfull\n");
		// if(!out_res)
		//  	return;
	

		// if(root->parent->child[2]->child[0]->name.c == eps)
		// {
		// 	f.noOfInput = 0;
		// 	f.inputList = NULL;
		// }
		// else
		//printf("Function check successfull2\n");
		inputType(root->parent->child[1] , &f);
		//printf("Function check successfull\n");

		bool compat = compatibleFunction(f , fn, root->tk.lineno);
		//printf("Function check successfull3\n");

		// if(!compat)
		// 	return;

		//variablenodeptr vpt = getID(root->tk.lexeme , root->st);
		variablenodeptr vpt = f.outputList;

		while(vpt != NULL)
		{
			markAssigned(vpt , root->st);
			vpt = vpt->next;
		}

		// function func = f; 
		// variablenodeptr tempi = func.inputList;

		// printf("INPUT : %d\n",func.noOfInput);
		// while(tempi != NULL)
		// {
		// 	printf("%s %s %d %d %d\n" , tempi->v.name , char_type[(int)tempi->v.type] , tempi->v.linedec , tempi->v.scopeDepth, tempi->v.offset);
		// 	tempi = tempi->next;
		// }
		// variablenodeptr tempo = func.outputList;

		// printf("OUTPUT : %d\n",func.noOfOutput);
		// while(tempo != NULL)
		// {
		// 	printf("%s %s %d %d %d\n" , tempo->v.name , char_type[(int)tempo->v.type] , tempo->v.linedec , tempo->v.scopeDepth, tempo->v.offset);
		// 	tempo = tempo->next;
		// }

	}
	else if(root->ruleNode->type==0 && (root->tk.type == END) && (strcmp(root->parent->ruleNode->name,"<functionDef>")==0))
	{
		function f = root->st->f;
		variablenodeptr out_ptr = f.outputList;
		while(out_ptr != NULL)
		{
			variablenodeptr t = getID(out_ptr->v.name,root->st);
			if(t != NULL && !t->v.assigned)
			{
				printf( "Line No.%d : Output parameter %s of function %s is not defined in the function.\n",f.linedec, out_ptr->v.name,f.name );
				//err = true;
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
	// 			printf( "Line No.%d : Conditional expression of while loop in line %d must be of boolean type\n",root->parent->child[0]->tk.lineno );
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
	// 		printf( "Line No.%d : No such variable %s in line %d\n" , root->parent->child[0]->tk.lexeme , root->parent->child[0]->tk.lineno );
	// 		err = true;
	// 		return;
	// 	}
	// 	sw_type = vt;
	// 	if(sw_type == RE)
	// 	{
	// 		printf( "Line No.%d : Switch case variable cannot be real at line %d\n",root->parent->child[0]->tk.lineno );
	// 		err = true;
	// 	}
	// 	else if(vt.isarr)
	// 	{
	// 		printf( "Line No.%d : Switch case variable %s cannot be of array type at line %d\n",root->parent->child[0]->tk.lexeme,root->parent->child[0]->tk.lineno );
	// 		err = true;
	// 	}
	// 	else if(sw_type == INT)
	// 	{
	// 		if(root->parent->child[3]->child[0]->tk.type == eps)
	// 		{
	// 			printf( "Line No.%d : Default case statement required in switch statement starting from line %d\n",root->parent->child[0]->tk.lineno );
	// 			err = true;
	// 		}	
	// 	}
	// 	else if(sw_type == BOOL)
	// 	{
	// 		if(root->parent->child[3]->child[0]->tk.type != eps)
	// 		{
	// 			printf( "Line No.%d : Default case statement not allowed in switch statement starting from line %d\n",root->parent->child[0]->tk.lineno );	
	// 			err = true;
	// 		}
	// 	}
	// }

	// else if(!root->ruleNode->type==0 && strcmp(root->name, "value"))
	// {
	// 	if(sw_type == INT && root->child[0]->tk.type != NUM)
	// 	{
	// 		printf( "Line No.%d : Expected integer case label at line %d\n",root->child[0]->tk.lineno );
	// 		err = true;
	// 	}	
	// 	else if(sw_type == BOOL && root->child[0]->tk.type == NUM)
	// 	{
	// 		printf( "Line No.%d : Expected boolean case label at line %d\n",root->child[0]->tk.lineno );
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