#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "extensible_hash.h"
#include "van_emde_boas.h"

/* Constantes de ocupação definidas para o rehashing */
#define MAX_LOAD 0.75
#define MIN_LOAD 0.25

/* Funções utilitárias do Extensible Hash */

/*
 * Função de dispersão adotada no hashing. Por questão de simplicidade, consiste simplesmente no resto da divisão pelo tamanho da tabela (key % m).
 * 
 * Params:
 *  hash (ExtensibleHash*): Tabela de dispersão a qual se deseja computar a função de dispersão;
 *  key (uint32_t): Chave a qual se deseja aplicar a função de dispersão.
 */
static uint32_t hash_function(ExtensibleHash *hash, uint32_t key);

/*
 * Busca pelo valor key ao longo de um bucket, retornando seu índice interno. Retorna -1 se não encontrado.
 *
 * Params:
 *  bucket (Bucket*): Bucket a qual se deseja realizar a busca sobre;
 *  key (uint32_t): Valor de busca interesado.
 */
static int bucket_find_index(Bucket *bucket, uint32_t key);

/*
 * Inserção de uma entrada ao final do bucket. 
 * 
 * Params:
 *  bucket (Bucket*): Bucket a qual se deseja inserir uma nova entrada;
 *  entry (HashEntry): Entrada a qual se deseja inserir.
 */
static void bucket_push(Bucket *bucket, HashEntry entry);

/*
 * Remoção da entrada no índice idx.
 *
 * Params:
 *  bucket (Bucket*): Bucket a qual se deseja realizar a remoção de um elemento;
 *  idx (uint32_t): Índice do elemento de interesse a ser removido.
 */
static void bucket_remove_at(Bucket *bucket, uint32_t idx);

/*
 * Redistribui todas as entradas existentes para uma nova tabela de dispersão de tamanho new_size. É chamado após doubling ou halving.
 *
 * Params:
 *  hash (ExtensibleHash*): Tabela de dispersão a qual se deseja fazer a redistribuição;
 *  new_size (uint32_t): Novo tamanho da tabela.
 */
static void rehash(ExtensibleHash *hash, uint32_t new_size);

/*
 * Verifica a taxa de ocupação da tabela hash e aplica o rehashing caso seja necessário.
 * 
 * Params:
 *  hash (ExtensibleHash*): Tabela de dispersão a qual deseja verificar seu fator de carga.
 */
static void check_load(ExtensibleHash *hash);

/* Funções principais do Extensible Hash */

ExtensibleHash *eh_create(uint8_t w);
VanEmdeBoas *eh_search(ExtensibleHash *hash, uint32_t key);
void eh_destroy(ExtensibleHash *hash);
VanEmdeBoas *eh_insert(ExtensibleHash *hash, uint32_t key);
void eh_remove(ExtensibleHash *hash, uint32_t key);
void eh_print(ExtensibleHash *hash);
void eh_destroy(ExtensibleHash *hash);

/* Implementação dos métodos */

static uint32_t hash_function(ExtensibleHash *hash, uint32_t key){
    //hash_size é sempre uma potência de 2, logo, podemos fazer uso de manipulação bit a bit.
    // return (key & (hash->hash_size-1));
    return key % hash->hash_size;
}

static int bucket_find_index(Bucket *bucket, uint32_t key){
    for (uint32_t i = 0; i < bucket->size; i++){
        // Verificação se o cluster corrente casa com a chave de busca
        if (bucket->entries[i].c == key) return (int) i;
    }
    return -1;
}

static void bucket_push(Bucket *bucket, HashEntry entry){
    // Verificação se o bucket atingiu a capacidade máxima atual
    if (bucket->size == bucket->capacity){
        // Dobra-se a capacidade antiga
        uint32_t new_capacity = bucket->capacity == 0 ? 2 : bucket->capacity * 2;
        bucket->entries = realloc(bucket->entries, new_capacity * sizeof(HashEntry));
        bucket->capacity = new_capacity;
    }
    bucket->entries[bucket->size++] = entry;
}

static void bucket_remove_at(Bucket *bucket, uint32_t idx){
    // Troca pelo último elemento da lista de colisões
    bucket->entries[idx] = bucket->entries[bucket->size - 1];
    bucket->size--;
}

