%{
  #include <stdio.h>
  #include "abstree.h"
  extern int yylex(void);
  int yyerror(const char *s);
  extern int yylineno;
  extern int is_error;
%}

%union {
    int num;
    unsigned char sym;
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
}

%token TYPE_INT
%token TYPE_CHAR

%token<num> NUMBER
%token<str> ID
%token<str> STRING
%token<sym> CHAR

%token COMMA
%token SEMICOLON
%token PAR_OPEN
%token PAR_CLOSE
%token CURLY_OPEN
%token CURLY_CLOSE

%token PLUS_SIGN
%token MINUS_SIGN
%token MULT_SIGN
%token DIV_SIGN
%token ASIGN_OP
%token NOT_OP
%token OR_OP
%token AND_OP

%token COMP_EQ
%token COMP_NE
%token COMP_LT
%token COMP_LE
%token COMP_GT
%token COMP_GE

%token PROGRAM
%token RETURN
%token IF
%token THEN
%token ELSE
%token WHILE
%token DO

%token READ
%token WRITE

%token NEWLINE


%type<block> declProg 
%type<declfv> declFuncVar
%type<var> declVar
%type<func> declFunc
%type<params> paramList paramListCont
%type<block> block
%type<vars> declVarList
%type<num> type
%type<comms> commandList
%type<comm> command
%type<expr> expr orExpr andExpr eqExpr desigExpr addExpr multExpr unitExpr
%type<prim> primExpr
%type<exprList> exprList

%%

input: 
    declFuncVar declProg {programCall($1, $2);}
;

declFuncVar: 
      type ID declVar SEMICOLON declFuncVar {$$ = declfvAssignVar($5, $1, declVar($3, $2, yylineno));}
    | type ID declFunc declFuncVar {$$ = declfvAssignFunc($4, $1, $2, $3);}
    | /*epsilon*/ {$$ = declfvCall();}
;

declProg: 
      PROGRAM block {$$ = $2;}
;

declVar: 
      COMMA ID declVar {$$ = declVar($3, $2, yylineno);}
    | /*epsilon*/ {$$ = allocVar();}
;

declFunc: 
      PAR_OPEN paramList PAR_CLOSE block {$$ = declFunc($2, $4, yylineno);}
;

paramList: 
      paramListCont {$$ = $1;}
    | /*epsilon*/ {$$ = NULL;}
;

paramListCont: 
      type ID {$$ = addParam(allocParams(), $1, $2, yylineno);}
    | type ID COMMA paramListCont {$$ = addParam($4, $1, $2, yylineno);}
;

block: 
      CURLY_OPEN declVarList commandList CURLY_CLOSE {$$ = allocBlock($2, $3);}
;

declVarList: 
      type ID declVar SEMICOLON declVarList {$$ = assignVar($5, $1, declVar($3, $2, yylineno));}
    | /*epsilon*/ {$$ = allocVars();}
;

type: 
      TYPE_INT {$$ = 0;}
    | TYPE_CHAR {$$ = 1;}
;

commandList:
      command {$$ = assignComm(allocComms(), $1);}
    | command commandList {$$ = assignComm($2, $1);}
;

command: 
      SEMICOLON {$$ = NULL;}
    | expr SEMICOLON {$$ = newComm(BASE_COMM, $1, NULL, NULL, yylineno);}
    | RETURN expr SEMICOLON {$$ = newComm(RETURN_COMM, $2, NULL, NULL, yylineno);}
    | READ ID SEMICOLON {$$ = newComm(READ_COMM, $2, NULL, NULL, yylineno);}
    | WRITE STRING SEMICOLON {$$ = newComm(WRITE_STR_COMM, $2, NULL, NULL, yylineno);}
    | WRITE expr SEMICOLON {$$ = newComm(WRITE_EXPR_COMM, $2, NULL, NULL, yylineno);}
    | NEWLINE SEMICOLON {$$ = newComm(NEWLINE_COMM, NULL, NULL, NULL, yylineno);}
    | IF PAR_OPEN expr PAR_CLOSE THEN command {$$ = newComm(IF_COMM, $3, $6, NULL, yylineno);}
    | IF PAR_OPEN expr PAR_CLOSE THEN command ELSE command {$$ = newComm(IF_ELSE_COMM, $3, $6, $8, yylineno);}
    | WHILE PAR_OPEN expr PAR_CLOSE DO command {$$ = newComm(WHILE_COMM, $3, $6, NULL, yylineno);}
    | block {$$ = newComm(BLOCK_COMM, $1, NULL, NULL, yylineno);}
