#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "abstree.h"
#include "semantic.h"
#include "translator.h"
#include "goianinha.tab.h"
/* Incluir externs do flex */
extern int yylex();
extern int yylineno;
extern char* yytext;

extern int yyparse();

// Garbage collector
void loadMemory(void * ptr);
void freeMemory();