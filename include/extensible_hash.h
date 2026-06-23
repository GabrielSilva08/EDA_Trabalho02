#ifndef EXTENSIBLE_HASH_H
#define EXTENSIBLE_HASH_H

#include <stdint.h>

// Declaração atencipada da estrutura van Emde Boas
struct VanEmdeBoas;

/*
 * Elemento de entrada da tabela hash, represente o par <chave, estrutura vEB> armazenado dentro de um bucket.
 * 
 * Params:
 *  c (uint32_t): Representa o índice de cluster (bits superiores) dado a decomposição da chave em coordenadas hierárquicas key = <c, i>;
 *  vEB (VanEmdeBoas*): Estrutura (subárvore) van Emde Boas associada à entrada.
 */
typedef struct {
    uint32_t c;
    struct VanEmdeBoas *vEB;
} HashEntry;

/*
 * Lista encadeada linear (array dinâmico) de entradas. Valores de chave com um mesmo hashing pertecem a um mesmo bucket.
 * 
 * Params:
 *  entries (HashEntry*): Elementos de entrada na estrutura. Adaptado para receber estruturas van Emde Boas;
 *  size (uint32_t): Quantidade de elementos no bucket;
 *  capacity (uint32_t): Capacidade máxima alocada ao bucket.
 */
typedef struct {
    HashEntry *entries;     
    uint32_t size;    
    uint32_t capacity;          
} Bucket;

/*
 * Tabela de dispersão (hash) extensível com doubling e halving. A função de dispersão adotada é simplesmente (key % hash_size), onde hash_size é sempre uma potência de 2. 
 * Quando a taxa de ocupação ultrapassa MAX_LOAD, dobramos o tamanho da tabela (doubling); quando cai abaixo de MIN_LOAD, dividimos o tamanho pela metade (halving), ambos os procedimentos são feitos através do rehash.
 * O tratamento de colisão escolhido é o encadeamento, onde as entradas com um mesmo hash são encadeadas em um mesmo bucket.
 *
 * Params:
 *  buckets (Bucket*): "Tabela" (array) do hashing, é nele onde as entradas são armazenadas;
 *  hash_size (uint32_t): Tamanho atual da tabela, m;
 *  values_quantity (uint32_t): Quantidade total de entradas (buckets) na tabela, n;
 *  w (uint8_t): Número de bits adotados pelo modelo palavra RAM da estrutura. 
 */
typedef struct {
    Bucket *buckets;
    uint32_t hash_size;     
    uint32_t values_quantity;
    uint8_t w;                          
} ExtensibleHash;

/* Operações públicas às outras estruturas do projeto */

/*
 * Método construtor responsável por inicializar um Extensible Hash.
 *
 * Params:
 *  w (uint8_t): Bits de máquina necessário para inicializar as entrandas vEB.
 * 
 * Returns:
 *  (ExtensibleHash*): Hash extensível inicializado
*/
ExtensibleHash *eh_create(uint8_t w);

/*
 * Busca pela estrutura vEB associada a chave de pesquisa key. Retorna o ponteiro que aponta pra estrutura se essa estiver presente, NULL caso o contrário.
 *
 * Params:
 *  hash (ExtensibleHash*): Tabela de dispersão a qual se deseja fazer uma busca pela chave.
 *  key (uint32_t): Chave de pesquisa a qual se desejar encontrar.
 * 
 * Returns:
 *  (VanEmdeBoas*): Estrutura buscada ou NULL
 */
struct VanEmdeBoas *eh_search(ExtensibleHash *hash, uint32_t key);

/*
 * Insere uma nova estrutura vEB associada ao valor key (cluster) e retorna o ponteiro associado.
 * Se uma estrutura com a respectiva chave já existir, é retornado diretamente o endereço, sem necessidade de fazer uma nova inclusão.
 * 
 * Params:
 *  hash (ExtensibleHash*): Tabela de dispersão a qual se deseja realizar a nova inserção;
 *  key (uint32_t): Chave associada a estrutura inserida.
 * 
 * Returns:
 *  (VanEmdeBoas*): Estrutura vEB inserida
 */
struct VanEmdeBoas *eh_insert(ExtensibleHash *hash, uint32_t key);

/*
 * Remoção da entrada associada à chave key da tabela de dispersão. 
 *
 * Params:
 *  hash (ExtensibleHash*): Tabela de dispersão a qual se deseja realizar a remoção;
 *  key (uint32_t): Chave associada a estrutura a ser removida.
 */
void eh_remove(ExtensibleHash *hash, uint32_t key);

/* 
 * Imprime a tabela de dispersão bem como as entradas associadas à cada bucket.
 * 
 * Params:
 *  hash (ExtensibleHash*): Tabela de dispersão a qual se deseja imprimir seu conteúdo.
 */
void eh_print(ExtensibleHash *hash);

/*
 * Método destrutor responsável por liberar a memória associada a um Extensible Hash.
 *
 * Params:
 *  hash (ExtensibleHash*): Tabela de dispersão a qual se deseja liberar.
 */
void eh_destroy(ExtensibleHash *hash);

#endif
