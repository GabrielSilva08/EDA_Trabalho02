#include <stdio.h>
#include <stdbool.h>
#include "van_emde_boas.h"

int main(void){
    
    VanEmdeBoas *tree = veb_create(32);

    veb_insert(tree, 102);
    veb_insert(tree, 268);
    veb_insert(tree, 322);
    veb_insert(tree, 14756);
    veb_insert(tree, 456);
    veb_insert(tree, 728);

    int inf = 0;
    uint32_t out = veb_successor(tree, 300, &inf);

    if (inf)
        printf("Sucessor de 300: INF (nao existe)\n");
    else
        printf("Sucessor de 300: %u\n", out);

    int inf1 = 0;
    out = veb_predecessor(tree, 300, &inf1);

    // printf("Valor de out: %u e do inf1: %d\n", out, inf1);
    if (inf1)
        printf("Predecessor de 300: -INF (nao existe)\n");
    else
        printf("Predecessor de 300: %u\n", out);

    // Caso de borda: nada menor que o mínimo da árvore
    int inf2 = 0;
    out = veb_predecessor(tree, 102, &inf2);

    if (inf2)
        printf("Predecessor de 102: -INF (nao existe)\n");
    else
        printf("Predecessor de 102: %u\n", out);

    
    veb_remove(tree, 268);
    printf("Removido o valor 268.\n");

    int inf3 = 0;
    out = veb_successor(tree, 102, &inf3);

    if (inf3)
        printf("Sucessor de 102 apos remover 268: INF\n");
    else
        printf("Sucessor de 102 apos remover 268: %u\n", out);
        

    // Imprime a estrutura inteira (Min + clusters não-nulo)
    veb_print(tree);
    printf("\n");

    veb_destroy(tree);
    return 0;
}