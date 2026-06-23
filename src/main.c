#include <stdio.h>
#include <stdlib.h>
#include "van_emde_boas.h"
#include "parser.h"

 int main(int argc, char *argv[]){
     /* Ponto de partida do programa. Realiza as seguintes etapas:
      * 1. Valida os argumentos de linha de comando;
      * 2. Inicializa a vEB (van Emde Boas);
      * 3. Delega a leitura e execução das operações ao módulo parser;
      * 4. Libera os recursos ao final. 
     */
     // Verifica se o caminho do arquivo de entrada (operações) foi fornecido
     if (argc != 2) {
         fprintf(stderr, "Uso: %s <arquivo_de_operacoes.txt>\n", argv[0]);
         return EXIT_FAILURE;
     }
     // Inicializa a vEB
     VanEmdeBoas *vEB = veb_create(32);
     // Lê e executa todas as operações do arquivo de entrada
     int status = parse_and_run(argv[1], vEB);
     // Libera toda a memória alocada pela estrutura
     veb_destroy(vEB);
     return (status == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
 }