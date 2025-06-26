#ifndef ATIVIDADE_STACK
#define ATIVIDADE_STACK

#include "main.h"

#define MAX_STACK_VALUE_LENGTH 64

/* Usando isso aqui para diferenciar os tipos de objetos de pilha */
#define STACKTYPE_CONTEXT   0
#define STACKTYPE_VAR       1
#define STACKTYPE_FUNC      2
#define STACKTYPE_PARAM     3

#define STACK_DEFAULT_ALLOC_SIZE 200
#define STACK_INCREASE_VALUE 200

struct stackable;

typedef struct stackable {
    unsigned int id;
    unsigned char stackableType;
    unsigned int currentContext;
    char value[MAX_STACK_VALUE_LENGTH];         
    unsigned int type;
    unsigned int param;
    unsigned int pos;
    unsigned int line;
    struct stackable* funcRef;
} stackable;

typedef struct contextStack {
    stackable * elements;
    int size;
    int allocSize;
    int currentContext;
} contextStack;

contextStack* createStack();

void freeStack(contextStack* in);

stackable* appendContext(contextStack* in);

void popContext(contextStack* in);

stackable* appendFunc(contextStack* in, const char* funcName, int paramCount, int retType, int line);

stackable* appendVar(contextStack* in, const char* varName, int varType, int line);

stackable* appendParam(contextStack* in, const char* paramName, int paramPos, int paramType, stackable* funcRef, int line);

stackable* findSymbol(contextStack* in, char* symbol);

void printStack(contextStack * in);

#endif