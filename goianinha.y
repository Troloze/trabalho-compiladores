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

%token ERROR

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
      type ID declVar SEMICOLON declFuncVar {$$ = declfvAssignVar($5, $1, declVar($3, $2));}
    | type ID declFunc declFuncVar {$$ = declfvAssignFunc($4, $1, $2, $3);}
    | /*epsilon*/ {$$ = declfvCall();}
;

declProg: 
      PROGRAM block {$$ = $2;}
;

declVar: 
      COMMA ID declVar {$$ = declVar($3, $2);}
    | /*epsilon*/ {$$ = allocVar();}
;

declFunc: 
      PAR_OPEN paramList PAR_CLOSE block {$$ = declFunc($2, $4);}
;

paramList: 
      paramListCont {$$ = $1;}
    | /*epsilon*/ {$$ = NULL;}
;

paramListCont: 
      type ID {$$ = addParam($$ = allocParams(), $1, $2);}
    | type ID COMMA paramListCont {$$ = addParam($4, $1, $2);}
;

block: 
      CURLY_OPEN declVarList commandList CURLY_CLOSE {$$ = allocBlock($2, $3);}
;

declVarList: 
      type ID declVar SEMICOLON declVarList {$$ = assignVar($5, $1, declVar($3, $2));}
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
      SEMICOLON {}
    | expr SEMICOLON {$$ = newComm(BASE_COMM, $1, NULL, NULL);}
    | RETURN expr SEMICOLON {$$ = newComm(RETURN_COMM, $2, NULL, NULL);}
    | READ ID SEMICOLON {$$ = newComm(READ_COMM, $2, NULL, NULL);}
    | WRITE STRING SEMICOLON {$$ = newComm(WRITE_STR_COMM, $2, NULL, NULL);}
    | WRITE expr SEMICOLON {$$ = newComm(WRITE_EXPR_COMM, $2, NULL, NULL);}
    | NEWLINE SEMICOLON {$$ = newComm(NEWLINE_COMM, NULL, NULL, NULL);}
    | IF PAR_OPEN expr PAR_CLOSE THEN command {$$ = newComm(IF_COMM, $3, $6, NULL);}
    | IF PAR_OPEN expr PAR_CLOSE THEN command ELSE command {$$ = newComm(IF_ELSE_COMM, $3, $6, $8);}
    | WHILE PAR_OPEN expr PAR_CLOSE DO command {$$ = newComm(WHILE_COMM, $3, $6, NULL);}
    | block {$$ = newComm(BLOCK_COMM, $1, NULL, NULL);}
;

expr: 
      ID ASIGN_OP expr {$$ = newExpr(ASS_OPTYPE, newExprFromID($1), $3, NULL);}
    | orExpr {$$ = $1;}
;

orExpr:
      orExpr OR_OP andExpr {$$ = newExpr(OR_OPTYPE, $1, $3, NULL);}
    | andExpr {$$ = $1;}
;

andExpr:
      andExpr AND_OP eqExpr {$$ = newExpr(AND_OPTYPE, $1, $3, NULL);}
    | eqExpr {$$ = $1}
;

eqExpr:
      eqExpr COMP_EQ desigExpr {$$ = newExpr(EQ_OPTYPE, $1, $3, NULL);}
    | eqExpr COMP_NE desigExpr {$$ = newExpr(NEQ_OPTYPE, $1, $3, NULL);}
    | desigExpr {$$ = $1}
;

desigExpr:
      desigExpr COMP_LT addExpr {$$ = newExpr(LT_OPTYPE, $1, $3, NULL);}
    | desigExpr COMP_GT addExpr {$$ = newExpr(GT_OPTYPE, $1, $3, NULL);}
    | desigExpr COMP_LE addExpr {$$ = newExpr(LE_OPTYPE, $1, $3, NULL);}
    | desigExpr COMP_GE addExpr {$$ = newExpr(GE_OPTYPE, $1, $3, NULL);}
    | addExpr {$$ = $1}
;

addExpr:
      addExpr PLUS_SIGN multExpr {$$ = newExpr(ADD_OPTYPE, $1, $3, NULL);}
    | addExpr MINUS_SIGN multExpr {$$ = newExpr(SUB_OPTYPE, $1, $3, NULL);}
    | multExpr {$$ = $1} 
;

multExpr:
      multExpr MULT_SIGN unitExpr {$$ = newExpr(MULT_OPTYPE, $1, $3, NULL);}
    | multExpr DIV_SIGN unitExpr {$$ = newExpr(DIV_OPTYPE, $1, $3, NULL);}
    | unitExpr {$$ = $1}
;

unitExpr:
      MINUS_SIGN primExpr {$$ = newExpr(NEG_OPTYPE, NULL, NULL, $2);}
    | NOT_OP primExpr {$$ = newExpr(NOT_OPTYPE, NULL, NULL, $2);}
    | primExpr {$$ = newExpr(PRIM_OPTYPE, NULL, NULL, $1)}
;

primExpr: 
      ID PAR_OPEN exprList PAR_CLOSE {$$ = newPrim(FUNC_CALL, $1, $3, NULL, 0, 0);}
    | ID PAR_OPEN PAR_CLOSE {$$ = newPrim(FUNC_CALL, $1, NULL, NULL, 0, 0);}
    | ID {$$ = newPrim(VAR, $1, NULL, NULL, 0, 0);}
    | CHAR {$$ = newPrim(CAR_CONST, NULL, NULL, NULL, 0, $1);}
    | NUMBER {$$ = newPrim(INT_CONST, NULL, NULL, NULL, $1, 0);}
    | PAR_OPEN expr PAR_CLOSE {$$ = newPrim(PAR_EXPR, NULL, NULL, $2, 0, 0);}
;

exprList:
      expr {$$ = stackExpr(allocExprs(), $1);} 
    | exprList COMMA expr {$$ = stackExpr($1, $3);}
;

%%


int yyerror(const char* s) {
    printf("ERROR ON LINE %d: %s\n", yylineno, s);
    is_error = 1;
    return 0;
}