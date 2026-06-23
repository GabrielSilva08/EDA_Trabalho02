#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "van_emde_boas.h"

// Tamanho máximo de uma linha no arquivo de entrada
#define MAX_LINE 256
 
int parse_and_run(char *filename, VanEmdeBoas *vEB){
    char line[MAX_LINE];
    FILE *fp = fopen(filename, "r");
 
    if (!fp){
        fprintf(stderr, "Erro: não foi possível abrir o arquivo \"%s\".\n", filename);
        return EXIT_FAILURE;
    }
 
    while (fgets(line, sizeof(line), fp)){
        // Remoção do caractere '\n', se houver
        line[strcspn(line, "\n")] = '\0';
        
        // INC <valor>
        if (strncmp(line, "INC", 3) == 0){
            int value = atoi(line + 4);
            veb_insert(vEB, value);

        // REM <valor>
        } else if (strncmp(line, "REM", 3) == 0){
            int value = atoi(line + 4);
            veb_remove(vEB, value);

        // SUC <valor>
        } else if (strncmp(line, "SUC", 3) == 0){
            int value = atoi(line+4), inf = 0;
            // Impressão do nome da operação
            printf("SUC %d\n", value);
            uint32_t succ = veb_successor(vEB, value, &inf);
            printf("%u\n", succ);
        
        // PRE <valor>
        } else if (strncmp(line, "PRE", 3) == 0){
            int value = atoi(line+4), inf = 0;
            // Impressão do nome da operação
            printf("PRE %d\n", value);
            uint32_t pred = veb_predecessor(vEB, value, &inf);
            printf("%u\n", pred);

        // IMP <versão>
        } else if (strncmp(line, "IMP", 3) == 0) {
            printf("IMP\n");
            veb_print(vEB);
            printf("\n");
        }
        // Operações não reconhecidas são ignoradas
    }
    fclose(fp);
    return EXIT_SUCCESS;
 }
