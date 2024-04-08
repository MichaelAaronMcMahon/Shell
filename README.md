Authors:
Michael McMahon mam1198
Ammar Ghauri amg610

---Test Specification---

Tests 1-15: Interactive mode: Upon running mysh without any commands, it is expected to print "Welcome to my shell" to the terminal and then
"Enter a command: " before each command. Start the program with ./mysh and then enter each of the commands under "input" to test each case
in interactive mode. 

Test1: Mysh executes a program
When given the name of an executable program, mysh runs it. Gives mysh the name prog, a program which prints the string "prog executed".
input: 
./prog
expected output (in the terminal):
prog executed 

Test2: Mysh executes a program with arguments
When given the name of an executable program that takes arguments and space separated arguments, mysh runs it with those arguments. 
Gives mysh the name argsprog and a list of the arguments aaa bbb ccc, a program which prints its arguments in the format "argv[1]: aaa"
input:
./argsprog aaa bbb ccc
expected output (in the terminal):
argv[0]: argsprog
argv[1]: aaa
argv[2]: bbb
argv[3]: ccc

Test3: Mysh finds all matching files to a wildcard expression and uses them as arguments of the program to be run.
When given a file path with an asterisk in the file name, mysh adds all matching files in the specified directory as arguments for the program.
Uses foo/bar/quu*txt as an argument in the command, foo/bar being a directory containing the files quu2.txt and quuabc.txt.
input:
./argsprog foo/bar/quu*txt
expected output (in the terminal):
argv[0]: argsprog
argv[1]: quuabc.txt
argv[2]: quu2.txt

Test4: When no matching files are found in the specified directory of a wildcard expression, the wildcard expression itself is added as an argument
to the program to be run.
input:
./argsprog foo/bar/xyz*txt
expected output (in the terminal):
argv[0]: argsprog
argv[1]: foo/bar/xyz*txt

Test5: mysh redirects the standard output of a program to a specified file when the > character is used in the command to the subsequent file
Uses > to redirect argsprog's standard output to the text file progout.txt.
input:
./argsprog abcd efgh ijkl mnop > progout.txt
expected output (written to progout.txt):
argv[0]: argsprog
argv[1]: abcd
argv[2]: efgh
argv[3]: ijkl
argv[4]: mnop

Test6: mysh redirects the standard input of a program to a specified file when the < character is used in the command to the subsequent file
Uses < to redirect redir's standard input to the text file progin.txt.
redir: a program which prints the line "beginning of redir", then reads and prints data from its standard input, and finally prints the line 
"end of redir".
progin.txt: a text file containing the line "123 xyz abcd lmnop"
input:
./redir < progin.txt
expected output (in the terminal):
beginning of redir
123 xyz abcd lmnop
end of redir

Test7: mysh handles redirection of both standard input and standard output in one command
input:
./redir < progin.txt > progout.txt
expected output (written to progout.txt):
beginning of redir
123 xyz abcd lmnop
end of redir

Test8: mysh can use a pipe to connect the standard input of one program to the standard output of another. 
Uses the pipe character "|" to connect the standard input of redir to the standard output of argsprog.
input:
./argsprog foo bar baz | ./redir
expected output (in the terminal):
beginning of redir
argv[0]: argsprog
argv[1]: foo
argv[2]: bar
argv[3]: baz

end of redir

Test9: mysh can run programs within subdirectories
Tests that mysh accepts pathnames with subdirectories as program names and runs the program correctly
dirprog: a program in ./foo/bar which prints the first character of each of its arguments.
input:
./foo/bar/dirprog abc def ghi
expected output (in the terminal):
.
a
d
g

Test 10: Conditional statements: Commands starting with the "then" keyword only execute if the previous command succeeded. 
If a conditional command is not executed, the previous exit status remains unchanged.
Enter a command that will fail and then a command beginning with "then". Then enter the same command again, showing that conditional commands not
executing don't change the previous exit status. Then run a successful command, then run a command beginning with "then".
input:
gkjdnfblk
then ./prog
then ./prog
./argsprog aaa bbb ccc
then ./prog
expected output (in the terminal):
execv: No such file or directory
Previous command failed, command not executed.
Previous command failed, command not executed.
argv[0]: ./argsprog
argv[1]: aaa
argv[2]: bbb
argv[3]: ccc
prog executed

