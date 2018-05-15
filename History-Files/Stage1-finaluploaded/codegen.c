/*
Rohit Lodha
2015A7PS0040P
*/

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <stdbool.h>

#include "codegen.h"
#include "semantics.h"
#include "symbolTable.h"
#include "ast.h"
#include "parser.h"
#include "lexer.h"

int size[] = {2, 4, 1};

void gen_code_util(char* str,AStree ast)
{
	fasm = fopen(str,"w");
	if(fasm == NULL)
		printf("No such file named %s\n",str);
	else
		gen_code(ast);
}

void gen_code(AStree ast)
{
	//printf("here\n");
	fprintf(fasm, "extern printf\n extern scanf\n");
	fprintf(fasm,"section .data\n");
	fprintf(fasm,"formatin : db \"%%d\",0\n");
	fprintf(fasm,"formatout : db \"%%d\",10,0\n");
	fprintf(fasm,"section .bss\n");
	label = 1;
	currentTable = globalTable->child[0];
	gen_data(currentTable);
	fprintf(fasm,"section .text\n GLOBAL main\nmain:\n");
	gen_prog(ast,currentTable);
	fclose(fasm);
}

void get_var_name(char* name, AStree a) //varaibel a to la10 // 10 line no
{
	//char n[50];
	printf("IN get var name\n");
	strcpy(name , "l");
	variablenodeptr vt = getID(a->tk.lexeme,a->st);
	printf("IN get var name %s\n",a->tk.lexeme);
	strcat(name,a->tk.lexeme);
	char line[50];
	sprintf(line , "%d" , vt->v.linedec);
	strcat(name, line);
	printf("IN get var name\n");

	// name = n;
}

void left(AStree a)
{
	if(a->type == 1)
		fprintf(fasm,"mov ax,dx\nmovsx rax,ax\npush rax\n");
	// else if(a->type == BOOL)
	// 	fprintf(fasm,"mov al,dl\nmovsx rax,ax\npush rax\n");
}

void right(AStree a)
{
	if(a->type == 1)
		fprintf(fasm,"mov bx,dx\n");
	// else if(a->type == BOOL)
	// 	fprintf(fasm,"mov bl,dl\n");
}

// void get_switch_id(char* name,AStree a)
// {
// 	while(a->name.c != conditionalStmt)
// 		a = a->parent;
// 	get_var_name(name,a->child[0]);
// }

// int get_switch_end(AStree a)
// {
// 	while(a->name.c != conditionalStmt)
// 		a = a->parent;
// 	return a->end;
// }

void compose(AStree a,tp t)
{
	if(t == 1)
	{
		if(a->tk.type == PLUS)
		{
			fprintf(fasm,"pop rax\nadd dx,ax\n");
		}
		else if(a->tk.type == MINUS)
			fprintf(fasm,"pop rax\nneg dx\nadd dx,ax\n");
		else if(a->tk.type == MUL)
		{
			fprintf(fasm,"pop rax\nimul dx\nmov dx,ax\n");
		}
		else if(a->tk.type == DIV)
		{
			fprintf(fasm,"pop rax\nmov si,dx\nmov dx,0\nidiv si\nmov dx,ax\n");
		}
		else if(a->tk.type == LT)
		{
			fprintf(fasm,"pop rax\ncmp ax,dx\njl L%d\n",label);
			fprintf(fasm,"mov dl,0\njmp L%d\n",label+1);
			fprintf(fasm,"L%d: mov dl,1\nL%d:\n",label,label+1);
			label+=2;
		}
		else if(a->tk.type == LE)
		{
			fprintf(fasm,"pop rax\ncmp ax,dx\njle L%d\n",label);
			fprintf(fasm,"mov dl,0\njmp L%d\n",label+1);
			fprintf(fasm,"L%d: mov dl,1\nL%d:\n",label,label+1);
			label+=2;
		}
		else if(a->tk.type == GT)
		{
			fprintf(fasm,"pop rax\ncmp ax,dx\njg L%d\n",label);
			fprintf(fasm,"mov dl,0\njmp L%d\n",label+1);
			fprintf(fasm,"L%d: mov dl,1\nL%d:\n",label,label+1);
			label+=2;
		}
		else if(a->tk.type == GE)
		{
			fprintf(fasm,"pop rax\ncmp ax,dx\njge L%d\n",label);
			fprintf(fasm,"mov dl,0\njmp L%d\n",label+1);
			fprintf(fasm,"L%d: mov dl,1\nL%d:\n",label,label+1);
			label+=2;
		}
		else if(a->tk.type == EQ)
		{
			fprintf(fasm,"pop rax\ncmp ax,dx\nje L%d\n",label);
			fprintf(fasm,"mov dl,0\njmp L%d\n",label+1);
			fprintf(fasm,"L%d: mov dl,1\nL%d:\n",label,label+1);
			label+=2;
		}
		else if(a->tk.type == NE)
		{
			fprintf(fasm,"pop rax\ncmp ax,dx\njne L%d\n",label);
			fprintf(fasm,"mov dl,0\njmp L%d\n",label+1);
			fprintf(fasm,"L%d: mov dl,1\nL%d:\n",label,label+1);
			label+=2;
		}
	}
	// else if(t == BOOL)
	// {
	// 	if(a->tk.type == AND)
	// 	{
	// 		fprintf(fasm,"pop rax\nand dl,al\n");
	// 	}
	// 	else if(a->tk.type == OR)
	// 	{
	// 		fprintf(fasm,"pop rax\nor dl,al\n");
	// 	}
	// }
}

