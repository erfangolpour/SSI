void add_background_process(pid_t pid, const char* command);
void remove_background_process(pid_t pid);
void pause_background_process(pid_t pid);
void resume_background_process(pid_t pid);
void kill_background_process(pid_t pid);
void bglist();
void free_background_processes();