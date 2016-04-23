#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <memory.h>

//Declare the functions we want to use as string parameteres
//NOTE: builtin functions like "cd" and "exit" is not yet supported
char *functions[] = {
        "ls\n",
        "cd\n",
        "clear\n",
        "exit\n",
        "help\n"
    };
    
//Execute a command provided by the user
void execute_cmd(char cmd[]) {
    int i;
    //Check the functions to determine which system call to perform
    for (i = 0; i < sizeof(functions)/sizeof(functions[0]); i++) {
        if(strcmp(cmd, functions[i]) == 0) {
            system(cmd);
            break;
        }
    }
}

#define MAX_DATA 1024
int main() {
	char line[MAX_DATA];
	printf("Welcome to the new shell!\n");
	
	while(1) {
	    printf("$ ");
	    //reads the user input and store it in line, will break if input is
	    //greater than 1024 bytes
		if (!fgets(line, 1024, stdin)) break;
		execute_cmd(line);
	}
	return 0;
}
