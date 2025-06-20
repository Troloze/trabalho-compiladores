#include "main.h"

int is_error = 0;   /* Irá mudar para 1 em caso de erro no parsing */
FILE * yyin;        /* Variavel utilizada por yylex como arquivo de entrada*/

int main(int argc, char** argv){
     if( argc !=2){
          printf("Uso correto: programaPrincipal arquivo_de_teste\n");
          exit(1);
     }

     yyin=fopen( argv[1], "r" );
     yyparse();
     printf("Parsed.\n");
     freeMemory();
     printf("Parsing memory released.\n");
     if (!is_error) {
          // Árvore Abstrata construída com sucesso.
          printProg();
          printf("Abstract Tree Printed.\n");
     }
     memFlush();
     printf("Abstract Tree's Memory Freed.\n");
     return 0; 
}

// -------------------------------------------------------
// Funções de gerenciamento de memória para o lexer
// -------------------------------------------------------

void ** list = NULL;
int listSize = 0;
int listThreshold = 100;

void loadMemory(void * ptr) {
     if (list == NULL) list = malloc(sizeof(void*) * 100);
     if (listSize == listThreshold - 1) {
          listThreshold += 100;
          list = realloc(list, listThreshold * sizeof(void*));
     } 
     list[listSize++] = ptr;
}

void freeMemory() {
     for (int i = 0; i < listSize; i++) {
          free(list[i]);
     }
     free(list);
}
