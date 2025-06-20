%{
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "abstree.h"
#include "goianinha.tab.h"
#define YY_NO_FILENO

#ifndef fileno
#define fileno(f) _fileno(f)  // MSVC-style fileno
#endif

extern void loadMemory(void * ptr);
%}
%option yylineno noyywrap nounput never-interactive

ID                  [a-zA-Z][a-zA-Z0-9_]*
WHITESPACE          [\t\r\n' ']
CONSTINT            [0-9]+
CADEIA_CARACTERES   [^\n\"]* 
ASPA                \"
APOSTROFO           \'
CONSTCAR            {APOSTROFO}[a-zA-Z0-9_ ]{APOSTROFO}
CONSTSTR            {ASPA}{CADEIA_CARACTERES}{ASPA}
ABRE_COMENTARIO     \/\*
CADEIA_COMENTARIO   ([^\*]|\*[^\/])*
FECHA_COMENTARIO    \*\/
COMENTARIO          {ABRE_COMENTARIO}{CADEIA_COMENTARIO}{FECHA_COMENTARIO}

%%
programa                {return PROGRAM;}
retorne                 {return RETURN;}
se                      {return IF;}
entao                   {return THEN;}
senao                   {return ELSE;}
enquanto                {return WHILE;}
execute                 {return DO;}
leia                    {return READ;}
escreva                 {return WRITE;}
novalinha               {return NEWLINE;}
OU                      {return OR_OP;}
E                       {return AND_OP;}
!                       {return NOT_OP;}
==                      {return COMP_EQ;}
!=                      {return COMP_NE;}
\>                      {return COMP_GT;}
\<                      {return COMP_LT;}
\>=                     {return COMP_GE;}
\<=                     {return COMP_LE;}
\+                      {return PLUS_SIGN;}
\-                      {return MINUS_SIGN;}
\*                      {return MULT_SIGN;}
\/                      {return DIV_SIGN;}
=                       {return ASIGN_OP;}
\{                      {return CURLY_OPEN;}
\}                      {return CURLY_CLOSE;}
\(                      {return PAR_OPEN;}
\)                      {return PAR_CLOSE;}
,                       {return COMMA;}
;                       {return SEMICOLON;}
car                     {
                            return TYPE_CHAR;
                        }
int                     {
                            return TYPE_INT;
                        }
{CONSTCAR}              {   
                            yylval.sym = yytext[1]; return CHAR;
                        }
{CONSTSTR}              {
                            char * newStr = calloc(yyleng + 1, sizeof(char));
                            loadMemory(newStr);
                            yylval.str = strncpy(newStr, yytext, yyleng); 
                            return STRING;
                        }
{CONSTINT}              {
                            char * newStr = calloc(yyleng + 1, sizeof(char));
                            strncpy(newStr, yytext, yyleng); 
                            yylval.num = atoi(newStr);
                            free(newStr);
                            return NUMBER;
                        }
{ID}                    {
                            char * newStr = calloc(yyleng + 1, sizeof(char));
                            yylval.str = strncpy(newStr, yytext, yyleng);
                            loadMemory(newStr);
                            return ID;
                        }
{COMENTARIO}            {}
{ASPA}                  {printf("ERRO NA LINHA %d: CADEIA DE CARACTERES OCUPA MAIS DE UMA LINHA OU NAO TERMINA.\n", yylineno); return -1;}
{ABRE_COMENTARIO}       {printf("ERROR NA LINHA %d: COMENTARIO NAO TERMINA.\n", yylineno); return -1;}
{WHITESPACE}            /* Não fazer nada */
.                       {printf("ERRO NA LINHA: %d: CARACTERE INVALIDO.\n", yylineno); return -1;}
%%
