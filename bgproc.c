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
} *bg_processes = NULL;

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
    struct BackgroundProcess *current = bg_processes;
    if (current == NULL) {
        printf("No background processes\n");
        return;
    }
    int count = 0;
    while (current != NULL) {
        printf("%d:\t%s\t%s\n", current->pid, current->command, current->paused ? "(Paused)" : "");
        current = current->next;
        count++;
    }
    printf("Total background jobs: %d\n", count);
}

bool does_process_exist(pid_t pid) {
    // Check if the process exists and is a child of the shell
    struct BackgroundProcess *current = bg_processes;
    while (current != NULL) {
        if (current->pid == pid) {
            return true;
        }
        current = current->next;
    }
    return false;
}

void pause_background_process(pid_t pid) {
    if (!does_process_exist(pid)) {
        printf("Background process with PID %d does not exist or is not a child of this shell\n", pid);
    } else if (kill(pid, SIGSTOP) == -1) {
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
    if (!does_process_exist(pid)) {
        printf("Background process with PID %d does not exist or is not a child of this shell\n", pid);
    } else if (kill(pid, SIGCONT) == -1) {
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
    if (!does_process_exist(pid)) {
        printf("Background process with PID %d does not exist or is not a child of this shell\n", pid);
    } else if (kill(pid, SIGTERM) == -1) {
        perror("Failed to gracefully terminate background process");
        printf("Attempting to force kill background process with PID %d\n", pid);
        if (kill(pid, SIGKILL) == -1) {
            perror("Failed to kill background process");
        }
    } else {
        printf("Background process with PID %d killed\n", pid);
        // Find and remove the terminated background process from the list
        remove_background_process(pid);
    }
}

void free_background_processes() {
    // Free the background processes linked list
    struct BackgroundProcess *current = bg_processes;
    while (current != NULL) {
        struct BackgroundProcess *temp = current;
        current = current->next;
        if (kill(temp->pid, SIGTERM) == -1) {
            perror("Failed to gracefully terminate background process");
            printf("Attempting to force kill background process with PID %d\n", temp->pid);
            if (kill(temp->pid, SIGKILL) == -1) {
                perror("Failed to kill background process");
            }
        }
        free(temp);
    }
}