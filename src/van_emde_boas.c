#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "van_emde_boas.h"

// Macros auxiliares necessárias para a decomposição de uma chave em coordenadas hierárquicas, x = <c, i>.
#define VEB_HIGH(x, w) ((x) >> ((w) >> 1)) // c = x >> w/2
#define VEB_LOW(x, w) ((x) & ((1 << ((w) >> 1)) - 1)) // i = x & ((1 << w/2) - 1)
#define VEB_INDEX(c, i, w) (((c) << ((w) >> 1)) | (i)) // x = (c << w/2) | i

VanEmdeBoas *veb_create(uint8_t w){
    VanEmdeBoas *vEB = malloc(sizeof(VanEmdeBoas));
    vEB->w = w;
    vEB->is_empty = 1;
    vEB->min = 0;
    vEB->max = 0;
    // Clusters indexados pelos bits superiores (c), onde cada qual tem tamanho w/2.
    vEB->clusters = eh_create(w/2); 
    // Criado sob demanda. Inicialmente, não há cluster não-vazio.
    vEB->summary  = NULL;

    return vEB;
}

void veb_destroy(VanEmdeBoas *vEB){
    if (vEB == NULL) return;

    // Destruição recursiva dos clusters filhos
    // Para cada bucket...
    for (uint32_t i = 0; i < vEB->clusters->hash_size; i++){
        // Para cada entrada...
        for (uint32_t j = 0; j < vEB->clusters->buckets[i].size; j++){
            veb_destroy(vEB->clusters->buckets[i].entries[j].vEB);
        }
    }
    // Liberação da tabela hash utilizada 
    eh_destroy(vEB->clusters);

    // Liberação do resumo
    if (vEB->summary != NULL) veb_destroy(vEB->summary);

    free(vEB);
}

void veb_insert(VanEmdeBoas *vEB, uint32_t x) {
    // Caso base: Estrutura vazia. x se torna min e max
    if (vEB->is_empty){
        vEB->min = x;
        vEB->max = x;
        vEB->is_empty = 0;
        return;
    }

    if (x == vEB->min || x == vEB->max) return;

    // Instanciação do resumo
    if (vEB->summary == NULL) vEB->summary = veb_create(vEB->w/2);
    
    // Se o novo elemento é menor que o menor global, passamos a inserir o antigo menor no lugar do x
    if (x < vEB->min){
        uint32_t temp;
        temp = x;
        x = vEB->min;
        vEB->min = temp;
    }

    // Atualização do max
    if (x > vEB->max) vEB->max = x;

    uint32_t c = VEB_HIGH(x, vEB->w);
    uint32_t i = VEB_LOW(x, vEB->w);
    VanEmdeBoas *child = eh_insert(vEB->clusters, c);
    veb_insert(child, i); // vEB[c] = i

    // Atualização do resumo para incluir novo cluster não-vazio se esse já não foi incluído
    if (child->min == child->max){
        veb_insert(vEB->summary, c);
    }
}

void veb_remove(VanEmdeBoas *vEB, uint32_t x){
    // Caso base: Único elemento
    if (vEB->min == vEB->max && vEB->max == x){
        vEB->is_empty = 1;
        return;
    }

    // Estrutura sem clusters instanciados, basta checar se a nova chave é diferente de min
    if (vEB->clusters->hash_size == 0 && vEB->min != x) return;
    
    // O menor elemento é aquele a ser removido, basta procurar pelo novo mínimo para por em seu lugar e removê-lo da posição original
    if (x == vEB->min){
        uint32_t min_cluster = vEB->summary->min;
        VanEmdeBoas *child = eh_search(vEB->clusters, min_cluster);

        // Novo menor elemento
        vEB->min = VEB_INDEX(min_cluster, child->min, vEB->w);
        x = vEB->min;
    }

    uint32_t c = VEB_HIGH(x, vEB->w);
    uint32_t i  = VEB_LOW (x, vEB->w);

    VanEmdeBoas *child = eh_search(vEB->clusters, c);
    if (child == NULL) return;

    // Remoção do novo menor no cluster
    veb_remove(child, i);
    
    // Cluster se tornou vazio após remoção do elemento.
    if (child->is_empty){
        // Remoção da lista de clusters não-vazios (resumo)
        veb_remove(vEB->summary, c);

        // Resumo se tornou vazio após remoção do cluster
        if (vEB->summary->is_empty) {
            veb_destroy(vEB->summary);
            vEB->summary = NULL;
        }

        // Remoção do cluster da tabela de dispersão
        eh_remove(vEB->clusters, c);
        veb_destroy(child);
    }

    // Se x for o maior valor, basta atualizar o novo maior valor com o valor apontado pelo resumo (max do último cluster vivo)
    if (x == vEB->max){
        if (vEB->summary == NULL){
            vEB->max = vEB->min;
        }else {
            uint32_t last_cluster = vEB->summary->max;
            VanEmdeBoas *last = eh_search(vEB->clusters, last_cluster);
            vEB->max = VEB_INDEX(last_cluster, last->max, vEB->w);
        }
    }
}