static void rehash(ExtensibleHash *hash, uint32_t new_size){
    // Alocação de novos buckets
    Bucket *new_buckets = calloc(new_size, sizeof(Bucket));

    // Percorre todos os buckets atuais e reinserere cada entrada
    for(uint32_t i = 0; i < hash->hash_size; i++){
        for(uint32_t j = 0; j < hash->buckets[i].size; j++) {
            HashEntry entry = hash->buckets[i].entries[j];
            // Nova posição realocadas
            uint32_t slot = entry.c % new_size;
            bucket_push(&new_buckets[slot], entry);
        }
        // Liberação da antiga lista de buckets
        free(hash->buckets[i].entries); 
    }
    free(hash->buckets);
    hash->buckets = new_buckets;
    hash->hash_size = new_size;
}

static void check_load(ExtensibleHash *hash){ 
    // Doubling: values_quantity / hash_size > MAX_LOAD
    if (hash->values_quantity > MAX_LOAD * hash->hash_size){
        rehash(hash, hash->hash_size * 2);
        return;
    }
    // Halving: values_quantity / hash_size < MIN_LOAD, mas hash_size > 1 (evitar metadear hash de tamanho unitário)
    if (hash->hash_size > 1 && hash->values_quantity < MIN_LOAD * hash->hash_size){
        rehash(hash, hash->hash_size / 2);
    }
}

ExtensibleHash *eh_create(uint8_t w){
    ExtensibleHash *hash = malloc(sizeof(ExtensibleHash));
    hash->buckets = NULL;
    hash->hash_size = 0;
    hash->values_quantity = 0;
    hash->w = w;
    return hash;
}

VanEmdeBoas *eh_search(ExtensibleHash *hash, uint32_t key){
    // Caso da tabela vazia
    if (hash->hash_size == 0) return NULL;

    uint32_t slot = hash_function(hash, key);
    int idx = bucket_find_index(&hash->buckets[slot], key);
    if (idx < 0) return NULL;
    // cluster associado
    return hash->buckets[slot].entries[idx].vEB;
}

VanEmdeBoas *eh_insert(ExtensibleHash *hash, uint32_t key){
    // Inicialização da tabela com 1 bucket na primeira inserção 
    if (hash->hash_size == 0) {
        hash->hash_size++;
        hash->buckets = calloc(1, sizeof(Bucket));
    }

    // Se a chave já existe, retorna o vEB associado sem duplicar
    VanEmdeBoas *existing = eh_search(hash, key);
    if (existing != NULL) return existing;

    // Criação de uma nova estrutura vEB filho para o cluster com metade do tamanho original (w/2)
    VanEmdeBoas *child = veb_create(hash->w);

    HashEntry entry = {.c = key, .vEB = child};
    uint32_t slot = hash_function(hash, key);
    bucket_push(&hash->buckets[slot], entry);

    hash->values_quantity++;
    // Verificação se o fator de carga continua aceitável (possível dobrar)
    check_load(hash);
    return child;
}

void eh_remove(ExtensibleHash *hash, uint32_t key){
    // Hash vazio (nada a remover)
    if (hash->hash_size == 0) return;

    uint32_t slot = hash_function(hash, key);
    int idx = bucket_find_index(&hash->buckets[slot], key);
    if (idx < 0) return; // Bucket não encontrado

    bucket_remove_at(&hash->buckets[slot], (uint32_t) idx);
    hash->values_quantity--;
    // Verificação se o fator de carga continua aceitável (possível metadear)
    check_load(hash);
}

void eh_print(ExtensibleHash *hash){
    printf("---- hash (size=%u, entries=%u) ----\n", hash->hash_size, hash->values_quantity);
    for (uint32_t i = 0; i < hash->hash_size; i++){
        // Impressão dos buckets
        printf("bucket[%u]: ", i);
        // Impressão das entradas
        for (uint32_t j = 0; j < hash->buckets[i].size; j++) {
            printf("%u ", hash->buckets[i].entries[j].c);
        }
        printf("\n");
    }
    printf("----\n");
}

void eh_destroy(ExtensibleHash *hash){
    for (uint32_t i = 0; i < hash->hash_size; i++) {
        // Liberação das entradas presentes em 1 bucket
        free(hash->buckets[i].entries);
    }
    // Liberação da estrutura de buckets
    free(hash->buckets);
    free(hash);
}