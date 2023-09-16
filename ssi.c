/*
 * Title: ssi.c
 * Description: Simple Shell Interpreter
 * Course: CSC 360
 * Author: Erfan Golpour
 * Date: 2019-12-01
 */

#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <unistd.h>

#define MAX_LINE 80
#define MAX_ARGS 40
#define MAX_PATH 20

/* Function to generate prompt in the form username@hostname: path > */
char *get_prompt()
{
    char *prompt = malloc(MAX_LINE);
    char *username = getlogin(); // get username
    char hostname[MAX_LINE];
    gethostname(hostname, MAX_LINE); // get hostname
    char *path = getcwd(NULL, 0);    // get current working directory
    sprintf(prompt, "%s@%s: %s > ", username, hostname, path);
    free(path);
    return prompt;
}

int main(int argc, char *argv[])
{
    const char *prompt = get_prompt();

    int bailout = 0;
    while (!bailout)
    {

        char *reply = readline(prompt);

        if (!strcmp(reply, "bye"))
        {
            bailout = 1;
        }
        else
        {
            printf("\nYou said: %s\n\n", reply);
        }

        free(reply);
    }
    printf("Bye Bye\n");
}