Test 11: Conditional statements: Commands starting with the "else" keyword only execute if the previous command failed. 
If a conditional command is not executed, the previous exit status remains unchanged.
Enter a command that will succeed and then a command beginning with "else". Then enter the same command again, showing that conditional commands not
executing don't change the previous exit status. Then run a failing command, then run a command beginning with "else".
input:
./prog
else ./argsprog e f g
else ./argsprog e f g
sdafkgnjgl
else ./argsprog e f g
expected output (in the terminal):
prog executed
Previous command succeeded, command not executed.
Previous command succeeded, command not executed.
execv: No such file or directory
argv[0]: ./argsprog
argv[1]: e
argv[2]: f
argv[3]: g

Test 12: Redirection takes precedence in the case that both piping and redirection are specified in a command
Connects the output of argsprog with the commands "this should be lost to redir", but redirects the standard input of redir to the text file
progin.txt, which contains the line "123 xyz abcd lmnop".
input: 
./argsprog this should be lost | ./redir < progin.txt
expected output (in the terminal):
beginning of redir
123 xyz abcd lmnop
end of redir

Batch mode test: runs all of the previous commands in batch mode with the file batch.txt which contains all of the commands in order.
End the previous interactive session with the exit command.
input: ./mysh batch.txt

expected output (in the terminal):
prog executed
argv[0]: ./argsprog
argv[1]: aaa
argv[2]: bbb
argv[3]: ccc
argv[0]: ./argsprog
argv[1]: quuabc.txt
argv[2]: quu2.txt
argv[0]: ./argsprog
argv[1]: foo/bar/xyz*txt
beginning of redir
123 xyz abcd lmnop
end of redir
beginning of redir
argv[0]: ./argsprog
argv[1]: foo
argv[2]: bar
argv[3]: baz

end of redir
.
a
d
g
execv: No such file or directory
Previous command failed, command not executed.
Previous command failed, command not executed.
argv[0]: ./argsprog
argv[1]: aaa
argv[2]: bbb
argv[3]: ccc
prog executed
prog executed
Previous command succeeded, command not executed.
Previous command succeeded, command not executed.
execv: No such file or directory
argv[0]: ./argsprog
argv[1]: e
argv[2]: f
argv[3]: g
beginning of redir
123 xyz abcd lmnop
end of redir

expected output( in progout.txt):
beginning of redir
123 xyz abcd lmnop
end of redir


Test 13: Built-in Commands(pwd): Tests if pwd correctly prints the current working directory. No special preparation needed for this; simply compile with the makefile, run the program using './mysh',run pwd, and expect the current directory as output.
 Commands;
 1. ./mysh
 2. pwd

For error handling, we tried to run pwd with a command after it to confirm the error message we put in the program would a pead (which it does)

Test 14: Built-in Commands(cd): The goal of this test was to verify the shell's cd command functionality by changing directories within the shell environment and using pwd to display the current directory to ensure the changes took effect. The test involves navigating into a subdirectory, moving back up the directory tree, and then moving back into the original directory.

Environment Setup:

Initial directory: /common/home/amg610/CS214
Test directory: testFol (located within the initial directory)

Test Steps:

Initial Directory Check

Command: pwd
Expected Output: /common/home/amg610/CS214
Enter Test Directory

Command: cd testFol then pwd
Expected Output: /common/home/amg610/CS214/testFol
Navigate Up Once

Command: cd .. then pwd
Expected Output: /common/home/amg610/CS214
Navigate Up Again

Command: cd .. then pwd
Expected Output: /common/home/amg610
Return to Initial Directory

Command: cd CS214 then pwd
Expected Output: /common/home/amg610/CS214
Result: The shell's cd functionality allows for expected directory navigation, demonstrating its correct implementation.

We then checked for the error handling by trying to use change directory into a directory what did not exist (Which worked). We also tried to use cd when there where no argument as well as when there were two or more arguments ('cd' , 'cd myShell testFol' , etc.). We were met with an error message just as programmed.


