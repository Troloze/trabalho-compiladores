#include "main.h"

int is_error = 0;   /* Irá mudar para 1 em caso de erro no parsing */
extern FILE * yyin;        /* Variavel utilizada por yylex como arquivo de entrada*/

int main(int argc, char** argv){
     int err = 0;
     int printStuff = 0;
     if( argc < 2){
          printf("Uso correto: programaPrincipal arquivo_de_teste\n");
          exit(1);
     }
     if (argc >= 3) {
          if (argv[2][1] == 'p') {
               printStuff = 1;
               enableSemanticPrint();
          }
     }
     yyin=fopen( argv[1], "r" );
     if (yyin == NULL) {
          printf("Nao foi possivel abrir o arquivo \"%s\".\n", argv[1]);
          return 0;
     }
     yyparse();
     if (printStuff) printf("Parsed.\n");
     freeMemory();
     if (printStuff) printf("Memoria do lexer liberada.\n");

     if (is_error) {
          memFlush();
          return 0;
     }

     // Árvore Abstrata construída com sucesso.
     if (printStuff) printProg();
     if (printStuff) printf("Arvore abstrata Impressa...\n");
     err = semanticAnalyze(getAbsTree());
     if (err) {
          printf("\n");
          memFlush();
          printf("Memoria da arvore abstrata liberada.\n");
          return 0;
     }
     printf("\nNao houveram problemas semanticos.\n");
     memFlush();
     printf("Memoria da arvore abstrata liberada.\n");
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
