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
CONSTINT            \-?[0-9]+
CADEIA_CARACTERES   [^\n\"]* 
ASPA                \"
APOSTROFO           \'
CONSTCAR            {APOSTROFO}([ -&\(-~]|\\([\"\'\\abtnvfre?]|[0-3]?[0-7]{1,2}|[x][0-9a-fA-F]{1,2})){APOSTROFO}
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
                            if (yytext[1] == '\\') {
                                if (yyleng == 3) {
                                    yylval.sym = yytext[1]; 
                                    return CHAR;
                                }
                                switch (yytext[2]) {
                                    case 'a':
                                        yylval.sym = 7;
                                        break;
                                    case 'b':
                                        yylval.sym = 8;
                                        break;
                                    case 't':
                                        yylval.sym = 9;
                                        break;
                                    case 'n':
                                        yylval.sym = 10;
                                        break;
                                    case 'v':
                                        yylval.sym = 11;
                                        break;
                                    case 'f':
                                        yylval.sym = 12;
                                        break;
                                    case 'r':
                                        yylval.sym = 13;
                                        break;
                                    case 'e':
                                        yylval.sym = 27;
                                        break;
                                    case '\\':
                                        yylval.sym = 92;
                                        break;
                                    case '\'':
                                        yylval.sym = 39;
                                        break;
                                    case '\"':
                                        yylval.sym = 34;
                                        break;
                                    case '?':
                                        yylval.sym = 63;
                                        break;
                                    case 'x':
                                        yylval.sym = 0;
                                        if (yytext[3] > 'F') yylval.sym += 10 + yytext[3] - 'a'; 
                                        else if (yytext[3] > '9') yylval.sym += 10 + yytext[3] - 'A'; 
                                        else yylval.sym += yytext[3] - '0';
                                        if (yyleng == 5) break;
                                        yylval.sym *= 16;
                                        if (yytext[4] > 'F') yylval.sym += 10 + yytext[4] - 'a'; 
                                        else if (yytext[4] > '9') yylval.sym += 10 + yytext[4] - 'A'; 
                                        else yylval.sym += yytext[4] - '0';
                                        break;
                                    default: 
                                        yylval.sym = 0;
                                        yylval.sym += yytext[2] - '0';
                                        if (yyleng == 4) break;
                                        yylval.sym *= 8;
                                        yylval.sym += yytext[3] - '0';
                                        if (yyleng == 5) break;
                                        yylval.sym *= 8;
                                        yylval.sym += yytext[4] - '0';
                                }
                                return CHAR;
                            }
                            yylval.sym = yytext[1]; 
                            return CHAR;
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
{APOSTROFO}             {printf("ERRO NA LINHA %d: CARACTERE E INVALIDO, OCUPA MAIS DE UMA LINHA OU NAO TERMINA.\n", yylineno); return -1;}
{ABRE_COMENTARIO}       {printf("ERROR NA LINHA %d: COMENTARIO NAO TERMINA.\n", yylineno); return -1;}
{WHITESPACE}            /* Não fazer nada */
.                       {printf("ERRO NA LINHA: %d: CARACTERE INVALIDO.\n", yylineno); return -1;}
%%
