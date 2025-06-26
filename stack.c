#include "stack.h"


unsigned int newID(){
    static unsigned int currentID = 1;
    return currentID++;
}

/* If stack is full, increase it */
int expandStack(contextStack* in) {
    if (in->size < in->allocSize - 1) return 0; 
    in->allocSize += STACK_INCREASE_VALUE;
    in->elements = realloc(in->elements, sizeof(stackable) * in->allocSize);
    if (in->elements == NULL) 
        return 1;
    return 0;
}

/* If stack is too empty, reduce it */
int reduceStack(contextStack* in) {
    if (in->allocSize == 20) return 0;
    if (in->size >= in->allocSize - STACK_INCREASE_VALUE - 1) return 0;

    in->allocSize -= STACK_INCREASE_VALUE;

    in->elements = realloc(in->elements, sizeof(stackable) * in->allocSize);
    if (in->elements == NULL) 
        return 1;
    return 0;
}

contextStack* createStack() {
    contextStack* ret = malloc(sizeof(contextStack));
    ret->allocSize = STACK_DEFAULT_ALLOC_SIZE;
    ret->elements = malloc(sizeof(stackable) * ret->allocSize);
    ret->size = 0;
    return ret;
}

void freeStack(contextStack* in) {
    free(in->elements);
    free(in);
}

stackable* appendContext(contextStack* in) {
    expandStack(in);
    stackable* newContext = &in->elements[in->size];
    newContext->id = newID();
    newContext->stackableType = STACKTYPE_CONTEXT;
    newContext->value[0] = '\0';
    newContext->param = in->currentContext;
    newContext->type = -1;
    newContext->funcRef = NULL;
    newContext->pos = in->size;
    in->currentContext = in->size;
    in->size++;
    return &in->elements[in->size - 1];
}

void popContext(contextStack* in) {
    in->size = in->currentContext;
    in->currentContext = in->elements[in->size].param;
    reduceStack(in);
}

stackable* appendFunc(contextStack* in, const char* funcName, int paramCount, int retType, int line) {
    expandStack(in);
    stackable* newFunc = &in->elements[in->size];
    newFunc->id = newID();
    newFunc->stackableType = STACKTYPE_FUNC;
    strcpy(newFunc->value, funcName);
    newFunc->param = paramCount;
    newFunc->type = retType;
    newFunc->funcRef = NULL;
    newFunc->pos = in->size;
    newFunc->line = line;
    newFunc->currentContext = in->currentContext;
    in->size++;
    return newFunc;
}

stackable* appendVar(contextStack* in, const char* varName, int varType, int line) {
    expandStack(in);
    stackable* newVar = &in->elements[in->size];
    newVar->id = newID();
    newVar->stackableType = STACKTYPE_VAR;
    strcpy(newVar->value, varName);
    newVar->param = -1;
    newVar->type = varType;
    newVar->funcRef = NULL;
    newVar->pos = in->size;
    newVar->line = line;
    newVar->currentContext = in->currentContext;
    in->size++;
    return newVar;
}

stackable* appendParam(contextStack* in, const char* paramName, int paramPos, int paramType, stackable* funcRef, int line) {
    expandStack(in);
    stackable* newParam = &in->elements[in->size];
    newParam->id = newID();
    newParam->stackableType = STACKTYPE_PARAM;
    strcpy(newParam->value, paramName);
    newParam->funcRef = funcRef;
    newParam->type = paramType;
    newParam->param = paramPos;
    newParam->pos = in->size;
    newParam->line = line;
    newParam->currentContext = in->currentContext;
    in->size++;
    return newParam;
}

void printStack(contextStack* in) {
    int i, j, contextCount = 0;
    stackable * current;
    printf("Stack size:%d\n", in->size);
    for (i = 0; i < in->size; i++) {
        current = &(in->elements[i]);
        for (j = 0; j < contextCount; j++) printf("  ");
        printf("%d:\t", i);
        switch (current->stackableType) {
        case STACKTYPE_CONTEXT:
            printf("Context Start:\n");
            contextCount++;
            break;
        case STACKTYPE_FUNC:
            printf("Func: %s, Param Count: %d, return type: %d\n", current->value, current->param, current->type);
            break;
        case STACKTYPE_VAR:
            printf("Var: %s, type: %d\n", current->value, current->type);
            break;
        case STACKTYPE_PARAM:
            printf("Param: %s, ParentFunc: %s, ParamPos: %d, param type: %d\n", current->value, current->funcRef->value, current->param, current->type);
            break;
        default:
            break;
        }
    }
}

stackable* findSymbol(contextStack* in, char* symbol) {
    int i;
    if (symbol == NULL) return NULL;
    stackable * current = NULL;
    /* Varre a pilha procurando o símbolo */
    for (i = in->size; i > 0; i--) {
        current = &(in->elements[i]);
        if (current->stackableType == STACKTYPE_CONTEXT)
            continue;
        if (!strcmp(symbol, current->value)) 
            return current;
    } 
    return NULL;
}