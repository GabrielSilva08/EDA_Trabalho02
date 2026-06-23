#include <stdio.h>
#include <stdlib.h>
#include "extensible_hash.h"
#include "van_emde_boas.h"

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
        veb_destroy(vEB);
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
    veb_destroy(a);
    veb_destroy(b);
    eh_destroy(hash);
    return 0;
}
