#include <string.h>
#include <stdlib.h>
#include <stdio.h>

typedef enum commType {
    BASE_COMM,
    RETURN_COMM,
    READ_COMM,
    WRITE_EXPR_COMM,
    WRITE_STR_COMM,
    NEWLINE_COMM,
    IF_COMM,
    IF_ELSE_COMM,
    WHILE_COMM,
    BLOCK_COMM
} commType;

typedef enum opType {
    ASS_OPTYPE,
    OR_OPTYPE,
    AND_OPTYPE,
    EQ_OPTYPE,
    NEQ_OPTYPE,
    LT_OPTYPE,
    LE_OPTYPE,
    GT_OPTYPE,
    GE_OPTYPE,
    ADD_OPTYPE,
    SUB_OPTYPE,
    MULT_OPTYPE,
    DIV_OPTYPE,
    NEG_OPTYPE,
    NOT_OPTYPE,
    PRIM_OPTYPE,
} opType;

typedef enum primType {
    FUNC_CALL,
    VAR,
    CAR_CONST,
    INT_CONST,
    PAR_EXPR
} primType;

typedef struct program program;
typedef struct declfv declfv;
typedef struct vars vars;
typedef struct var var;
typedef struct funcs funcs;
typedef struct func func;
typedef struct params params;
typedef struct param param;
typedef struct block block;
typedef struct comms comms;
typedef struct comm comm;
typedef struct expr expr;
typedef struct exprList exprList;
typedef struct prim prim;

typedef struct program {
    declfv* declfv;
    block* block;
} program;

typedef struct declfv {
    vars* vars;
    funcs* funcs;
} declfv;

typedef struct vars {
    int count;
    var** var;
} vars;

typedef struct var {
    int type;
    int count;
    char** ids;
} var;

typedef struct funcs {
    int count;
    func** func;
} funcs;

typedef struct func {
    int type;
    char* id;
    params* params;
    block* block;
} func;

typedef struct params {
    int count;
    param ** param;    
} params;

typedef struct param {
    int type;
    char * id;
} param;

typedef struct block {
    vars* vars;
    comms* comms;
} block;

typedef struct comms {
    int count;
    comm ** comm;
} comms;

typedef struct comm {
    int type;
    union {
        expr * expr;
        char * idStr;
        block * block;
    };
    comm * comm1; 
    comm * comm2;
} comm;

typedef struct expr {
    int type;
    union {
        prim* prim;
        expr* expr1;
    };
    expr* expr2;
} expr;

typedef struct exprList {
    int count;
    expr** expr;
} exprList;

typedef struct prim {
    int type;
    union {
        int iValue;
        char cValue;
    };
    char * name;
    union {
        expr* expr;
        exprList * exprs;
    };
} prim;

typedef struct safeMemory {
    int size;
    int maxSize;
    void** p;
} safeMemory;

void memFlush();
program* getAbsTree();
void programCall(declfv * dfv, block * blk);
declfv* declfvCall();
var* allocVar();
var* declVar(var* in, char * id);
vars* allocVars();
vars* assignVar(vars* in, int type, var * new);
func* declFunc(params* params, block* blk);
params* allocParams();
params* addParam(params* in, int type, char* id);
declfv* declfvAssignVar(declfv* in, int type, var* varIds);
declfv* declfvAssignFunc(declfv* in, int type, char * id, func* newFunc);
block* allocBlock(vars* varList, comms * commList);
comms* allocComms();
comms* assignComm(comms* in, comm* new);
comm* newComm(commType type, void* expr1, void* comm1, void* comm2);
expr* newExpr(opType type, expr* expr1, expr* expr2, prim* prim1);
expr* newExprFromID(char* id);
exprList* allocExprs();
exprList* stackExpr(exprList* in, expr* _expr);
prim* newPrim(primType type, char * id, exprList* param, expr* _expr, int num, char c);
void printProg();
