#include <stdio.h>
#include <stdlib.h>
#include "extensible_hash.h"
#include "van_emde_boas.h"
// #include "parser.h"

// int main(int argc, char *argv[]){
//     /* Ponto de partida do programa. Realiza as seguintes etapas:
//      * 1. Valida os argumentos de linha de comando;
//      * 2. Inicializa a PPBST (Partial Persistent Binary Search Tree);
//      * 3. Delega a leitura e execução das operações ao módulo parser;
//      * 4. Libera os recursos ao final. 
//     */
//     // Verifica se o caminho do arquivo de entrada (operações) foi fornecido
//     if (argc != 2) {
//         fprintf(stderr, "Uso: %s <arquivo_de_operacoes.txt>\n", argv[0]);
//         return EXIT_FAILURE;
//     }
//     // Inicializa a PPBST
//     PPersistentBST tree;
//     ppbst_init(&tree);
//     // Lê e executa todas as operações do arquivo de entrada
//     int status = parse_and_run(argv[1], &tree);
//     // Libera toda a memória alocada pela árvore
//     ppbst_destroy(&tree);
//     return (status == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
// }

// Extensible Hashing testing

int main(int argc, char *argv[]){
    ExtensibleHash *hash = eh_create(32);

    printf("=== Teste 1: Tabela vazia ===\n");
    eh_print(hash);
    printf("Buscando valor 5 (retorno esperado -> NULL): %p\n\n", (void*) eh_search(hash, 5));

    printf("=== Teste 2: Inserção das chaves 0..9 === \n");
    for(uint32_t i = 0; i < 10; i++){
        eh_insert(hash, i);
        eh_print(hash);
    }

    printf("\n\n=== Teste 3: Buscas ===\n");
    for(uint32_t i = 0; i < 10; i++){
        VanEmdeBoas *vEB = eh_search(hash, i);
        printf("key=%u -> %s\n", i, vEB != NULL ? "vEB encontrado!" : "NADA encontrado!");
    }
    printf("key=99 -> %s\n\n", eh_search(hash, 99) != NULL ? "vEB encontrado!" : "NADA encontrado!");

    printf("\n=== Teste 4: Inserção de duplicata ===\n");
    VanEmdeBoas *b = eh_search(hash, 3);
    VanEmdeBoas *a = eh_insert(hash, 3);
    printf("Mesma estrutura? %s\n\n", b==a ? "Sim" : "Nao");

    printf("=== Teste 5: Remoção de chaves ===\n");
    for(uint32_t i = 0; i < 9; i++){
        eh_remove(hash, i);
        eh_print(hash);
    }

    printf("\n\n=== Teste 6: Removendo a última chave ===\n");
    eh_remove(hash, 9);
    eh_print(hash);

    eh_destroy(hash);
    return 0;
}
