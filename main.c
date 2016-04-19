/**
 * For our week 13-16 ICA we have been given a task to write a simple shell implementation
 * in the c language. Except for in the lectures, I have never programmed in C so I've been
 * learning from c.learncodethehardway.org and I've gotten the shell script from this tutorial:
 *      http://brennan.io/2015/01/16/write-a-shell-in-c/
 *
 * This is a simple script allowing for 3 shell commands: 'cd', 'help' and 'exit'.
 * Whitespace are used for seperating arguments.
 *
 * @file        main.c
 * @author      Stephen Brennan, Jorgen Lybeck Hansen
 * @date        10.04.16
 */
#include <malloc.h>
#include <stdlib.h>
#include <memory.h>
#include <unistd.h>
#include <wait.h>


/*
 * Function Declarations for shell commands
 */
int lsh_cd(char **args);
int lsh_help(char **args);
int lsh_exit(char **args);

/*
 * List of commands, followed by corresponding functions
 */
char *builtin_str[] = {
        "cd",
        "help",
        "exit"
};

int (*builtin_func[]) (char **) = {
        &lsh_cd,
        &lsh_help,
        &lsh_exit
};

int lsh_num_builtins() {
    return sizeof(builtin_str) / sizeof(char *);
}


// Function to read through each line
#define LSH_RL_BUFSIZE 1024
char *lsh_read_line(void) {
    // Set to 1 MB as a constant. This is the allocated memory for one line
    int bufsize = LSH_RL_BUFSIZE;
    // Which position in the terminal are we starting to read from
    int position = 0;
    // System call, we store memory allocation. malloc(sizeof(char)) is the byte size of char
    // If you want to allocate memory in c, use sizeof.
    char *buffer = malloc(sizeof(char) * bufsize); // *buffer makes buffer a pointer
    int c;
    // If the buffer is missing, we'll print an error and exit the shell
    if (!buffer) {
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
    }

    while (1) {
        // Read a character
        c = getchar();

        // If we hit EOF, replace it with a null character and return.
        if (c == EOF || c == '\n') {
            buffer[position] = '\0';
            return buffer;
        } else {
            buffer[position] = c;
        }
        position++;

        // If we have exceeded the buffer, reallocate.
        if (position >= bufsize) {
            bufsize += LSH_RL_BUFSIZE;
            buffer = realloc(buffer, bufsize);
            if (!buffer) {
                fprintf(stderr, "lsh: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }
    }
}

char *lsh_read_line_library_implementation(void)
{
    char *line = NULL;
    ssize_t bufsize = 0; //getline() allocate a buffer
    getline(&line, &bufsize, stdin);
    return line;
}

// We will use whitespace for separating arguments in the shell.

#define LSH_TOK_BUFSIZE 64
#define LSH_TOK_DELIM " \t\r\n\a"
char **lsh_split_line(char *line)
{
    int bufsize = LSH_TOK_BUFSIZE, position = 0;
    char **tokens = malloc(bufsize * sizeof(char*));
    char *token, **tokens_backup;

    if (!tokens) {
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, LSH_TOK_DELIM);
    while (token != NULL) {
        tokens[position] = token;
        position++;

        if (position >= bufsize) {
            bufsize += LSH_TOK_BUFSIZE;
            tokens_backup = tokens;
            tokens = realloc(tokens, bufsize * sizeof(char*));
            if (!tokens) {
                free(tokens_backup);
                fprintf(stderr, "lsh: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }

        token = strtok(NULL, LSH_TOK_DELIM);
    }
    tokens[position] = NULL;
    return tokens;
}

        // fork()
// When fork() is called, the OS makes a duplicate of the Init process and runs them both.
// Init is ran when the system powers on and for the whole lifetime of runtime of the OS.
// forking Init let you run a program on another process. The forked process is the Parent of
//  the new process. Creating a Child process of Init will only duplicate Init.
        // exec()
// Here is where exec() joins in. We don't want to duplicate the same process with the same programs.
// We want to run a new program. .exec() replaces a currently running program with a new one.
// Calling .exec() makes the OS stop your process, load a new program and start it instead of the old one.

// First an existing process .fork() itself into two seperate ones,
// Then the new child process use .exec() to replace itself with a new program.
// The parent process can continue to function. Or it can keep tabs on it's children by using the system call wait()

int lsh_launch(char **args)
{
    pid_t pid, wpid;
    int status;
    pid = fork();
    if (pid == 0) {
        // Child process
        if (execvp(args[0], args) == -1) {
            perror("lsh");
        }
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        // Error forking
        perror("lsh");
    } else {
        // Parent process
        do {
            waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }

    return 1;
}

/*
 * Builtin functions
 */
int lsh_cd(char **args)
{
    if (args[1] == NULL) {
        fprintf(stderr, "lsh: expected argument to \"cd\"\n");
    } else {
        if (chdir(args[1]) != 0) {
            perror("lsh");
        }
    }
    return 1;
}

int lsh_help(char **args)
{
    int i;
    printf("IS-213 - Open Source Shell by Group 1.2\n");
    printf("Type program names and arguments, and hit enter.\n");
    printf("The following are built in:\n");

    for (i = 0; i < lsh_num_builtins(); i++) {
        printf("  %s\n", builtin_str[i]);
    }

    printf("Use the man command for information on other programs.\n");
    return 1;
}

int lsh_exit(char **args)
{
    return 0;
}

/*
 * So far this code has been created in 3 parts:
 *      Forward declarations of functions, were we have declared but
 *      not defined functions.
 *
 *      Part 2 is an array of builtin command names, followed by an
 *      array of corresponding functions. Because we did it like this,
 *      we can later add command names by just modifying the array.
 *      This is a better solution than using a large switch statement.
 *
 *      The last part is where functions are implemented.
 *
 *      The missing piece now is the function for executing the
 *      shell
 */
int lsh_execute(char **args)
{
    int i;

    if (args[0] == NULL) {
        // Empty command
        return 1;
    }

    for (i = 0; i < lsh_num_builtins(); i++) {
        if (strcmp(args[0], builtin_str[i]) == 0) {
            return (* builtin_func[i])(args);
        }
    }
    return lsh_launch(args);
}

void lsh_loop(void)
{
    char *line;
    char **args;
    int status;

    do {
        printf("> ");
        line = lsh_read_line_library_implementation();
        args = lsh_split_line(line);
        status = lsh_execute(args);

        free(line);
        free(args);
    } while (status);
}

int main(int argc, char **argv)
{
    // Load configurations

    // Run a function that loops
    lsh_loop();

    // Perform shutdown/cleanup
    return EXIT_SUCCESS;


