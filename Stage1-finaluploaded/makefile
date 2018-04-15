mycompiler: driver.o lexer.o parser.o ast.o
	gcc -o stage1exe driver.o lexer.o parser.o ast.o 
	 	
driver.o: driver.c
	gcc -c -w driver.c

ast.o: ast.c
	gcc -c -w ast.c
	
parser.o: parser.c
	gcc -c -w parser.c
	
lexer.o: lexer.c
	gcc -c -w lexer.c







