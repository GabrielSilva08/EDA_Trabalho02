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
            uint32_t value = (uint32_t)strtoul(line + 4, NULL, 10);
            veb_insert(vEB, value);

        // REM <valor>
        } else if (strncmp(line, "REM", 3) == 0){
            uint32_t value = (uint32_t)strtoul(line + 4, NULL, 10);
            veb_remove(vEB, value);

        // SUC <valor>
        } else if (strncmp(line, "SUC", 3) == 0){
            uint32_t value = (uint32_t)strtoul(line + 4, NULL, 10);
            int inf = 0;
            uint32_t succ = veb_successor(vEB, value, &inf);
            
            if (inf) printf("SUC %u\n+INF\n", value);
            else printf("SUC %u\n%u\n", value, succ);
                
        
        // PRE <valor>
        } else if (strncmp(line, "PRE", 3) == 0){
            uint32_t value = (uint32_t)strtoul(line + 4, NULL, 10);
            int inf = 0;
            uint32_t pred = veb_predecessor(vEB, value, &inf);
            
            if (inf) printf("PRE %u\n-INF\n", value);
            else printf("PRE %u\n%u\n", value, pred);

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
