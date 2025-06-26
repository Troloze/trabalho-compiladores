#include "semantic.h"

char isPrint = 0;
void enableSemanticPrint() {
    isPrint = 1;
}



int analyzePrim(contextStack * stack, prim * p, int * typeOut, func * f);
int analyzeExpr(contextStack * stack, expr * e, int * typeOut, func * f);
int analyzeComm(contextStack * stack, comm * c, func * f);
int analyzeBlock(contextStack * stack, block * b, func * f);

int validateID(contextStack * stack, char * idStr, int line, char isVar, int * typeOut, func * f) {
    stackable * stk = findSymbol(stack, idStr);
    stackable * stkf = NULL;
    if (f) stkf = findSymbol(stack, f->id);
    if (isPrint) printf("Var validation: %s\n", idStr);
    if (!stk) {
        printf("\nERRO SEMANTICO NA LINHA %d:\nsimbolo \"%s\" nao declarado foi referenciado.\n", line, idStr);
        return 1;
    }
    if (stkf) {
        if (stk->currentContext==0 && stk->line > stkf->line) {
            printf("\nERRO SEMANTICO NA LINHA %d:\nsimbolo \"%s\" nao declarado foi referenciado.\n", line, idStr);
            return 1;
        }
    }
    if (isVar && !(stk->type & 1)) return 2;
    if (typeOut) *typeOut = stk->type;
    return 0;
}

int validateFunc(contextStack * stack, prim * p, int line, int * typeOut, func * f) {
    stackable * stk = findSymbol(stack, p->name);
    stackable * stkf = NULL;
    if (f) stkf = findSymbol(stack, f->id);
    int err = 0, type = 0;
    if (isPrint) printf("Func validation: %s", p->name);
    if (!stk) {
        printf("\nERRO SEMANTICO NA LINHA %d:\nFuncao \"%s\" nao declarada foi chamada.\n", line,  p->name);
        return 1;
    }
    if (isPrint) printf(".");
    if (stkf) {
        if (stk->line > stkf->line) {
            printf("\nERRO SEMANTICO NA LINHA %d:\nFuncao \"%s\" nao declarada foi chamada.\n", line,  p->name);
            return 1;
        }
    }
    if (isPrint) printf(".");
    *typeOut = stk->type;
    if (!p->expr) {
        if (stk->param != 0) {
            printf("\nERRO SEMANTICO NA LINHA %d:\nFuncao \"%s\" chamada com um numero incorreto de argumentos.\n", line,  p->name);
            return 1;
        }
        if (isPrint) printf(".\n");
        if (isPrint) printf("No param\n");
        return 0;
    } 
    if (stk->param != p->exprs->count) {
        printf("\nERRO SEMANTICO NA LINHA %d:\nFuncao \"%s\" chamada com um numero incorreto de argumentos.\n", line,  p->name);
        return 1;
    }
    if (isPrint) printf(".\n");
    if (isPrint) printf("Param validation:\n");
    for (int i = 0; i < stk->param; i++) {
        expr * cExpr = p->exprs->expr[i];
        stackable * cPar = &stk[i + 1];
        err = analyzeExpr(stack, cExpr, &type, f);
        if (err) return err;
        if (isPrint) printf("%d: %d %s - %d\n", i, cPar->type, cPar->value, type);
        if (cPar->type != type  && type != 2) {
            printf("\nERRO SEMANTICO NA LINHA %d:\nPassagem de parametro de tipo incorreto na chamada da funcao \"%s\".\n", line, p->name);
            return 1;
        }
    }
    return 0;
}