void gen_data(SymbolTablePtr currentTable)
{
	int i;
	
	for(i = 0 ; i < 100 ; i++)
	{
		variablenodeptr temp = currentTable->VariableTable[i]; 
		while(temp != NULL)
		{
			variable v = temp->v;

			// if(v.type.isarr)
			// {
			// 	int noe = v.type.high - v.type.low + 1;
			// 	char name[50];
			// 	strcpy(name , v.name);
			// 	sprintf(name , "%s%d" , name, v.line);

			// 	if(v.type.type == INT)
			// 		fprintf(fasm , "l%s: resw %d\n" , name , noe);
			// 	else if(v.type.type == RE)
			// 		fprintf(fasm , "l%s: resd %d\n" , name , noe);
			// 	else if(v.type.type == BOOL)
			// 		fprintf(fasm , "l%s: resb %d\n" , name , noe);

			// 	for(int j = 0 ; j < noe - 1 ; j++)
			// 		temp = temp->next;

			// }
			// else
			// {
				char name[50];
				strcpy(name , v.name);
				sprintf(name , "%s%d" , name, v.linedec);

				if(v.type == 1)
					fprintf(fasm , "l%s: resw 1\n" , name);
				// else if(v.type.type == RE)
				// 	fprintf(fasm , "l%s: resd 1\n" , name);
				// else if(v.type.type == BOOL)
				// 	fprintf(fasm , "l%s: resb 1\n" , name);
			//}
			temp = temp->next;
		}
	}

	for(i = 0 ; i < currentTable->noc ; i++)
		gen_data(currentTable->child[i]);
}

