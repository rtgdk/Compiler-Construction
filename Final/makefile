mycompiler: driver.o lexer.o parser.o symbolTable.o ast.o semantics.o
	gcc -o compiler driver.o lexer.o parser.o symbolTable.o ast.o semantics.o
	 	
driver.o: driver.c
	gcc -c -w driver.c

semantics.o: semantics.c
	gcc -c -w semantics.c
		
ast.o: ast.c
	gcc -c -w ast.c

symbolTable.o: symbolTable.c
	gcc -c -w symbolTable.c
		
parser.o: parser.c
	gcc -c -w parser.c
	
lexer.o: lexer.c
	gcc -c -w lexer.c