int analyzePrim(contextStack * stack, prim * p, int * typeOut, func * f) {
    if (isPrint) printf("Prim: ");
    switch (p->type) {
    case FUNC_CALL:
        if (isPrint) printf("Func Call: %s\n", p->name);
        return validateFunc(stack, p, p->line, typeOut, f);
    case VAR:
        if (isPrint) printf("Var: %s\n", p->name);
        return validateID(stack, p->name, p->line, 0, typeOut, f);
    case CAR_CONST:
        if (isPrint) printf("Const Car: %d\n", p->cValue);
        *typeOut = 1; // char
        return 0;
    case INT_CONST:
        if (p->iValue >= -128 && p->iValue <=255) {
            if (isPrint) printf("Const \"any\": %d\n", p->iValue);
            *typeOut = 2;   // "any" 
            return 0;
        }
        if (isPrint) printf("Const int: %d\n", p->iValue);
        *typeOut = 0;   // int
        return 0; 
    case PAR_EXPR:
        if (isPrint) printf("Par expr\n", p->cValue);
        return analyzeExpr(stack, p->expr, typeOut, f);
    }
    return 0;
}

int analyzeExpr(contextStack * stack, expr * e, int * typeOut, func * f) {
    int e1Type = 0, e2Type = 0;
    int err = 0;
    *typeOut = 0;
    if (isPrint) printf("Expressao: ");
    switch (e->type)
    {
    case PRIM_OPTYPE:
        if (isPrint) printf("Prim\n");
        err = analyzePrim(stack, e->prim, &e1Type, f);
        *typeOut = e1Type;
        return err;
    case NEG_OPTYPE:
    case NOT_OPTYPE:
        if (isPrint) printf("Neg\\Not\n");
        err = analyzePrim(stack, e->prim, &e1Type, f);
        if (err) return err;
        if (e1Type == 1) {
            printf("\nERRO SEMANTICO NA LINHA %d:\nOperacao nao aceita tipo caractere.\n", e->line);
            return 1;
        }
        return 0;
    case ASS_OPTYPE:
        if (isPrint) printf("Assign\n");
        err = analyzeExpr(stack, e->expr1, &e1Type, f);
        if (err) return err;
        err = analyzeExpr(stack, e->expr2, &e2Type, f);
        if (err) return err;
        if (e1Type != e2Type && e2Type != 2) {
            printf("\nERRO SEMANTICO NA LINHA %d:\nTentativa de atribuicao de tipos incompativeis.\n", e->line);
            return 1;
        }
        *typeOut = e1Type;
        return 0;
    case EQ_OPTYPE:
    case NEQ_OPTYPE:
    case LE_OPTYPE:
    case LT_OPTYPE:
    case GE_OPTYPE:
    case GT_OPTYPE:
        if (isPrint) printf("Operacao comparativa\n");
        err = analyzeExpr(stack, e->expr1, &e1Type, f);
        if (err) return err;
        err = analyzeExpr(stack, e->expr2, &e2Type, f);
        if (err) return err;
        if (e1Type != e2Type && !(e1Type == 2 || e2Type == 2)) {
            printf("\nERRO SEMANTICO NA LINHA %d:\nTentativa de comparacao de tipos incompativeis.\n", e->line);
            return 1;
        }
        *typeOut = 0;
        return 0;
    default:
        if (isPrint) printf("Default: %d\n", e->type);
        err = analyzeExpr(stack, e->expr1, &e1Type, f);
        if (err) return err;
        err = analyzeExpr(stack, e->expr2, &e2Type, f);
        if (err) return err;
        if (e1Type == 1 || e2Type == 1) {
            printf("\nERRO SEMANTICO NA LINHA %d:\nOperacoes aritimetica ou logica nao permitem tipo caractere.\n", e->line);
            return 1;
        }
        return 0;
    }
    return 0;
}

