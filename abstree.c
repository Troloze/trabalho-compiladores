#include "abstree.h"

// -------------------------------------------------------
// Gerenciador de memória da árvore abstrata
// (Para evitar vazamentos em caso de erro no parsing.)
// -------------------------------------------------------

safeMemory * mem = NULL;

void memFlush() {
    if (mem == NULL) return;
    printf("%d memory points being freed.\n", mem->size);
    for (int i = 0; i < mem->size; i++) {
        free(mem->p[i]);
    }
    free(mem->p);
    free(mem);
    mem = NULL;
}

void memInit() {
    mem = malloc(sizeof(safeMemory));
    mem->p = malloc(sizeof(void*) * 100);
    mem->maxSize = 100;
    mem->size = 0;
}

void addToMem(void* new) {
    if (mem == NULL)  memInit();
    if (mem->size + 1 == mem->maxSize) {
        mem->maxSize += 100;
        mem->p = realloc(mem->p, sizeof(void*) * mem->maxSize);
    }
    mem->p[mem->size++] = new;
}

void replaceFromMem(void* old, void* new) {
    if (mem == NULL)  memInit();
    for (int i = 0; i < mem->size; i++) {
        if (mem->p[i] == old) {
            mem->p[i] = new;
            break;
        }
    }
}

void* safeMalloc(size_t size) {
    void* ret = malloc(size);
    addToMem(ret);
    return ret;
}

void* safeCalloc(size_t count, size_t size) {
    void* ret = calloc(count, size);
    addToMem(ret);
    return ret;
}

void* safeRealloc(void* ptr, size_t size) {
    void* ret = realloc(ptr, size);
    replaceFromMem(ptr, ret);
    return ret;
}

// -------------------------------------------------------
// Funções para a montagem da árvore abstrata
// -------------------------------------------------------

static program * prog;

program* getAbsTree() {
    return prog;
}

void programCall(declfv * dfv, block * blk) {
    prog = safeMalloc(sizeof(program));
    prog->declfv = dfv;
    prog->block = blk;
}

declfv* declfvCall() {
    declfv* ret = safeMalloc(sizeof(declfv));    
    ret->funcs = safeMalloc(sizeof(funcs));
    ret->funcs->count = 0;
    ret->funcs->func = safeMalloc(0);
    ret->vars = safeMalloc(sizeof(vars));
    ret->vars->count = 0;
    ret->vars->var = safeMalloc(0);
    return ret;
}

vars* allocVars() {
    vars* ret = safeMalloc(sizeof(vars));
    ret->count = 0;
    ret->var = safeMalloc(0);
    return ret;
}

vars* assignVar(vars* in, int type, var * new) {
    new->type = type;
    in->var = safeRealloc(in->var, sizeof(var*) * (in->count + 1));
    in->var[in->count++] = new;
    return in;
}

var* allocVar() {
    var* ret = safeMalloc(sizeof(var));
    ret->ids = safeMalloc(0);
    ret->count = 0;
    return ret;
}

var* declVar(var* in, char * id, int line) {
    char * newId = safeCalloc(strlen(id) + 1, sizeof(char));
    strcpy(newId, id);
    in->line = line;
    in->ids = safeRealloc(in->ids, sizeof(char*) * (in->count + 1));
    in->ids[in->count++] = newId;
    return in;
}

params* allocParams() {
    params* ret = safeMalloc(sizeof(params));
    ret->count = 0;
    ret->param = safeMalloc(0);
    return ret;
}

params* addParam(params* in, int type, char* id, int line) {
    char* newStr = safeCalloc(strlen(id) + 1, sizeof(char));
    strcpy(newStr, id);
    in->param = safeRealloc(in->param, sizeof(param*) * (in->count + 1));
    param* newPar = safeMalloc(sizeof(param));
    newPar->line = line;
    newPar->id = newStr;
    newPar->type = type;
    in->param[in->count++] = newPar;
    return in;
}

func* declFunc(params* params, block* blk, int line) {
    func* ret = safeMalloc(sizeof(func));
    ret->line = line;
    ret->params = params;
    ret->block = blk;
    return ret;
}

declfv* declfvAssignVar(declfv* in, int type, var* varIds) {
    varIds->type = type;
    in->vars->var = safeRealloc(in->vars->var, sizeof(var*) * (in->vars->count + 1));
    in->vars->var[in->vars->count++] = varIds;
    return in;
}

