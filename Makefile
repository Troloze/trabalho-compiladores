

prog.exe: lex.yy.c goianinha.tab.c goianinha.tab.h main.c main.h abstree.c abstree.h semantic.c semantic.h stack.c stack.h
	gcc -std=c11 -D_XOPEN_SOURCE=700 goianinha.tab.c lex.yy.c main.c abstree.c semantic.c stack.c -o prog

lex.yy.c: goianinha.l
	flex goianinha.l

goianinha.tab.c goianinha.tab.h: goianinha.y
	bison goianinha.y -d