Test 15: Built-in Commands(which): Test the which command functionality in a custom shell to verify it can accurately identify the location of executables within the system's PATH.  which is used to locate a command by searching the directories listed in the environment's PATH variable. It should return the path of the executable file if found.

Environment Setup and Test Execution:

Create Test Program

A program, prog.c, is prepared and compiled. This step ensures there is an executable to search for.
Make Executable Globally Accessible for the Session

Commands used:
bash
Copy code
gcc prog.c -o prog
chmod +x prog
mkdir -p $HOME/bin
export PATH=$PATH:$HOME/bin
These commands compile prog.c to an executable prog, make it executable, ensure there's a bin directory in the home directory, and add this bin directory to the session's PATH. This setup mimics making prog globally accessible.
Testing which Command

After compiling mysh.c and starting the custom shell with ./mysh, the which prog command is issued.
Expected Output: The path to the prog executable for the session, e.g., /common/home/amg610/bin/prog. This confirms which successfully locates executables within the PATH.
Error Handling Tests:

No Arguments: Running which without any arguments should prompt an error message indicating the incorrect number of arguments.
Multiple Arguments: Similar error handling for multiple arguments to ensure only one argument is accepted.
Non-existent Command: Before adding prog to the PATH, which prog should return an error or indicate that prog is not found, verifying that which only locates programs within the PATH.



Test 15: Built-in Commands(exit): The exit command is a built-in feature that instructs the shell to terminate the current session. It's a crucial command for closing the shell when the user has completed their tasks. We simply compile and run our code using ./mysh. When it asks for a command, all we do is type exit and then it goes to the next line and stops asking and executing commands. 

Testing Process:

Compile and Run the Shell:

The custom shell is compiled (if necessary) and launched by executing ./mysh in the terminal. This step initiates the shell session, ready to accept commands from the user.
Issue exit Command:

Upon the shell prompting "Enter a command:", the user inputs exit and presses Enter. This action sends the instruction to terminate the shell session.
Observe Behavior:

Expected Behavior: The shell stops asking for new commands and returns control to the original terminal session or command prompt. The transition is smooth, without errors, indicating the shell has gracefully closed.

---Bare names---
As of right now bare name commands cause problems with pipe commands when run in the same interactive session
or from the same batch file. Testing these cases upon running ./mysh afresh work, however.
Run ./mysh and then enter all of the commands under "input".

Test b1: mysh takes bare names as program names and searches for them in /usr/local/bin, /usr/bin and /bin in that order
Attempts to find and run pip in /usr/local/bin
input:
pip 
expected output (in the terminal):
Usage:   
  pip <command> [options]

Commands:
  install                     Install packages.
  download                    Download packages.
  uninstall                   Uninstall packages.
  freeze                      Output installed packages in requirements format.
  inspect                     Inspect the python environment.
  list                        List installed packages.
  show                        Show information about installed packages.
  check                       Verify installed packages have compatible dependencies.
  config                      Manage local and global configuration.
  search                      Search PyPI for packages.
  cache                       Inspect and manage pip's wheel cache.
  index                       Inspect information available from package indexes.
  wheel                       Build wheels from your requirements.
  hash                        Compute hashes of package archives.
  completion                  A helper command used for command completion.
  debug                       Show information useful for debugging.
  help                        Show help for commands.

