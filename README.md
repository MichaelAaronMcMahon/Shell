Authors:
Michael McMahon mam1198
Ammar Ghauri amg610

---Test Specification---

Test1: Mysh executes a program
When given the name of an executable program, mysh runs it. Gives mysh the name prog, a program which prints the string "prog executed".
input: 
./mysh
prog
expected output (in the terminal):
prog executed 

Test2: Mysh executes a program with arguments
When given the name of an executable program that takes arguments and space separated arguments, mysh runs it with those arguments. 
Gives mysh the name argsprog and a list of the arguments aaa bbb ccc, a program which prints its arguments in the format "argv[1]: aaa"
input:
./mysh
progargs aaa bbb ccc
expected output (in the terminal):
argv[0]: argsprog
argv[1]: aaa
argv[2]: bbb
argv[3]: ccc

Test3: Mysh finds all matching files to a wildcard expression and uses them as arguments of the program to be run.
When given a file path with an asterisk in the file name, mysh adds all matching files in the specified directory as arguments for the program.
Uses foo/bar/quu*txt as an argument in the command, foo/bar being a directory containing the files quu2.txt and quuabc.txt.
input:
./mysh
argsprog foo/bar/quu*txt
expected output (in the terminal):
argv[0]: argsprog
argv[1]: quuabc.txt
argv[2]: quu2.txt

Test4: When no matching files are found in the specified directory of a wildcard expression, the wildcard expression itself is added as an argument
to the program to be run.
input:
./mysh
argsprog foo/bar/xyz*txt
expected output (in the terminal):
argv[0]: argsprog
argv[1]: foo/bar/xyz*txt

Test5: mysh redirects the standard output of a program to a specified file when the > character is used in the command to the subsequent file
Uses > to redirect argsprog's standard output to the text file progout.txt.
input:
./mysh
argsprog abcd efgh ijkl mnop > progout.txt
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
./mysh
redir < progin.txt
expected output (in the terminal):
beginning of redir
123 xyz abcd lmnop
end of redir

Test7: mysh handles redirection of both standard input and standard output in one command
input:
./mysh
./redir < progin.txt > progout.txt
expected output (written to progout.txt):
beginning of redir
123 xyz abcd lmnop
end of redir

Test8: mysh can use a pipe to connect the standard input of one program to the standard output of another. 
Uses the pipe character "|" to connect the standard input of redir to the standard output of argsprog.
input:
./mysh
argsprog foo bar baz | redir
expected output (in the terminal):
beginning of redir
argv[0]: argsprog
argv[1]: foo
argv[2]: bar
argv[3]: baz

end of redir

Test9