declfv* declfvAssignFunc(declfv* in, int type, char * id, func* newFunc) {
    char* newId = safeCalloc(strlen(id) + 1, sizeof(char));
    newFunc->id = newId;
    newFunc->type = type;
    strcpy(newId, id);
    in->funcs->func = safeRealloc(in->funcs->func, sizeof(func*) * (in->funcs->count + 1));
    in->funcs->func[in->funcs->count++] = newFunc;
    return in;
}

block* allocBlock(vars* varList, comms * commList) {
    block * ret = safeMalloc(sizeof(block));
    ret->comms = commList;
    ret->vars = varList;
    return ret;
}

comms* allocComms() {
    comms* ret = safeMalloc(sizeof(comms));
    ret->count = 0;
    ret->comm = safeMalloc(0);
    return ret;
}

comms* assignComm(comms* in, comm* new) {
    if (new == NULL) return in;
    in->comm = safeRealloc(in->comm, sizeof(comm*) * (in->count + 1));
    in->comm[in->count++] = new;
    return in;
}

comm* newComm(commType type, void* expr1, void* comm1, void* comm2, int line) {
    comm* ret = safeMalloc(sizeof(comm));
    char * newStr;
    ret->type = type;
    ret->comm1 = comm1;
    ret->comm2 = comm2;
    ret->line = line;
    
    switch (type)
    {
    case WRITE_STR_COMM:
    case READ_COMM:
        newStr = safeCalloc(strlen((char*) expr1) + 1, sizeof(char));
        strcpy(newStr, (char*) expr1);
        ret->idStr = newStr;
        break; 
    case BLOCK_COMM:
        ret->block = (block*) expr1;
        break;
    default:
        ret->expr = (expr*) expr1;
        break;
    }
    return ret;
}

expr* newExpr(opType type, expr* expr1, expr* expr2, prim* prim1, int line) {
    expr* ret = safeMalloc(sizeof(expr));
    ret->type = type;
    ret->line = line;
    if (prim1) ret->prim = prim1;
    else ret->expr1 = expr1;
    ret->expr2 = expr2;
    return ret;
}

expr* newExprFromID(char* id, int line) {
    prim* pr = newPrim(VAR, id, NULL, NULL, 0, 0, line);
    return newExpr(PRIM_OPTYPE, NULL, NULL, pr, line);
}

exprList* allocExprs(int line) {
    exprList* ret = safeMalloc(sizeof(exprList));
    ret->count = 0;
    ret->line = line;
    ret->expr = safeMalloc(0);
    return ret;
}

exprList* stackExpr(exprList* in, expr* _expr) {
    in->expr = safeRealloc(in->expr, sizeof(expr*) * (in->count + 1));
    in->expr[in->count++] = _expr;
    return in;
}

prim* newPrim(primType type, char * id, exprList* param, expr* _expr, int num, char c, int line) {
    prim* ret = safeMalloc(sizeof(prim));
    char * newStr;
    ret->type = type;
    ret->line = line;
    switch (type)
    {
    case FUNC_CALL:
        ret->exprs = param;
    case VAR:
        newStr = safeCalloc(strlen(id) + 1, sizeof(char));
        strcpy(newStr, id);
        ret->name = newStr;
        break;
    case CAR_CONST:
        ret->cValue = c;
        break;
    case INT_CONST:
        ret->iValue = num;
        break;
    case PAR_EXPR:
        ret->expr = _expr;
        break;
    }
    return ret;
}


// -------------------------------------------------------
// Funções de Debug
// -------------------------------------------------------

void printVar(vars* varlist);
void printBlock(block* blk);
void printComm(comm* cmd);
void printExpr(expr* xpr);
void printPrim(prim* prm);
void printExprList(exprList* exL);

void printVar(vars* varlist) {
    if (varlist->count == 0) {
        printf("Empty.\n");
        return;
    }
    for (int i = 0; i < varlist->count; i++) {
        var* cvar = varlist->var[i];
        printf("%s: ", (cvar->type)?("Car"):("Int"));
        for (int j = 0; j < cvar->count; j++) {
            printf("%s", cvar->ids[j]);
            if (j != cvar->count - 1) printf(", "); 
            else printf("\n");
            if ((j+1)%20 == 0) printf("\n     ");
        }
    }
}

void printBlock(block* blk) {
    printf("Block Vars:\n");
    printVar(blk->vars);
    printf("Block Commands:\n");
    comms* cms = blk->comms;
    if (cms->count == 0) {
        printf("Empty.\n");
        return;
    }
    for (int i = 0; i < cms->count; i++) printComm(cms->comm[i]);
}

