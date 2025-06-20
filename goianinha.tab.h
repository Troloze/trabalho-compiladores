
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton interface for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     TYPE_INT = 258,
     TYPE_CHAR = 259,
     NUMBER = 260,
     ID = 261,
     STRING = 262,
     CHAR = 263,
     COMMA = 264,
     SEMICOLON = 265,
     PAR_OPEN = 266,
     PAR_CLOSE = 267,
     CURLY_OPEN = 268,
     CURLY_CLOSE = 269,
     PLUS_SIGN = 270,
     MINUS_SIGN = 271,
     MULT_SIGN = 272,
     DIV_SIGN = 273,
     ASIGN_OP = 274,
     NOT_OP = 275,
     OR_OP = 276,
     AND_OP = 277,
     COMP_EQ = 278,
     COMP_NE = 279,
     COMP_LT = 280,
     COMP_LE = 281,
     COMP_GT = 282,
     COMP_GE = 283,
     PROGRAM = 284,
     RETURN = 285,
     IF = 286,
     THEN = 287,
     ELSE = 288,
     WHILE = 289,
     DO = 290,
     READ = 291,
     WRITE = 292,
     NEWLINE = 293,
     ERROR = 294
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 1676 of yacc.c  */
#line 10 "goianinha.y"

    int num;
    char sym;
    char* str;
    program* program;
    declfv* declfv;
    vars* vars;
    var* var;
    funcs* funcs;
    func* func;
    params* params;
    param* param;
    block* block;
    comms* comms;
    comm* comm;
    expr* expr;
    exprList* exprList;
    prim* prim;



/* Line 1676 of yacc.c  */
#line 113 "goianinha.tab.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE yylval;