;

expr: 
      ID ASIGN_OP expr {$$ = newExpr(ASS_OPTYPE, newExprFromID($1, yylineno), $3, NULL, yylineno);}
    | orExpr {$$ = $1;}
;

orExpr:
      orExpr OR_OP andExpr {$$ = newExpr(OR_OPTYPE, $1, $3, NULL, yylineno);}
    | andExpr {$$ = $1;}
;

andExpr:
      andExpr AND_OP eqExpr {$$ = newExpr(AND_OPTYPE, $1, $3, NULL, yylineno);}
    | eqExpr {$$ = $1;}
;

eqExpr:
      eqExpr COMP_EQ desigExpr {$$ = newExpr(EQ_OPTYPE, $1, $3, NULL, yylineno);}
    | eqExpr COMP_NE desigExpr {$$ = newExpr(NEQ_OPTYPE, $1, $3, NULL, yylineno);}
    | desigExpr {$$ = $1;}
;

desigExpr:
      desigExpr COMP_LT addExpr {$$ = newExpr(LT_OPTYPE, $1, $3, NULL, yylineno);}
    | desigExpr COMP_GT addExpr {$$ = newExpr(GT_OPTYPE, $1, $3, NULL, yylineno);}
    | desigExpr COMP_LE addExpr {$$ = newExpr(LE_OPTYPE, $1, $3, NULL, yylineno);}
    | desigExpr COMP_GE addExpr {$$ = newExpr(GE_OPTYPE, $1, $3, NULL, yylineno);}
    | addExpr {$$ = $1;}
;

addExpr:
      addExpr PLUS_SIGN multExpr {$$ = newExpr(ADD_OPTYPE, $1, $3, NULL, yylineno);}
    | addExpr MINUS_SIGN multExpr {$$ = newExpr(SUB_OPTYPE, $1, $3, NULL, yylineno);}
    | multExpr {$$ = $1;} 
;

multExpr:
      multExpr MULT_SIGN unitExpr {$$ = newExpr(MULT_OPTYPE, $1, $3, NULL, yylineno);}
    | multExpr DIV_SIGN unitExpr {$$ = newExpr(DIV_OPTYPE, $1, $3, NULL, yylineno);}
    | unitExpr {$$ = $1;}
;

unitExpr:
      MINUS_SIGN primExpr {$$ = newExpr(NEG_OPTYPE, NULL, NULL, $2, yylineno);}
    | NOT_OP primExpr {$$ = newExpr(NOT_OPTYPE, NULL, NULL, $2, yylineno);}
    | primExpr {$$ = newExpr(PRIM_OPTYPE, NULL, NULL, $1, yylineno);}
;

primExpr: 
      ID PAR_OPEN exprList PAR_CLOSE {$$ = newPrim(FUNC_CALL, $1, $3, NULL, 0, 0, yylineno);}
    | ID PAR_OPEN PAR_CLOSE {$$ = newPrim(FUNC_CALL, $1, NULL, NULL, 0, 0, yylineno);}
    | ID {$$ = newPrim(VAR, $1, NULL, NULL, 0, 0, yylineno);}
    | CHAR {$$ = newPrim(CAR_CONST, NULL, NULL, NULL, 0, $1, yylineno);}
    | NUMBER {$$ = newPrim(INT_CONST, NULL, NULL, NULL, $1, 0, yylineno);}
    | PAR_OPEN expr PAR_CLOSE {$$ = newPrim(PAR_EXPR, NULL, NULL, $2, 0, 0, yylineno);}
;

exprList:
      expr {$$ = stackExpr(allocExprs(yylineno), $1);} 
    | expr COMMA exprList   {$$ = stackExpr($3, $1);}
;

%%


int yyerror(const char* s) {
    printf("ERROR NA LINHA %d: %s\n", yylineno, s);
    is_error = 1;
    return 0;
}