int analyzeComm(contextStack * stack, comm * c, func * f) {
    int err = 0;
    int type = 0;
    if (isPrint) printf("Comando: ");
    switch (c->type)
    {
    case BASE_COMM:
        if (isPrint) printf("Base\n");
        return analyzeExpr(stack, c->expr, &type, f);
    case RETURN_COMM:
        if (isPrint) printf("Return\n");
        err = analyzeExpr(stack, c->expr, &type, f);
        if (f == NULL && !err && (type == 0 || type ==2)) return 0; // Retorno no bloco principal do programa
        if (!err && type != f->type && type != 2) {
            printf("\nERRO SEMANTICO NA LINHA %d:\nExpressao de retorno nao e do mesmo tipo da funcao.\n", c->line);
            return 1;
        }
        return err;
    case READ_COMM:
        if (isPrint) printf("Read\n");
        err = validateID(stack, c->idStr, c->line, 1, NULL, f);
        if (err == 2) printf("\nERRO SEMANTICO NA LINHA %d:\nsimbolo \"%s\" referenciado em leia nao e variavel ou parametro.\n", c->line, c->idStr);
        return err;
    case WRITE_EXPR_COMM:
        if (isPrint) printf("Write Expr\n");
        return analyzeExpr(stack, c->expr, &type, f);
    case IF_COMM:
    case WHILE_COMM:
        if (isPrint) printf("If ou While\n");
        err = analyzeExpr(stack, c->expr, &type, f);
        if (err) return err;
        return analyzeComm(stack, c->comm1, f);
    case IF_ELSE_COMM:
        if (isPrint) printf("If else\n");
        err = analyzeExpr(stack, c->expr, &type, f);
        if (err) return err;
        err = analyzeComm(stack, c->comm1, f);
        if (err) return err;
        return analyzeComm(stack, c->comm2, f);
        break;
    case BLOCK_COMM:
        if (isPrint) printf("Bloco\n");
        return analyzeBlock(stack, c->block, f);
    default:
        if (isPrint) printf("Outro\n");
    }
    return 0;
}

int analyzeBlock(contextStack * stack, block * b, func * f) {
    vars * bvars = b->vars;
    comms * bcomms = b->comms;
    appendContext(stack);
    if (isPrint) printf("Analise de bloco:\n");
    if (isPrint) printf("%d groupos de variaveis locais.\n", bvars->count);
    // Análise de variáveis
    for (int i = bvars->count - 1; i >= 0; i--) {
        var * cvar = bvars->var[i];
        if (isPrint) printf("%d do tipo: %d\n", cvar->count, cvar->type);
        for (int j = cvar->count - 1; j >= 0; j--) {
            if (isPrint) printf("Nome: %s\n", cvar->ids[j]);
            stackable * cSym = findSymbol(stack, cvar->ids[j]);
            if (cSym) {
                if (cSym->currentContext == stack->currentContext) {
                    printf("\nERRO SEMANTICO NA LINHA %d:\nVariavel local declarada com simbolo \"%s\" ja existente no escopo atual.\n", cvar->line, cvar->ids[j]);
                    return 1;
                }
            }
            appendVar(stack, cvar->ids[j], cvar->type, cvar->line);
        }
    }
    if (isPrint) printf("Comandos.\n");
    // Análise de comandos
    for (int i = bcomms->count - 1; i >= 0; i--) {
        // Comandos estão carregados de trás para frente.
        comm * ccomm = bcomms->comm[i];
        if (analyzeComm(stack, ccomm, f)) return 1;
        if (isPrint) printf("End of comm %d.\n", bcomms->count - i - 1);
    }
    popContext(stack);
    if (isPrint) printf("End of Block\n\n");
    return 0;
}