General Options:
  -h, --help                  Show help.
  --debug                     Let unhandled exceptions propagate outside the main subroutine, instead of logging them to stderr.
  --isolated                  Run pip in an isolated mode, ignoring environment variables and user configuration.
  --require-virtualenv        Allow pip to only run in a virtual environment; exit with an error otherwise.
  --python <python>           Run pip with the specified Python interpreter.
  -v, --verbose               Give more output. Option is additive, and can be used up to 3 times.
  -V, --version               Show version and exit.
  -q, --quiet                 Give less output. Option is additive, and can be used up to 3 times (corresponding to WARNING, ERROR, and CRITICAL logging levels).
  --log <path>                Path to a verbose appending log.
  --no-input                  Disable prompting for input.
  --keyring-provider <keyring_provider>
                              Enable the credential lookup via the keyring library if user input is allowed. Specify which mechanism to use [disabled, import, subprocess]. (default: disabled)
  --proxy <proxy>             Specify a proxy in the form scheme://[user:passwd@]proxy.server:port.
  --retries <retries>         Maximum number of retries each connection should attempt (default 5 times).
  --timeout <sec>             Set the socket timeout (default 15 seconds).
  --exists-action <action>    Default action when a path already exists: (s)witch, (i)gnore, (w)ipe, (b)ackup, (a)bort.
  --trusted-host <hostname>   Mark this host or host:port pair as trusted, even though it does not have valid or any HTTPS.
  --cert <path>               Path to PEM-encoded CA certificate bundle. If provided, overrides the default. See 'SSL Certificate Verification' in pip documentation for more information.
  --client-cert <path>        Path to SSL client certificate, a single file containing the private key and the certificate in PEM format.
  --cache-dir <dir>           Store the cache data in <dir>.
  --no-cache-dir              Disable the cache.
  --disable-pip-version-check
                              Don't periodically check PyPI to determine whether a new version of pip is available for download. Implied with --no-index.
  --no-color                  Suppress colored output.
  --no-python-version-warning
                              Silence deprecation warnings for upcoming unsupported Pythons.
  --use-feature <feature>     Enable new functionality, that may be backward incompatible.
  --use-deprecated <feature>  Enable deprecated functionality, that will be removed in the future.

Test b2: mysh takes bare names as program names and searches for them in /usr/local/bin, /usr/bin and /bin in that order
Attempts to find and run texplate in /usr/bin
input:
texplate
expected output (in the terminal):
 ______         __   __          ___             __             
/\__  _\       /\ \ /\ \        /\_ \           /\ \__          
\/_/\ \/    __ \ `\`\/'/'  _____\//\ \      __  \ \ ,_\    __   
   \ \ \  /'__`\`\/ > <   /\ '__`\\ \ \   /'__`\ \ \ \/  /'__`\ 
    \ \ \/\  __/   \/'/\`\\ \ \L\ \\_\ \_/\ \L\.\_\ \ \_/\  __/ 
     \ \_\ \____\  /\_\\ \_\ \ ,__//\____\ \__/.\_\\ \__\ \____\
      \/_/\/____/  \/_/ \/_/\ \ \/ \/____/\/__/\/_/ \/__/\/____/
                             \ \_\                              
                              \/_/                              

TeXplate 1.0.4, a document structure creation tool
Copyright (c) 2024, Island of TeX
All rights reserved.

Missing required option: '--output=<output>'
Usage: texplate [-c=<configuration>] -o=<output> [-t=<template>]
                [-m=<String=String>...]...
  -c, --config=<configuration>
                          The configuration file in which the tool
                            can read template data, for automation
                            purposes. Make sure to follow the correct
                            specification when writing a
                            configuration file.
  -m, --map=<String=String>...
                          The contextual map that provides the data
                            to be merged in the template. This
                            parameter can be used multiple times. You
                            can specify a map entry with the
                            key=value syntax (mind the entry
                            separator).
  -o, --output=<output>   The output file in which the chosen
                            template will be effectively written.
                            Make sure the directory has the correct
                            permissions for writing the output file.
  -t, --template=<template>
                          The name of the template. The tool will
                            search both user and system locations and
                            set the template model accordingly, based
                            on your specs.

Test b3: mysh takes bare names as program names and searches for them in /usr/local/bin, /usr/bin and /bin in that order
Attempts to find and run ciptool in /bin
input:
ciptool
expected output (in the terminal):
ciptool - Bluetooth Common ISDN Access Profile (CIP)

Usage:
        ciptool [options] [command]

Options:
        -i [hciX|bdaddr]   Local HCI device or BD Address
        -h, --help         Display help

Commands:
        show                    Show remote connections
        search                  Search for a remote device
        connect  <bdaddr>       Connect a remote device
        release  [bdaddr]       Disconnect the remote device
        loopback <bdaddr>       Loopback test of a device
