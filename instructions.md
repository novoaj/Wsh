# CS 537 Project 3 -- Shell

Administrivia
-------------

- **Due Date**: February 20th, at 11:59pm
- Projects may be turned in up to 3 days late but you will receive a penalty of
10 percentage points for every day it is turned in late.
- **Slip Days**: 
  - In case you need extra time on projects, you each will have 2 slip days for individual projects and 3 slip days for group projects (5 total slip days for the semester). After the due date we will make a copy of the handin directory for on time grading. 
  - To use a slip days or turn in your assignment late you will submit your files with an additional file that contains only a single digit number, which is the number of days late your assignment is (e.g 1, 2, 3). Each consecutive day we will make a copy of any directories which contain one of these slipdays.txt files. This file must be present when you submit you final submission, or we won't know to grade your code. 
  - We will track your slip days and late submissions from project to project and begin to deduct percentages after you have used up your slip days.
  - After using up your slip days you can get up to 90% if turned in 1 day late, 80% for 2 days late, and 70% for 3 days late, but for any single assignment we won't accept submissions after the third days without an exception. This means if you use both of your individual slip days on a single assignment you can only submit that assignment one additional day late for a total of 3 days late with a 10% deduction.
  - Any exception will need to be requested from the instructors.

  - Example slipdays.txt
