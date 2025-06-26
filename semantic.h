#ifndef ATT_COMP_SEMANTIC
#define ATT_COMP_SEMANTIC
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "abstree.h"
#include "stack.h"

void enableSemanticPrint();
int semanticAnalyze(program * prog);
#endif
