Title: Assignment 1 - SSI (Simple Shell Interpreter)
Description: A simple shell interpreter written in C
Author: Erfan Golpour #V00991341
Course: CSC 360
Date: 2023-09-29

How to compile and run the program:
- Compile the program using `make`
- Run the program using `./ssi`
- Use 'exit' to exit the program

Extra Features (previously discussed with the instructor):
- Command history: use the `history` command to see a list of commands issued in the current session.
- Auto Completion: thanks to the `readline` library, the user can use the `tab` key to auto complete commands and paths.
- Handling quotations: this implementation handles single and double quotations within input commands. For instance, a command like `git commit -m "Extra Features"`` is tokenized as ["git", "commit", "-m", "Extra Features"], rather than ["git", "commit", "-m", ""Extra", "Features""].
- Instant announcements: termination of background processes is immediately announced to the user using the "signal" library.
- Input/Output redirection: the input and output of background processes are redirected to /dev/null in order to avoid cluttering/blocking the terminal.
- More control over background processes: use `bgpause`, `bgresume`, and `bgkill` to pause/resume/kill background processes respectively.
- Handling of `SIGINT` (ctrl-c): the shell ignores the `SIGINT` signal, and passes them to the foreground process when it is running.
- Colored prompt!