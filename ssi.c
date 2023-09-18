/*
 * Title: ssi.c
 * Description: Simple Shell Interpreter
 * Course: CSC 360
 * Author: Erfan Golpour
 * Date: 2019-12-01
 */

#include <fcntl.h>
#include <readline/history.h>
#include <readline/readline.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#include "backgroundproc.h"

#define MAX_LINE 80
#define MAX_ARGS 40

char *get_prompt() {
    char *prompt = malloc(MAX_LINE);
    char *username = getlogin();  // get username
    char hostname[MAX_LINE];
    gethostname(hostname, MAX_LINE);  // get hostname
    char *path = getcwd(NULL, 0);     // get current working directory
    sprintf(prompt, "\033[1;32m%s@%s\033[0m: \033[1;34m%s\033[0m > ", username, hostname, path);
    free(path);
    return prompt;
}

void execute_command(char *command, bool run_in_background) {
    pid_t pid;
    int status;

    // Split the command into arguments
    char *args[MAX_ARGS];
    int arg_count = 0;

    char *token = strtok(command, " ");
    while (token != NULL) {
        args[arg_count++] = token;
        token = strtok(NULL, " ");
    }
    args[arg_count] = NULL;  // Null-terminate the argument list

    // Fork a new process
    pid = fork();

    if (pid < 0) {
        perror("Fork failed");
        exit(1);
    } else if (pid == 0) {  // Child process
        // Redirect standard output and standard error to /dev/null if the command is to be run in the background
        if (run_in_background) {
            int devnull = open("/dev/null", O_WRONLY);
            if (devnull == -1) {
                perror("Open failed");
                exit(1);
            }
            dup2(devnull, STDOUT_FILENO);
            dup2(devnull, STDERR_FILENO);
            close(devnull);
        }

        // Execute the command in the child process
        if (execvp(args[0], args) == -1) {
            perror("Execv failed");
            exit(1);
        }
    } else {  // Parent process
        if (run_in_background) {
            // Add the background process to the linked list
            add_background_process(pid, command);
        } else {
            // Wait for the child process to finish
            waitpid(pid, &status, 0);

            // Only print the exit status if the child exited abnormally
            if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
                printf("Exit status: %d\n", WEXITSTATUS(status));
            }
        }
    }
}

int main(int argc, char *argv[]) {
    char *command;
    char *prompt;

    // Set up the signal handler
    struct sigaction sa;
    sa.sa_handler = check_background_processes;
    sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
    sigaction(SIGCHLD, &sa, NULL);

    while (true) {
        prompt = get_prompt();
        command = readline(prompt);
        add_history(command);

        if (!strcmp(command, "exit")) {
            break;
        } else if (!strcmp(command, "cd") || !strcmp(command, "cd ~")) {
            chdir(getenv("HOME"));
        } else if (!strncmp(command, "cd ", 3)) {
            chdir(command + 3);
        } else if (!strncmp(command, "bg ", 3)) {
            execute_command(command + 3, true);
        } else if (!strcmp(command, "bglist")) {
            bglist();
        } else if (!strncmp(command, "bgpause ", 8)) {
            pid_t pid = atoi(command + 8);
            if (pid > 0) {
                pause_background_process(pid);
            } else {
                printf("Invalid PID. Please enter a valid integer PID.\n");
            }
        } else if (!strncmp(command, "bgresume ", 9)) {
            pid_t pid = atoi(command + 9);
            if (pid > 0) {
                resume_background_process(pid);
            } else {
                printf("Invalid PID. Please enter a valid integer PID.\n");
            }
        } else if (!strncmp(command, "bgkill ", 7)) {
            pid_t pid = atoi(command + 7);
            if (pid > 0) {
                kill_background_process(pid);
            } else {
                printf("Invalid PID. Please enter a valid integer PID.\n");
            }
        } else if (!strcmp(command, "history")) {
            HIST_ENTRY **history = history_list();
            for (int i = 0; history[i] != NULL; ++i) {
                printf("%d: %s\n", i, history[i]->line);
            }
        } else {
            execute_command(command, false);
        }

        free(command);
        free(prompt);
    }

    free_background_processes();

    return 0;
}
