#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

#define MAX_LINE 80

struct BackgroundProcess {
    pid_t pid;
    char command[MAX_LINE];
    bool paused;
    struct BackgroundProcess *next;
};
struct BackgroundProcess *bg_processes = NULL;

void add_background_process(pid_t pid, const char *command) {
    struct BackgroundProcess *process = (struct BackgroundProcess *)malloc(sizeof(struct BackgroundProcess));
    if (process == NULL) {
        perror("Memory allocation failed");
        exit(1);
    }

    process->pid = pid;
    strncpy(process->command, command, MAX_LINE);
    process->paused = false;
    process->next = NULL;

    // Add the process to the linked list
    if (bg_processes == NULL) {
        bg_processes = process;
    } else {
        struct BackgroundProcess *current = bg_processes;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = process;
    }

    printf("Background process with PID %d started\n", pid);
}

void remove_background_process(pid_t pid) {
    struct BackgroundProcess *current = bg_processes;
    struct BackgroundProcess *prev = NULL;

    while (current != NULL) {
        if (current->pid == pid) {
            if (prev == NULL) {
                // Process is the first in the list
                bg_processes = current->next;
            } else {
                prev->next = current->next;
            }
            free(current);
            return;
        }

        prev = current;
        current = current->next;
    }
}

void bglist() {
    printf("Background processes:\n");
    struct BackgroundProcess *current = bg_processes;
    if (current == NULL) {
        printf("No background processes\n");
        return;
    }
    while (current != NULL) {
        printf("PID: %d\tCommand: %s\t%s\n", current->pid, current->command, current->paused ? "(Paused)" : "");
        current = current->next;
    }
}

void check_background_processes() {
    int status;
    pid_t pid;

    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        // Find and remove the terminated background process from the list
        remove_background_process(pid);

        if (WIFEXITED(status)) {
            printf("\nBackground process with PID %d exited with status %d\n", pid, WEXITSTATUS(status));
        }
    }
}

void pause_background_process(pid_t pid) {
    if (kill(pid, SIGSTOP) == -1) {
        perror("Failed to pause background process");
    } else {
        printf("Background process with PID %d paused\n", pid);
        // Find the process in the linked list and mark it as paused
        struct BackgroundProcess *current = bg_processes;
        while (current != NULL) {
            if (current->pid == pid) {
                current->paused = true;
                break;
            }
            current = current->next;
        }
    }
}

void resume_background_process(pid_t pid) {
    if (kill(pid, SIGCONT) == -1) {
        perror("Failed to resume background process");
    } else {
        printf("Background process with PID %d resumed\n", pid);
        // Find the process in the linked list and mark it as not paused
        struct BackgroundProcess *current = bg_processes;
        while (current != NULL) {
            if (current->pid == pid) {
                current->paused = false;
                break;
            }
            current = current->next;
        }
    }
}

void kill_background_process(pid_t pid) {
    if (kill(pid, SIGTERM) == -1) {
        perror("Failed to kill background process");
    } else {
        printf("Background process with PID %d killed\n", pid);
        // Find and remove the terminated background process from the list
        remove_background_process(pid);
    }
}

void free_background_processes() {
    // Free the background process linked list
    struct BackgroundProcess *current = bg_processes;
    while (current != NULL) {
        struct BackgroundProcess *temp = current;
        current = current->next;
        kill(temp->pid, SIGTERM);
        free(temp);
    }
}