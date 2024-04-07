mysh: mysh.c lines.c lines.h
	gcc -o mysh mysh.c lines.c
argsprog: argsprog.c
	gcc -o argsprog argsprog.c
redir: redir.c
	gcc -o redir redir.c
lines: lines.c lines.h
	gcc -o lines lines.c