void printComm(comm* cmd) {
    switch(cmd->type) {
        case BASE_COMM:
            printf("Base Expression Command:\n");
            printExpr(cmd->expr);
            break;
        case RETURN_COMM:
            printf("Return Command:\n");
            printExpr(cmd->expr);
            break;
        case READ_COMM:
            printf("Read Command to var: %s\n", cmd->idStr);
            break;
        case WRITE_EXPR_COMM:
            printf("Write Expr Command:\n");
            printExpr(cmd->expr);
            break;
        case WRITE_STR_COMM:
            printf("Write Str Command: %s\n", cmd->idStr);
            break;
        case NEWLINE_COMM:
            printf("Newline Command:\n");
            break;
        case IF_COMM:
            printf("If Command:\n");
            printExpr(cmd->expr);
            printComm(cmd->comm1);
            break;
        case IF_ELSE_COMM:
            printf("If Else Command:\n");
            printExpr(cmd->expr);
            printComm(cmd->comm1);
            printComm(cmd->comm2);
            break;
        case WHILE_COMM:
            printf("While Command:\n");
            printExpr(cmd->expr);
            printComm(cmd->comm1);
            break;
        case BLOCK_COMM:
            printf("Block Command:\n");
            printBlock(cmd->block);
            break;
    }
}

void printExpr(expr* xpr) {
    printf("Expr - ");
    switch (xpr->type)
    {
    case ASS_OPTYPE:
        printf("Assign:\n");
        printExpr(xpr->expr1);
        printExpr(xpr->expr2);
        break;
    case OR_OPTYPE:
        printf("Or:\n");
        printExpr(xpr->expr1);
        printExpr(xpr->expr2);
        break;
    case AND_OPTYPE:
        printf("And:\n");
        printExpr(xpr->expr1);
        printExpr(xpr->expr2);
        break;
    case EQ_OPTYPE:
        printf("Equal:\n");
        printExpr(xpr->expr1);
        printExpr(xpr->expr2);
        break;
    case NEQ_OPTYPE:
        printf("Not Equal:\n");
        printExpr(xpr->expr1);
        printExpr(xpr->expr2);
        break;
    case LT_OPTYPE:
        printf("Less Than:\n");
        printExpr(xpr->expr1);
        printExpr(xpr->expr2);
        break;
    case LE_OPTYPE:
        printf("Less Equal:\n");
        printExpr(xpr->expr1);
        printExpr(xpr->expr2);
        break;
    case GT_OPTYPE:
        printf("Greater Than:\n");
        printExpr(xpr->expr1);
        printExpr(xpr->expr2);
        break;
    case GE_OPTYPE:
        printf("Greater Equal:\n");
        printExpr(xpr->expr1);
        printExpr(xpr->expr2);
        break;
    case ADD_OPTYPE:
        printf("Add:\n");
        printExpr(xpr->expr1);
        printExpr(xpr->expr2);
        break;
    case SUB_OPTYPE:
        printf("Sub:\n");
        printExpr(xpr->expr1);
        printExpr(xpr->expr2);
        break;
    case MULT_OPTYPE:
        printf("Mult:\n");
        printExpr(xpr->expr1);
        printExpr(xpr->expr2);
        break;
    case DIV_OPTYPE:
        printf("Div:\n");
        printExpr(xpr->expr1);
        printExpr(xpr->expr2);
        break;
    case NEG_OPTYPE:
        printf("Neg:\n");
        printPrim(xpr->prim);
        break;
    case NOT_OPTYPE:
        printf("Not:\n");
        printPrim(xpr->prim);
        break;
    case PRIM_OPTYPE:
        printf("Prim:\n");
        printPrim(xpr->prim);
        break;
    }
}

void printPrim(prim* prm) {
    printf("Prim: ");
    switch (prm->type)
    {
    case FUNC_CALL:
        printf("FUNC_CALL ");
        printf("%s:\n", prm->name);
        printExprList(prm->exprs);
        break;
    case VAR:
        printf("VAR: %s\n", prm->name);
        break;
    case CAR_CONST:
        printf("CAR_CONST: %d\n", prm->cValue);
        break;
    case INT_CONST:
        printf("INT_CONST: %d\n", prm->iValue);
        break;
    case PAR_EXPR:
        printf("PARENTHESIS_EXPRESSION:\n");
        printExpr(prm->expr);
        break; 
    }
}