void gen_prog(AStree aroot , SymbolTablePtr currentTable)
{
	printf("Herein prg %s\n",aroot->ruleNode->name);
	if(aroot->ruleNode->type==1)
	{
		printf("%s-%s\n",aroot->ruleNode->name,currentTable->f.name);
		if(strcmp(aroot->ruleNode->name,"<mainFunction>")==0)
		{

			gen_prog(aroot->child[1],currentTable);
			fprintf(fasm , "mov ebx,0\nmov eax,1\nint 0x80\n");
		}
		else if(strcmp(aroot->ruleNode->name,"<stmtAndFunctionDefs>")==0){
			//printf("in her2\n");
			gen_prog(aroot->child[0],currentTable);
			if(aroot->noc==2)
				gen_prog(aroot->child[1],currentTable);
		}
		else if(strcmp(aroot->ruleNode->name,"<moreStmtOrFunctionDef>")==0){
			gen_prog(aroot->child[0],currentTable);
			if(aroot->noc==2)
				gen_prog(aroot->child[1],currentTable);
		}
		else if(strcmp(aroot->ruleNode->name,"<stmtOrFunctionDef>")==0){
			gen_prog(aroot->child[0],currentTable);
		}
		else if(strcmp(aroot->ruleNode->name,"<stmt>")==0)
		{
			gen_prog(aroot->child[0],currentTable);
		}
		else if(strcmp(aroot->ruleNode->name,"<assignmentStmt_type1>")==0){
			printf("in here\n");
			gen_prog(aroot->child[1],currentTable);
			printf("in ASs\n");
			char name[50];
			get_var_name(name,aroot->child[0]->child[0]);
			// if(strcmp(aroot->child[0]->name,"<rightHandSide_type1>"))
			// {	
			printf("%s\n",name );
			printf("%d\n",aroot->child[0]->type );
				if(aroot->child[0]->type == 1)
				{
					fprintf(fasm,"mov word[%s],dx\n",name);
				}

		}
		

		// }
		else if(strcmp(aroot->ruleNode->name,"<rightHandSide_type1>")==0)
		{
			// if(aroot->child[0]->name.c == lvalueIDStmt)
				gen_prog(aroot->child[0],currentTable);
				printf("in RHS\n");
			// else
			// 	gen_prog(aroot->child[0]->child[1],currentTable);
		}
		else if(strcmp(aroot->ruleNode->name,"<arithmeticExpression>")==0)
		{
			
			gen_prog(aroot->child[0],currentTable);
			printf("in A3\n");
			if(aroot->noc>1){
				printf("in AE2\n");
				left(aroot->child[0]);
				gen_prog(aroot->child[1],currentTable);
				compose(aroot->child[1]->child[0]->child[0],aroot->child[0]->type);
			}
		
			// }
		}
		else if(strcmp(aroot->ruleNode->name,"<arithmeticTerm>")==0)
		{
			gen_prog(aroot->child[0],currentTable);
			printf("in AT\n");
			if(aroot->noc>1){
				printf("in AT2\n");
				left(aroot->child[0]);
				gen_prog(aroot->child[1],currentTable);
				compose(aroot->child[1]->child[0]->child[0],aroot->child[0]->type);
			}

		}
		else if(strcmp(aroot->ruleNode->name,"<arithmeticFactor>")==0)
		{
			gen_prog(aroot->child[0],currentTable);
			printf("in AF\n");
		}


		else if(strcmp(aroot->ruleNode->name,"<moreTerms_one>")==0)
		{
			gen_prog(aroot->child[1],currentTable);
			printf("in mt1\n");
			if(aroot->noc > 2)
			{
				printf("in mt12\n");
				left(aroot->child[1]);
				printf("in mt1\n");
				gen_prog(aroot->child[2],currentTable);
				printf("in mt1\n");
				compose(aroot->child[2]->child[0]->child[0],aroot->child[1]->type);
				printf("in mt1\n");
			}
		}
		else if(strcmp(aroot->ruleNode->name,"<moreTerms_two>")==0)
		{
			gen_prog(aroot->child[1],currentTable);
			printf("in mt2\n");
			if(aroot->noc > 2)
			{
				printf("in mt22\n");
				left(aroot->child[1]);
				gen_prog(aroot->child[2],currentTable);
				compose(aroot->child[2]->child[0]->child[0],aroot->child[1]->type);
			}
		}
		else if(strcmp(aroot->ruleNode->name,"<var>")==0)
		{
			if(aroot->child[0]->tk.type == NUM)
			{
				printf("in num\n");
				fprintf(fasm,"mov dx,%s\n",aroot->child[0]->tk.lexeme);
			}
			else 
			{
				printf("in id\n");
				char name[50];
				get_var_name(name,aroot->child[0]);
				int t = aroot->child[0]->type;
				// if(aroot->child[1]->child[0]->is_terminal)
				// {
					if(t == 1)
						fprintf(fasm,"mov dx,[%s]\n",name);
			
			}
		}
		else if(strcmp(aroot->ruleNode->name,"<ioStmt>")==0)
		{
			printf("in io\n");
			if(aroot->child[0]->tk.type==READ)//->name,"READ"))
			{
				//scanning
				int t = aroot->child[1]->type;
				char name[50];
				get_var_name(name,aroot->child[1]);
				//fprintf(fasm,"lea rsi,[%s]\n",name);
				// if(aroot->child[1]->child[0]->is_terminal)
				// {
					variablenodeptr vt = getID(aroot->child[1]->tk.lexeme,aroot->child[1]->st);
					
			    		fprintf(fasm,"mov rdi,formatin\n");
						fprintf(fasm,"lea rsi,[%s]\n",name);
						fprintf(fasm,"mov al,0\ncall scanf\n");
			
			}
			else
			{
				printf("in print\n");
					AStree idnode = aroot->child[1]; 
					int t = idnode->type;
				    char name[50];
				    get_var_name(name,idnode);
				   
				    	variablenodeptr vt = getID(idnode->tk.lexeme,idnode->st);
				   //  	if(vt->var.type.isarr)
				  
				    		fprintf(fasm,"mov rdi,formatout\n");
					     	if(t == 1)
								fprintf(fasm,"movsx rsi,word[%s]\n",name);
							// else if(t == BOOL)
							// 	fprintf(fasm,"movsx rsi,byte[%s]\n",name);
							fprintf(fasm,"mov al,0\ncall printf\n");
					
				
			}
		}
	} 
}