int analyzeFunction(contextStack * stack, func * f) {
    block * fblock = f->block;
    vars * bvars = fblock->vars;
    stackable * cfunc = findSymbol(stack, f->id);
    if (isPrint) printf("Analise aprofundada da funcao: %s, %d vars\n", f->id, bvars->count);
    // Verificar se variáveis locais não tem mesmo nome de parametros
    for (int i = bvars->count - 1; i >= 0 ; i--) {
        var * cvar = bvars->var[i];
        if (isPrint) printf("var: %d\n", cvar->count);
        for (int j = cvar->count - 1; j >= 0; j--) {
            if (isPrint) printf("Variavel: %s, %d.\n", cvar->ids[j], cvar->type);
            stackable * cStk = findSymbol(stack, cvar->ids[j]);
            if (cStk == NULL) continue;
            if (cStk->stackableType != STACKTYPE_PARAM) continue;
            if (cStk->funcRef->id == cfunc->id) {
                printf("\nERRO SEMANTICO NA LINHA %d:\nVariavel local declarada com simbolo \"%s\" ja existente nos parametros da funcao.\n", cvar->line, cvar->ids[j]);
                return 1;
            } 
        }
    }
    return analyzeBlock(stack, fblock, f);
}

int globalVarShallow(contextStack * stack, var * v) {
    for (int i = 0; i < v->count; i++) {
        if (findSymbol(stack, v->ids[i])) {
            printf("\nERRO SEMANTICO NA LINHA %d:\nVariavel declarada com simbolo \"%s\" ja existente no escopo.\n", v->line, v->ids[i]);
            return 1;
        }
        if (isPrint) printf("Variavel: %s de tipo %d\n", v->ids[i], v->type);
        appendVar(stack, v->ids[i], v->type, v->line);       
    }
    return 0;
}

int functionShallow(contextStack * stack, func * f) {
    if (findSymbol(stack, f->id)) {
        printf("\nERRO SEMANTICO NA LINHA %d:\nFuncao declarada com simbolo \"%s\" ja existente no escopo.\n", f->line, f->id);
        return 1;
    }

    if (!f->params) {
        if (isPrint) printf("Funcao: %s, 0 params.\n.\n", f->id);
        appendFunc(stack, f->id, 0, f->type, f->line);
        return 0;
    }

    
    if (isPrint) printf("Funcao: %s, %d params.\n", f->id, f->params->count);
    stackable * cfunc = appendFunc(stack, f->id, f->params->count, f->type, f->line);
    for (int i = 0; i < f->params->count; i++) {
        param *cparam = f->params->param[i];
        if (isPrint) printf("Parametro: %s - %d\n", cparam->id, cparam->type);
        stackable * cSym = findSymbol(stack, cparam->id);
        if (cSym) {
            if (cSym->funcRef == cfunc) {
                printf("\nERRO SEMANTICO NA LINHA %d:\nSimbolo \"%s\" utilizado como nome de parametro mais de uma vez em uma declaracao de funcao.\n", f->line, cparam->id);
                return 1;
            }
        }
        appendParam(stack, cparam->id, f->params->count - i - 1, cparam->type, cfunc, cparam->line);
    }
    if (isPrint) printf(".\n");
    return 0;
}

int semanticAnalyze(program * prog) {
    contextStack * stack = createStack();
    appendContext(stack);
    declfv * dfv = prog->declfv;
    if (isPrint) printf("\nAnalise semantica:\n\n");
    if (isPrint) printf("Variaveis Globais (%d)\n", dfv->vars->count);
    for (int i = 0; i < dfv->vars->count; i++) {
        if (globalVarShallow(stack, dfv->vars->var[i])) {
            printStack(stack);
            freeStack(stack);
            return 1;
        }
    }
    if (isPrint) printf("Funcoes (%d)\n", dfv->funcs->count);
    for (int i = 0; i < dfv->funcs->count; i++) {
        if (functionShallow(stack, dfv->funcs->func[i])) {
            printStack(stack);
            freeStack(stack);
            return 1;
        }
    }

    if (isPrint) printf("Analise aprofundada das funcoes.\n");
    for (int i = 0; i < dfv->funcs->count; i++) {
        if (analyzeFunction(stack, dfv->funcs->func[i])) {
            printStack(stack);
            freeStack(stack);
            return 1;
        }
    }

    if (analyzeBlock(stack, prog->block, NULL)) {
        printStack(stack);
        freeStack(stack);
        return 1;
    }

    freeStack(stack);
    return 0;
}