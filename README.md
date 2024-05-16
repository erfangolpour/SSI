# Simple Shell Interpreter (SSI)

The Simple Shell Interpreter (SSI) is a basic command-line interface that simulates a Unix shell. It provides a prompt, accepts user commands, and executes them. Additionally, it supports background processes, allowing users to run multiple commands concurrently.

## Features

- Interactive command prompt
- Support for built-in commands:
  - `cd` (change directory)
  - `exit` (exit the shell)
  - `bg` (run a command in the background)
  - `bglist` (list all background processes)
  - `bgpause` (pause a background process by PID)
  - `bgresume` (resume a paused background process by PID)
  - `bgkill` (kill a background process by PID)
  - `history` (display command history)
- Signal handling for Ctrl+C (SIGINT) and child processes (SIGCHLD)
- Colored prompt with username, hostname, and current working directory
- Command history and line editing using the GNU Readline library

## Building and Running

To build and run the Simple Shell Interpreter, follow these steps:

1. Ensure that you have the required dependencies installed: `gcc`, `make`, and the `readline` library.

2. Clone the repository or download the source code.

3. Open a terminal and navigate to the project directory.

4. Build the project using the provided `Makefile`:

   ```
   make
   ```

   This will compile the source files and create an executable named `ssi`.

5. Run the Simple Shell Interpreter:

   ```
   ./ssi
   ```

   You should see the shell prompt, and you can start entering commands.

## Usage

Once the shell is running, you can enter commands at the prompt. Here are some examples:

- Execute a command:

  ```
  ls -l
  ```

- Change directory:

  ```
  cd path/to/directory
  ```

- Run a command in the background:

  ```
  bg command arg1 arg2
  ```

- List all background processes:

  ```
  bglist
  ```

- Pause a background process by PID:

  ```
  bgpause 1234
  ```

- Resume a paused background process by PID:

  ```
  bgresume 1234
  ```

- Kill a background process by PID:

  ```
  bgkill 1234
  ```

- View command history:

  ```
  history
  ```

- Exit the shell:

  ```
  exit
  ```

## Code Structure

The project consists of three main source files:

1. `ssi.c`: Contains the main loop and signal handling for the shell.
2. `bgproc.c`: Implements the background process management functionality.
3. `tokenizer.c`: Provides functions for tokenizing user input.

## Contributing

Contributions to the Simple Shell Interpreter project are welcome! If you find any bugs, have suggestions for improvements, or would like to add new features, please open an issue or submit a pull request on the project's GitHub repository.

## License

This project is licensed under the [GPLv3](LICENSE). Feel free to use, modify, and distribute the code as per the terms of the license.
