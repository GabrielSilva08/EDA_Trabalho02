#ifndef PARSER_H
#define PARSER_H
#include "van_emde_boas.h"

/* Realiza o parsing das operações passadas pelo arquivo de operações e as executa sobre a estrutura vEB. Cada linha do arquivo representa uma operação no formato:
 *
 * - INC <valor> -> Insere o valor na estrutura vEB;
 * - REM <valor> -> Remove a chave com aquele valor, se existir, da estrutura;
 * - SUC <valor> -> Imprime o sucessor do valor. +INF caso o valor não exista na estrutura;
 * - PRE <valor> -> Imprime o predecessor do valor. -INF caso o valor não exista na estrutura;
 * - IMP <valor> -> Imprime o valor mínimo da estrutura juntamente com os valores presentes nos clusters não vazios.
 * 
 * Linhas não reconhecidas são ignoradas. As etapas do parsing consistem em:
 * 
 *  1. Abre o arquivo de entrada;
 *  2. Lê linha; 
 *  3. Executa operação (se for válida) sobre a vEB fornecida;
 *  4. Repete os passos 2 e 3 até chegar no fim de arquivo.
 * 
 * Params:
 *     filename (char*): Caminho para o arquivo de entrada (.txt);
 *     vEB (VanEmdeBoas*): Ponteiro para a vEB sobre a qual as operações serão aplicadas.
 *     
 * Returns:
 *     (int): Resultado da execução. 0 em sucesso, 1 em caso de erro ao abrir o arquivo.
*/
int parse_and_run(char *filename, VanEmdeBoas *vEB);
#endif
