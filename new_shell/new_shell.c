#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <memory.h>

//Get arguments from a supplied command, this is only necessary for special 
//commands like 'cd' which cannot be called upon by system().
char * getargs(char *cmd) {
    char *args = (char *)malloc(strlen(cmd));
    //Fill in processing here to filter out args
    return args;
}
   
//Execute a command provided by the user
void execute_cmd(char *cmd) {
    char *args = getargs(cmd);
    if (strcmp(cmd, "cd") == 0) {
        if (!args || strlen(args) == 0) {
            printf("Supplied no args to cd: usage is cd <path/to/dir>");
        } else {
            chdir(args);
        }
    }
    else if (strcmp(cmd, "exit") == 0) {
        exit(0);
    } else {
        system(cmd);
    }
}

#define MAX_DATA 1024
int main() {
	char line[MAX_DATA];
	char *cmd;
	char *args;
	int i;
	printf("Welcome to the new shell!\n");
	
	while(1) {
	    printf("$ ");
	    //reads the user input and store it in line, will break if input is
	    //greater than 1024 bytes
		if (!fgets(line, 1024, stdin)) break;
		cmd = (char *)malloc(strlen(line));
		//Copy the content that is string from line to cmd, skipping '\n'
		for (i = 0; i < strlen(line) - 1; i++) {
		    cmd[i] = line[i];
		}
		
		execute_cmd(cmd);
		//system(line);
		free(cmd);
	}
	return 0;
}
