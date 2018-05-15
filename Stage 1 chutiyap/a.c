#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

void removeComments(char *testcaseFile, char *cleanFile){
	FILE* f1 = fopen(testcaseFile , "r");
    if(f1 == NULL)
    {
        printf("File %s does not exist!\n" , testcaseFile);
        return;
    }    
    char c;
    while((c = fgetc(f1)) != EOF)
    {        
        if(c != '#')
            putchar(c);
        else
        {
			while((c = fgetc(f1)) != EOF)
                {
                    if(c == '\n')
                    {
                    	putchar('\n');
                        break;
                    }
                }
        }

    }
    fclose(f1);
}

int main(){
	//removeComments("a.txt.txt","b.txt");
	char c='9';
	switch(c){
		case '0' ... '8': printf("%c",c);
	}
	
	return 1;
}

