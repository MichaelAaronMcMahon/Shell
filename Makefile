mysh: mysh.c lines.c lines.h
	gcc -o mysh mysh.c lines.c
argsprog: argsprog.c
	gcc -o argsprog argsprog.c
redir: redir.c
	gcc -o redir redir.c
dirprog: ./foo/bar/dirprog.c
	gcc -o ./foo/bar/dirprog ./foo/bar/dirprog.c
prog: prog.c
	gcc -o prog prog.c