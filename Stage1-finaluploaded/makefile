mycompiler: driver.o lexer.o parser.o ast.o symbolTable.o semantics.o codegen.o
	gcc -o stage1exe driver.o lexer.o parser.o ast.o symbolTable.o semantics.o codegen.o
	 	
driver.o: driver.c
	gcc -c -w driver.c

codegen.o: codegen.c
	gcc -c -w codegen.c

semantics.o: semantics.c
	gcc -c -w semantics.c
	
symbolTable.o: symbolTable.c
	gcc -c -w symbolTable.c
	
ast.o: ast.c
	gcc -c -w ast.c
	
parser.o: parser.c
	gcc -c -w parser.c
	
lexer.o: lexer.c
	gcc -c -w lexer.c