uint32_t veb_successor(VanEmdeBoas *vEB, uint32_t x, int* is_infinity) {
    if (vEB->is_empty){
        // return +INF;
        *is_infinity = 1;
        return x;
    }
    // Se x é menor que o mínimo, então o sucessor é o próprio mínimo
    if (x < vEB->min) return vEB->min;

    // Se x é maior ou igual ao máximo, o sucessor não está na estrutura
    if (x >= vEB->max){ 
        // return +INF;
        *is_infinity = 1;
        return x;
    }

    uint32_t c = VEB_HIGH(x, vEB->w);
    uint32_t i = VEB_LOW(x, vEB->w);

    // Busca pelo cluster correto
    VanEmdeBoas *child = eh_search(vEB->clusters, c);

    // se i é menor que o maior elemento no cluster c, então o sucessor estar em c
    if (child != NULL && i < child->max) {
        uint32_t low_succ = veb_successor(child, i, is_infinity);
        return VEB_INDEX(c, low_succ, vEB->w);
    }

    // Caso contrário, o sucessor está no próximo cluster não-vazio 
    uint32_t next_cluster = veb_successor(vEB->summary, c, is_infinity);
    VanEmdeBoas *next = eh_search(vEB->clusters, next_cluster);

    return VEB_INDEX(next_cluster, next->min, vEB->w);
}

uint32_t veb_predecessor(VanEmdeBoas *vEB, uint32_t x, int* is_infinity) {
    if (vEB->is_empty){ 
        // return -INF;
        *is_infinity = 1;
        return x;
    }

    // Se x é menor ou igual ao mínimo, o predecessor não está na estrutura
    if (x <= vEB->min){ 
        // return -INF;
        *is_infinity = 1;
        return x;
    }

    // Se x é maior que o máximo, então o predecessor é o próprio máximo
    if (x > vEB->max) return vEB->max;

    uint32_t c = VEB_HIGH(x, vEB->w);
    uint32_t i  = VEB_LOW (x, vEB->w);

    VanEmdeBoas *child = eh_search(vEB->clusters, c);

    // se i é maior que o menor elemento no cluster c, então o predecessor pode estar em c
    if (child != NULL && i > child->min) {
        int temp = 0;
        uint32_t pred = veb_predecessor(child, i, &temp);

        if (temp){
            // Predecessor não existe dentro do cluster. Retorna o min do cluster
            return VEB_INDEX(c, child->min, vEB->w);
        }
        return VEB_INDEX(c, pred, vEB->w);
    }

    // Predecessor está no cluster anterior. Uso do resumo para recupera-lo 
    int temp = 0;
    uint32_t prev_cluster = veb_predecessor(vEB->summary, c, &temp);

    if (temp){
        // Não há cluster anterior. O predecessor é o mínimo (que não está nos clusters)
        return vEB->min;
    }

    VanEmdeBoas *prev = eh_search(vEB->clusters, prev_cluster);
    return VEB_INDEX(prev_cluster, prev->max, vEB->w);
}

/*
 * Método auxiliar responsável por imprimir os valores presentes em um cluster separados por vírgula.
 *
 * Params:
 *  vEB (VanEmdeBoas*): Estrutura vEB na qual o cluster alvo pertence;
 *  offset (uint32_t): Deslocamento necessário para impressão dos valores originais de chave;
 *  w_root (uint8_t): Número de bits original da estrutura. Necessário para recuperação dos índices i das chaves x = <c,i>;
 *  first (int*): Flag indicativa se o cluster alvo é o primeiro a ser impreso depois da impressão de "min".
 */
static void print_values_recursive(VanEmdeBoas *vEB, uint32_t offset, uint8_t w_root, int *first){
    if (vEB->is_empty) return;


    // Impressão do min deste nível recomposto diretamente com o offset
    if(!*first) printf(", ");

    /* Maneira como deseja que a impressão dos valores seja feita: */

    //printf("%u", offset + vEB->min);                                      // <- Impressão do valor original reconstruído
    //printf("%u", vEB->min);                                               // <- Impressão da metade de bits inferior
    printf("%u", VEB_LOW(offset + vEB->min, w_root));                       // <- Impressão do 1º Nível de recursão do cluster    
    
    *first = 0;

    if (vEB->min == vEB->max) return;

    // Aprofundamento em cada nível diferente de cluster não-nulo, acumulando os offsets em cada nível
    for(uint32_t i = 0; i < vEB->clusters->hash_size; i++){
        for(uint32_t j = 0; j < vEB->clusters->buckets[i].size; j++){
            uint32_t child_c = vEB->clusters->buckets[i].entries[j].c;
            VanEmdeBoas *child_cluster = vEB->clusters->buckets[i].entries[j].vEB;

            uint32_t child_offset = offset + ((child_c) << ((vEB->w) >> 1));
            print_values_recursive(child_cluster, child_offset, root_w, first);   
        }
    }
}

void veb_print(VanEmdeBoas *vEB){
    if (vEB->is_empty) return;

    printf("Min: %u", vEB->min);

    // Só tem 1 elemento?
    if (vEB->min == vEB->max) return; 

    // Para cada cluster não-vazio...
    for(uint32_t i = 0; i < vEB->clusters->hash_size; i++){
        // Para cada entrada...
        for(uint32_t j = 0; j < vEB->clusters->buckets[i].size; j++){
            uint32_t c = vEB->clusters->buckets[i].entries[j].c;
            VanEmdeBoas *cluster = vEB->clusters->buckets[i].entries[j].vEB;

            printf(", C[%u]: ", c);

            uint32_t offset = c << ((vEB->w) >> 1);
            int first = 1;
            // Impressão dos valores presentes no cluster c
            print_values_recursive(cluster, offset, vEB->w, &first);
        }
    }
}