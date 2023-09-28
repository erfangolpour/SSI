#include <fcntl.h>
#include <readline/history.h>
#include <readline/readline.h>
#include <signal.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "bgproc.h"
#include "tokenizer.h"

#define MAX_LINE 80

int current_fg_pid = -1;  // PID of the current foreground process

char *get_prompt();
void execute_command(char **args, bool run_in_background);
void check_background_processes();
void handle_sigint();

int main(int argc, char *argv[]) {
    char *command;
    char *prompt;

    // Set up the signal handler for child processes
    struct sigaction sa;
    sa.sa_handler = check_background_processes;
    sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
    sigaction(SIGCHLD, &sa, NULL);

    // Set up the signal handler for Ctrl+C
    struct sigaction sac;
    sac.sa_handler = handle_sigint;
    sac.sa_flags = SA_RESTART;
    sigaction(SIGINT, &sac, NULL);

    while (true) {
        prompt = get_prompt();
        command = readline(prompt);
        add_history(command);

        char **args = tokenize(command);
        int token_count = 0;
        while (args[token_count] != NULL) {
            token_count++;
        }

        if (token_count == 0) {
            free(args);
            free(prompt);
            free(command);
            continue;
        }

        if (!strcmp(args[0], "exit")) {
            free(args);
            free(prompt);
            free(command);
            break;
        } else if (!strcmp(args[0], "cd")) {
            if (token_count == 1) {
                chdir(getenv("HOME"));
            } else if (!strcmp(args[1], "~")) {
                chdir(getenv("HOME"));
            } else if (chdir(args[1]) == -1) {
                perror("Failed to change directory");
            }
        } else if (!strcmp(args[0], "bg")) {
            if (token_count > 1) {
                execute_command(args + 1, true);
            } else {
                printf("Please enter a command to execute in the background.\n");
            }
        } else if (!strcmp(args[0], "bglist")) {
            bglist();
        } else if (!strcmp(args[0], "bgpause")) {
            if (token_count > 1) {
                pid_t pid = atoi(args[1]);
                if (pid > 0) {
                    pause_background_process(pid);
                } else {
                    printf("Invalid PID. Please enter a valid integer PID.\n");
                }
            } else {
                printf("Please enter a PID.\n");
            }
        } else if (!strcmp(args[0], "bgresume")) {
            if (token_count > 1) {
                pid_t pid = atoi(args[1]);
                if (pid > 0) {
                    resume_background_process(pid);
                } else {
                    printf("Invalid PID. Please enter a valid integer PID.\n");
                }
            } else {
                printf("Please enter a PID.\n");
            }
        } else if (!strcmp(args[0], "bgkill")) {
            if (token_count > 1) {
                pid_t pid = atoi(args[1]);
                if (pid > 0) {
                    kill_background_process(pid);
                } else {
                    printf("Invalid PID. Please enter a valid integer PID.\n");
                }
            } else {
                printf("Please enter a PID.\n");
            }
        } else if (!strcmp(args[0], "history")) {
            HIST_ENTRY **history = history_list();
            for (int i = 0; history[i] != NULL; i++) {
                printf("    %d: %s\n", i + 1, history[i]->line);
            }
        } else {
            execute_command(args, false);
        }

        free(args);
        free(prompt);
        free(command);
    }

    free_background_processes();

    return 0;
}

char *get_prompt() {
    char *prompt = malloc(MAX_LINE);
    if (prompt == NULL) {
        perror("Memory allocation failed");
        exit(1);
    }
    char *username = getlogin();  // get username
    char hostname[MAX_LINE];
    gethostname(hostname, MAX_LINE);  // get hostname
    char *path = getcwd(NULL, 0);     // get current working directory
    sprintf(prompt, "\033[1;32m%s@%s\033[0m: \033[1;34m%s\033[0m > ", username, hostname, path);
    free(path);
    return prompt;
}

void execute_command(char **args, bool run_in_background) {
    pid_t pid;

    // Fork a new process
    pid = fork();

    if (pid < 0) {
        perror("Fork failed.");
        exit(1);

    } else if (pid == 0) {  // Child process
        // Redirect standard output and standard error to /dev/null if the command is to be run in the background
        if (run_in_background) {
            signal(SIGINT, SIG_IGN);  // Ignore SIGINT in background processes

            int devnull = open("/dev/null", O_RDWR);
            if (devnull == -1) {
                perror("Failed to redirect standard output and standard error to /dev/null.");
            } else {
                // redirect stdout and stderr to /dev/null
                dup2(devnull, STDOUT_FILENO);
                dup2(devnull, STDERR_FILENO);
                // redirect stdin to /dev/null
                dup2(devnull, STDIN_FILENO);
                // close /dev/null
                close(devnull);
            }
        }

        // Execute the command in the child process
        if (execvp(args[0], args) != 0) {
            perror("Failed to execute command");
            exit(1);
        }
    } else {  // Parent process
        if (run_in_background) {
            // Add the background process to the linked list
            // The initial command must be reconstructed from the args array
            char command[MAX_LINE];
            strcpy(command, args[0]);
            for (int i = 1; args[i] != NULL; i++) {
                strcat(command, " ");
                strcat(command, args[i]);
            }
            add_background_process(pid, command);
        } else {
            int status;
            current_fg_pid = pid;

            // Wait for the child process to finish
            waitpid(pid, &status, 0);

            // Only print the exit status if the child exited abnormally
            if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
                printf("Execution of the previous command failed with exit status %d\n", WEXITSTATUS(status));
            }

            current_fg_pid = -1;
        }
    }
}

void check_background_processes() {
    int status;
    pid_t pid;

    while ((pid = waitpid(0, &status, WNOHANG)) > 0) {
        // Find and remove the terminated background process from the list
        remove_background_process(pid);

        if (WIFEXITED(status)) {
            printf("\nBackground process with PID %d has terminated with status code %d\n", pid, WEXITSTATUS(status));

            char *prompt = get_prompt();
            printf("%s", prompt);
            printf("%s", rl_line_buffer);
            free(prompt);
            rl_redisplay();
        }
    }
}

void handle_sigint() {
    // Print a newline and the prompt
    printf("\n");
    if (current_fg_pid != -1) {
        kill(current_fg_pid, SIGINT);
    } else {
        char *prompt = get_prompt();
        printf("%s", prompt);
        free(prompt);
        for (int i = strlen(rl_line_buffer); i > 0; i--) {
            printf(" ");
        }
        rl_replace_line("", 0);
        rl_redisplay();
    }
}