```
1
```
- Test will be provided in ```~cs537-1/tests/P3```. There is a ```README.md``` file in that directory which contains the instructions to run
the tests. The tests are partially complete and your project may be graded on additional tests, however the tests aim to be complete insofar as parsing and corner cases we would like you to handle.
- Questions: We will be using Piazza for all questions.
- Collaboration: The assignment must be done by yourself. Copying code (from others) is considered cheating. [Read this](http://pages.cs.wisc.edu/~remzi/Classes/537/Spring2018/dontcheat.html) for more info on what is OK and what is not.
Please help us all have a good semester by not doing this.
- This project is to be done on the [Linux lab machines
](https://csl.cs.wisc.edu/docs/csl/2012-08-16-instructional-facilities/),
so you can learn more about programming in C on a typical UNIX-based
platform (Linux).  Your solution will be tested on these machines.
- **Handing it in**: Copy `wsh.c` and any header files you may have written to your handin directory, `~cs537-1/handin/<login>/P3/`. Please do not write multiple `.c` files. You should also create a `README.md` file describing your implementation. This file should include your name, your cs login, you wisc ID and email, and the status of your implementation. If it all works then just say that. If there are things you know don't work, let us know.
- If applicable, a **document describing online resources used** called **resources.txt**. You are welcome to use resources online you help you with your assignment. **We don't recommend you use Large-Language Models such as ChatGPT.** For this course in particular we have seen these tools give close, but not quite right examples or explanations, that leave students more confused if they don't already know what the right answer is. Be aware that when you seek help from the instructional staff, we will not assist with working with these LLMs and we will expect you to be able to walk the instructional staff member through your code and logic. Online resources (e.g. stack overflow) and generative tools are transforming many industries including computer science and education.  However, if you use online sources, you are required to turn in a document describing your uses of these sources. Indicate in this document what percentage of your solution was done strictly by you and what was done utilizing these tools. Be specific, indicating sources used and how you interacted with those sources. Not giving credit to outside sources is a form of plagiarism. It can be good practice to make comments of sources in your code where that source was used. You will not be penalized for using LLMs or reading posts, but you should not create posts in online forums about the projects in the course. The majority of your code should also be written from your own efforts and you should be able to explain all the code you submit.


## Unix Shell

In this project, you’ll build a simple Unix shell. The shell is the heart of the command-line interface, and thus is central to the Unix/C programming environment. Mastering use of the shell is necessary to become proficient in this world; knowing how the shell itself is built is the focus of this project.

There are three specific objectives to this assignment:

* To further familiarize yourself with the Linux programming environment.
* To learn how processes are created, destroyed, and managed.
* To gain exposure to the necessary functionality in shells.

## Overview

In this assignment, you will implement a *command line interpreter (CLI)* or, as it is more commonly known, a *shell*. The shell should operate in this basic way: when you type in a command (in response to its prompt), the shell creates a child process that executes the command you entered and then prompts for more user input when it has finished.

The shells you implement will be similar to, but simpler than, the one you run every day in Unix. If you don't know what shell you are running, it’s probably `bash` or `zsh`. One thing you should do on your own time is to learn more about your shell, by reading the man pages or other online materials.

## Program Specifications

### Basic Shell: `wsh`

Your basic shell, called `wsh` (short for Wisconsin Shell, naturally), is an interactive loop: it prints a prompt `wsh> ` (note the space after the greater-than sign), parses the input when the user presses enter, executes the command, and waits for the command to finish before printing a new prompt. This is repeated until the user types `exit`. The name of your executable should be `wsh`.

The shell can be invoked with either no arguments or a single argument; anything else is an error. Here is the no-argument way:

```sh
prompt> ./wsh
wsh> 
```

At this point, `wsh` is running, and ready to accept commands. Type away!

The mode above is called *interactive* mode, and allows the user to type commands directly. The shell also supports a *batch* mode, which instead reads input from a batch file and executes commands from therein. Here is how you run the shell with a batch file named `script.wsh`:

```sh
prompt> ./wsh script.wsh
```

One difference between batch and interactive modes: in interactive mode, a prompt is printed (`wsh> `). In batch mode, no prompt should be printed.

You should structure your shell such that it creates a process for each new command (the exception are `built-in` commands, discussed below). Your basic shell should be able to parse a command and run the program corresponding to the command. For example, if the user types `ls -la /tmp`, your shell should run the program `/bin/ls` with the given arguments `-la` and `/tmp` (how does the shell know to run `/bin/ls`? It’s something called the shell **path**; more on this below).

## Structure

### Basic Shell

The shell is very simple (conceptually): it runs in a while loop, repeatedly asking for input to tell it what command to execute. It then executes that command. The loop continues indefinitely, until the user types the built-in command `exit`, at which point it exits. That’s it!

For reading lines of input, you should use `getline()`. This allows you to obtain arbitrarily long input lines with ease. Generally, the shell will be run in *interactive mode*, where the user types a command (one at a time) and the shell acts on it. However, your shell will also support *batch mode*, in which the shell is given an input file of commands; in this case, the shell should not read user input (from `stdin`) but rather from this file to get the commands to execute.

In either mode, if you hit the end-of-file marker (EOF), you should call `exit(0)` and exit gracefully. EOF can be generated by pressing `Ctrl-D`.

To parse the input line into constituent pieces, you might want to use `strsep()`. Read the man page (carefully) for more details.

To execute commands, look into `fork()`, `exec()`, and `wait()/waitpid()`. See the man pages for these functions, and also read the relevant book chapter for a brief overview.

You will note that there are a variety of commands in the `exec` family; for this project, you must use `execvp`. You should **not** use the `system()` library function call to run a command. Remember that if `execvp()` is successful, it will not return; if it does return, there was an error (e.g., the command does not exist). The most challenging part is getting the arguments correctly specified.

### Pipes

Pipe is one of the basic UNIX concepts and `wsh` supports it. It allows composition of multiple simple programs together to create a command with a complex behavior. This is so powerful that it is basically ubiquitous in shell scripts because you can express very complex behavior with very few lines of code. Pipe, denoted as `|`, redirects standard output of the program on the left side to the input of the program on the right side. We recommend using `dup2` to redirect a child's standard output.

```sh
<program1> <arglist1> | <program2> <arglist2> | ... | <programN> <arglistN>
```

The example below shows a command, which compresses a file `f.txt`, decompress it and prints the last 10 lines of the decompressed file.

```sh
cat f.txt | gzip -c | gunzip -c | tail -n 10
```

The processes in this pipe stream should all have the same *process group ID*.  You can read more about processes and process group IDs [here](https://www.win.tue.nl/~aeb/linux/lk/lk-10.html#:~:text=By%20convention%2C%20the%20process%20group,equivalently%2C%20getpgid(0)%20.).

**Our shell is not supposed to handle builtins (described below) within pipes. That is, in pipes, every command is treated as a user program 
and executed through execvp**



### Environment variables and shell variables

Every linux process has its set of environment variables. These variables are stored in the `environ` extern variable. You should use `man environ` to learn more about this.

Some important things about environment are the following:
1. When `fork` is called, the child process gets a copy of the `environ` variable.
2. When a system call from the `exec` family of calls is used, the new process is either given the `environ` variable as its environment or a user specified environment depending on the exact system call used. See `man 3 exec`.
3. There are functions such as `getenv` and `setenv` that allow you to view and change the environment of the current process. See the `man environ` for more details.

Shell variables are different from environment variables. They can only be used within shell commands, are only active for the current session of the shell, and are not inherited by any child processes created by the shell.

We use the built-in `local` command to define and set a shell variable:

```
local MYSHELLVARNAME=somevalue
```

This variable can then be used in a command like so:

```
cd $MYSHELLVARNAME
```

which will translate to 

```
cd somevalue
```

In our implementation of shell, a variable that does not exist should be replaced by an empty string. An assignment to an empty string will clear the variable.

Environment variables may be added or modified by using the built-in `export` command like so:

```
export MYENVVARNAME=somevalue
```

After this command is executed, the `MYENVVARNAME` variable will be present in the environment of any child processes spawned by the shell. 

**Variable substitution**: Whenever the `$` sign is used at the start of a token, it is always followed by a variable name. Variable values should be directly substituted for their names when the shell interprets the command. Tokens in our shell are always separated by whitespace, and variable names and values are guaranteed to each be a single token. For example, given the command `mv $ab $cd,`, you would need to replace variables `ab` and `cd`. If a variable exists as both the environment variable and a shell variable, the environment variable takes precedence. Please note that, for a token to be considred a variable name, the token must start with the `$` sign which means in a command like `cd ab$ef`, the token `ab$ef` is a normal token, it is not a variable name, our shell must not attempt to substitute `$ef`.

You can assume the following when handling variable assignment:
- There will be at most one variable assignment per line.
- Lines containing variable assignments will not include pipes or any other commands.
- The entire value of the variable will be present on the same line, following the `=` operator. There will not be multi-line values; you do not need to worry about quotation marks surrounding the value. 
- Variable names and values will not contain spaces or `=` characters.
- There is no limit on the number of variables you should be able to assign.
- There won't be recursive variable definitions of the form local anothervar=$previousvar

**Displaying Variables**: The `env` utility program (not a shell built-in) can be used to print the environment variables. For local variables, we use a built-in command in our shell called `vars`. Vars will print all of the local variables and their values in the format `<var>=<value>`, one variable per line. Variables should be printed in insertion order, with the most recently created variables printing last. Updates to existing variables will modify them in-place in the variable list, without moving them around in the list. Here's an example:

```
wsh> local a=b
wsh> local c=d
wsh> vars
a=b
c=d
wsh> 
```


### Paths

In our example above, the user typed `ls` but the shell knew to execute the program `/bin/ls`. How does your shell know this?

It turns out there is a **PATH** environment variable to describe the set of directories to search for executables; the set of directories that comprise the path are sometimes called the search path of the shell. The path variable contains the list of all directories to search, in order, when the user types a command. You can find out your path in your default shell (e.g. `bash`) by running `echo $PATH`.

**Important:** Note that the shell itself does not implement `ls` or other commands (except built-ins). All it does is find those executables in one of the directories specified by path and create a new process to run them.

In the `exec` family of commands, certain commands will allow the programmer to specify the directories to search for the executable, while others will use the value from the `PATH` environment variable. You are encouraged to use the later type.

Note: Most shells allow you to specify a binary specifically without using a search path, using either **absolute paths** or **relative paths**. For example, a user could type the absolute path `/bin/ls` and execute the `ls` binary without a search path being needed. A user could also specify a relative path which starts with the current working directory and specifies the executable directly, e.g., `./main`. In this project, you do not have to worry about these features.

### History

Your shell will also keep track of the last five commands executed by the user. Use the `history` builtin command to show the history list as shown here. If the same command is executed more than once consecutively, it should only be stored in the history list once. **The most recent command is number one.** Builtin commands should not be stored in the history.
```
wsh> history
1)  man sleep
2)  man exec
3)  rm -rf a
4)  mkdir a
5)  ls
```

By default, history should store up to five commands. The length of the history should be configurable, using `history set <n>`, where `n` is an integer. If there are fewer commands in the history than its capacity, simply print the commands that are stored (do not print blank lines for empty slots). If a larger history is shrunk using `history set`, drop the commands which no longer fit into the history. 

To execute a command from the history, use `history <n>`, where `n` is the nth command in the history. For example, running `history 1` in the above example should execute `man sleep` again. Commands in the history list should not be recorded in the history when executed this way. This means that successive runs of `history n` should run the same command repeatedly.

If history is called with an integer greater than the capacity of the history, or if history is called with a number that does not have a corresponding command yet, it will do nothing, and the shell should print the next prompt.

### Built-in Commands

Whenever your shell accepts a command, it should check whether the command is a **built-in command**. Built-in commands should be executed directly in your shell, without forking and execing a new process. For example, to implement the `exit` built-in command, you simply call `exit(0);` in your wsh source code, which then will exit the shell.

Here is the list of built-in commands for `wsh`:

* `exit`: When the user types exit, your shell should simply call the `exit` system call with 0 as a parameter. It is an error to pass any arguments to `exit`.
* `cd`: `cd` always take one argument (0 or >1 args should be signaled as an error). To change directories, use the `chdir()` system call with the argument supplied by the user; if `chdir` fails, that is also an error.
* `export`: Used as `export VAR=<value>` to create or assign variable `VAR` as an environment variable.
* `local`: Used as `local VAR=<value>` to create or assign variable `VAR` as a shell variable.
* `vars`: Described earlier in the "environment variables and shell variables" section.
* `history`: Described earlier in the history section.

### Miscellaneous Hints

Remember to get the basic functionality of your shell working before worrying about all of the error conditions and end cases. For example, first get a single command running (probably first a command with no arguments, such as `ls`).

Next, add built-in commands. Then, try working on pipes and terminal control. Each of these requires a little more effort on parsing, but each should not be too hard to implement. It is recommended that you separate the process of parsing and execution - parse first, look for syntax errors (if any), and then finally execute the commands.

We simplify the parsing by having a single space ` ` as the only allowed delimiter. It means that any token on the command line will be delimited by a single space ` ` in our tests.

Check the return codes of all system calls from the very beginning of your work. This will often catch errors in how you are invoking these new system calls. It’s also just good programming sense.

Beat up your own code! You are the best (and in this case, the only) tester of this code. Throw lots of different inputs at it and make sure the shell behaves well. Good code comes through testing; you must run many different tests to make sure things work as desired. Don't be gentle – other users certainly won't be. Finally, keep versions of your code. We recommend using a source control system such as `git`, but older copies of source files will work in a pinch. By keeping older, working versions around, you can comfortably work on adding new functionality, safe in the knowledge you can always go back to an older, working version if need be.

Error conditions should result in `wsh` terminating with an exit code of -1.  Non-error conditions should result in an exit code of 0.