void printExprList(exprList* exL) {
    if (!exL) {
        printf("No params.\n");
        return;
    }
    printf("Param list:\n");
    for (int i = 0; i < exL->count; i++) {
        expr* cexpr = exL->expr[i];
        printExpr(cexpr);
    }
    printf("End of param list.\n");
}

void printProg() {
    declfv* fv = prog->declfv;
    vars* gvars = fv->vars;
    funcs* gfuncs = fv->funcs;
    printf("Global Vars:\n");
    printVar(gvars);
    printf("\nFunctions:\n");
    if (gfuncs->count) {
        for (int i = 0; i < gfuncs->count; i++) {
            func* cfunc = gfuncs->func[i];
            printf("Function %s %s\n", (cfunc->type)?("Car"):("Int"), cfunc->id);
            if (cfunc->params) {
                printf("Params %d:\n", cfunc->params->count);
                for (int j = 0; j < cfunc->params->count; j++) {
                    param* cparam = cfunc->params->param[j];
                    printf("%s: %s", (cparam->type)?("Car"):("Int"), cparam->id);
                    if (j == cfunc->params->count - 1) printf("\n");
                    else printf(", ");
                }
            }
            block* blk = cfunc->block;
            printBlock(blk);
            printf("\n");
        }
    }
    else printf("Empty.\n");
    printf("\nProgram Start:\n");
    printBlock(prog->block);
}

/*

// -------------------------------------------------------
// Funções de limpesa de memória
// Obs: Não funciona em caso de erro no parsing
// -------------------------------------------------------

void freeVar(vars* varlist);
void freeFunc(funcs* funcList);
void freeBlock(block* blk);
void freeComm(comm* cmd);
void freeExpr(expr* xpr);
void freePrim(prim* prm);
void freeExprList(exprList* exL);

void freeVar(vars* varlist) {
    for (int i = 0; i < varlist->count; i++) {
        var* cvar = varlist->var[i];
        for (int j = 0; j < cvar->count; j++) 
            free(cvar->ids[j]);
        free(cvar->ids);
        free(cvar);
    }
    free(varlist->var);
    free(varlist);
}

void freeFunc(funcs* funcList) {
    for (int i = 0; i < funcList->count; i++) {
        func* cfunc = funcList->func[i];
        params* cpars = cfunc->params;
        if (cpars) {
            for (int j = 0; j < cpars->count; j++) {
                param* cpar = cpars->param[j];
                free(cpar->id);
                free(cpar);
            }
            free(cpars);
        }
        freeBlock(cfunc->block);
        free(cfunc->id);
        free(cfunc);
    }
    free(funcList);
}

void freeBlock(block* blk) {
    freeVar(blk->vars);
    for (int i = 0; i < blk->comms->count; i++) 
        free(blk->comms->comm[i]);
    free(blk->comms);
    free(blk);
}

void freeComm(comm* cmd) {
    switch (cmd->type) {
        case BLOCK_COMM:
            freeBlock(cmd->block);
            break;
        case READ_COMM:
        case WRITE_STR_COMM:
            free(cmd->idStr);
            break;
        default:
            if (cmd->expr) freeExpr(cmd->expr);
            break;
    }
    if (cmd->comm1) freeComm(cmd->comm1);
    if (cmd->comm2) freeComm(cmd->comm2);
    free(cmd);
}

void freeExpr(expr* xpr) {
    switch (xpr->type) {
        case PRIM_OPTYPE:
        case NEG_OPTYPE:
        case NOT_OPTYPE:
            freePrim(xpr->prim);
            break;
        default:
            freeExpr(xpr->expr1);
    }    

    if (xpr->expr2) freeExpr(xpr->expr2);
    free(xpr);
}

void freePrim(prim* prm) {
    switch (prm->type) {
        case FUNC_CALL:
            freeExprList(prm->exprs);
        case VAR:
            free(prm->name);
            break;
        case PAR_EXPR:
            freeExpr(prm->expr);
            break;
    }
    free(prm);
}

void freeExprList(exprList* exL) {
    if (!exL) return;
    for (int i = 0; i < exL->count; i++) {
        freeExpr(exL->expr[i]);
    }
    free(exL);
}

void freeProg() {
    freeVar(prog->declfv->vars);
    freeFunc(prog->declfv->funcs);
    free(prog->declfv);
    freeBlock(prog->block);
    free(prog);
    return;
}

*/