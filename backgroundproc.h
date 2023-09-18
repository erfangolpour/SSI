#ifndef BACKGROUND_H
#define BACKGROUND_H

#include <stdbool.h>

struct BackgroundProcess;

void add_background_process(pid_t pid, const char* command);
void remove_background_process(pid_t pid);
void pause_background_process(pid_t pid);
void resume_background_process(pid_t pid);
void kill_background_process(pid_t pid);
void bglist();
void check_background_processes();
void free_background_processes();

#endif // BACKGROUND_H
