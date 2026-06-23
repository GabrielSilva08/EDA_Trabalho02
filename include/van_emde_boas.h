#ifndef VAN_EMDE_BOAS_H
#define VAN_EMDE_BOAS_H

#include <stdint.h>
#include "extensible_hash.h"

/*
 * Estrutura van Emde Boas. O universo de chaves (cujo tamanho é representado por w bits) é dividido em sqrt(u) clusters de tamanho sqrt(u).
 * A profundidade da recursão, que o principal fator de complexidade dos métodos implementados, é O(log log u), ou O(log w).
 *
 * Params:
 *  w (uint8_t): Número de bits utilizados na representação dos elementos;
 *  min (uint32_t): Menor elemento presente salvo à parte da estrutura de clusters;
 *  max (uint32_t): Maior elemento presente;
 *  is_empty (int): Flag para indicar se a estrutura está vazia ou não;
 *  clusters (ExtensibleHash*): Vetor de clusters, faz uso do ExtensibleHash para indexação das vEB associadas e ocasionar melhoria significativa de espaço;
 *  summary (VanEmdeBoas*): Estrutura de resumo, responsável por manter rastreio dos clusters não vazios.
 */
typedef struct VanEmdeBoas {
    uint8_t w;
    uint32_t min;
    uint32_t max;
    int is_empty;       
    ExtensibleHash *clusters;
    struct VanEmdeBoas *summary;
} VanEmdeBoas;

/* Operações públicas disponíveis às outras partes do projeto, como o parser e a main*/

/*
 * Método construtor responsável por realizar a alocação do espaço de memória necessário e inicializar a estrutura para o espaço de chaves de tamanho w bits.
 *
 * Params:
 *  w (uint8_t): Número de bits.
 * 
 * Returns:
 *  (VanEmdeBoas*): Estrutura vEB alocada
 */
VanEmdeBoas *veb_create(uint8_t w);

/*
 * Método destrutor responsável por realizar a desalocação recursiva de toda a memória utilizada pela estrutura.
 * 
 * Params:
 *  vEB (VanEmdeBoas*): Estrutura vEB a ser destruída.
 */
void veb_destroy(VanEmdeBoas *vEB);

/*
 * Inseri valor de chave x na estrutura vEB, se a chave já constar na estrutura (valor repetido), esse é ignorado.
 *
 * Params:
 *  vEB (VanEmdeBoas*): Estrutura vEB a ter o elemento inserido;
 *  x (uint32_t): Chave a ser inserida.
*/
void veb_insert(VanEmdeBoas *vEB, uint32_t x);

/*
 * Remove o valor de chave x na estrutura vEB.
 *
 * Params:
 *  vEB (VanEmdeBoas*): Estrutura vEB a ter o elemento removido;
 *  x (uint32_t): Chave a ser removida.
*/
void veb_remove(VanEmdeBoas *vEB, uint32_t x);

/*
 * Retorna o menor elemento maior que o valor de chave x passado. Caso esse elemento não exista na estrutura, o campo is_infinity sinaliza que o retorno é +INF.
 * 
 * Params:
 *  vEB (VanEmdeBoas*): Estrutura vEB a qual desejamos buscar pelo sucessor;
 *  x (uint32_t): Valor a qual desejamos buscar seu sucessor na estrutura;
 *  is_infinity (int*): Flag indicadora se o retorno é ou não +INF.
 * 
 * Returns:
 *  (uint32_t): Sucessor de x se esse estiver presente na estrutura.
 */
uint32_t veb_successor  (VanEmdeBoas *vEB, uint32_t x, int* is_infinity);

/*
 * Retorna o maior elemento menor que o valor de chave x passado. Caso esse elemento não exista na estrutura, o campo is_infinity sinaliza que o retorno é -INF.
 * 
 * Params:
 *  vEB (VanEmdeBoas*): Estrutura vEB a qual desejamos buscar pelo predecessor;
 *  x (uint32_t): Valor a qual desejamos buscar seu predecessor na estrutura;
 *  is_infinity (int*): Flag indicadora se o retorno é ou não +INF.
 * 
 * Returns:
 *  (uint32_t): Predecessor de x se esse estiver presente na estrutura.
 */
uint32_t veb_predecessor(VanEmdeBoas *vEB, uint32_t x, int* is_infinity);

/*
 * Realiza a impressão do menor valor salvo pela estrutura juntamente com os clusters não-nulos.
 *
 * Params:
 *  vEB (VanEmdeBoas*): Estrutura vEB a ser impresa.
 */
void veb_print(VanEmdeBoas *vEB);

#endif
