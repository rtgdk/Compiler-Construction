mycompiler: driver.o lexer.o parser.o
	gcc -o stage1exe driver.o lexer.o parser.o 
	 	
driver.o: driver.c
	gcc -c -w driver.c

parser.o: parser.c
	gcc -c -w parser.c
	
lexer.o: lexer.c
	gcc -c -w lexer.c







