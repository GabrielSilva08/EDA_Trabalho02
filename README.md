# Trabalho 02 - Estrutura de Dados Avançadas

## Árvore van Emde Boas + Tabela de Dispersão

O código fonte implementa uma árvore de van Emde Boas na linguagem C para o modelo padrão de palavra RAM (considerando uma máquina que trabalha com palavras de tamanho $w=32$ bits). Para efeitos de ganho de espaço, fora implementada também uma tabela de dispersão (table doubling/having), com o tratamento de colisões sendo feito por encadeamento, para representar o vetor de clusters presente em uma estrutura vEB. A implementação segue uma organização limpa e modular, com os devidos objetos separados em arquivos para possibilitar menor acoplamento entre as classes. O programa implementado oferece suporte às operações de inserção, remoção, busca pelo sucessor, busca pelo predecessor e impressão da estrutura como um todo.

---

## Sumário

- [Linguagem e versão](#linguagem-e-versão)
- [Estrutura do projeto](#estrutura-do-projeto)
- [Como compilar e executar](#como-compilar-e-executar)
- [Formato do arquivo de entrada](#formato-do-arquivo-de-entrada)
- [Divisão dos módulos do projeto](#divisão-dos-módulos-do-projeto)
- [Exemplo de uso](#exemplo-de-uso)

---

## Linguagem e versão

| Item       | Valor                          |
|------------|--------------------------------|
| Linguagem  | C                              |
| Padrão     | C11 (`-std=c11`)               |
| Compilador | GCC = 13.2.0                   |
| Build tool | GNU Make                       |

---

## Estrutura do projeto

```text
EDA_Trabalho02/
├── bin/                     # Contém o executável final gerado após a compilação do projeto.
│   └── program.exe           
├── build/                   # Armazena os arquivos objetos produzidos durante o processo de compilação.
│   ├── main.o
│   ├── parser.o
│   ├── extensible_hash.o
│   └── van_emde_boas.o
├── include/                 # Contém os arquivos de cabeçalho utilizados no projeto.
│   ├── parser.h
│   ├── extensible_hash.h
│   └── van_emde_boas.h
├── input/                   # Contém os arquivos de entrada utilizados para testes e execução do programa.   
├── output/                  # Contém os arquivos de saída gerados pela execução do programa.
├── src/                     # Contém os arquivos-fonte responsáveis pela implementação do projeto.
│   ├── main.c
│   ├── parser.c
│   ├── extensible_hash.c
│   ├── van_emde_boas.c
│   └── ...                  # Arquivos.c de testes e depuração interna das próprias estruturas
├── Makefile                 # Arquivo responsável pela automação do processo de compilação do projeto.
└── README.md                # Documento contendo informações gerais sobre o projeto, compilação, execução e organização do repositório.
```

---

## Como compilar e executar

### Compilação

O `Makefile` presente nesse projeto já cuida dessa extensa etapa, bastando apenas executar:

```bash
make build
```

Isso gera os executáveis auxiliares na pasta `build\`, bem como o próprio programa final `program.exe` na pasta `bin\`.

### Execução

Com o código já compilado, basta executar:

```bash
make run
```

A qual irá rodar o programa sobre o arquivo de exemplo presente em `input\`. Caso deseje rodar sobre um outro arquivo de entrada, basta inserir o arquivo na pasta e então executar:

```bash
make run INPUT=<arquivo_com_operações_na_vEB.txt>
```

---

## Formato do arquivo de entrada

O arquivo de entrada é um `.txt` com **uma operação por linha**:

| Operação | Formato                | Descrição                                                                                                                   | Saída                                             |
|----------|------------------------|-----------------------------------------------------------------------------------------------------------------------------|---------------------------------------------------|
| Inserção    | `INC <valor>`          | Insere a chave `<valor>` na estrutura vEB. Duplicatas são ignoradas                                                         | Nenhuma                                           |
| Remoção     | `REM <valor>`          | Remove a chave `<valor>` da estrutura. Se inexistente, a operação será ignorada.                                            | Nenhuma                                           |
| Sucessor    | `SUC <valor>`          | Encontra a menor chave na estrutura estritamente maior que `<valor>`.                                                       | Linha 1: `SUC <valor>` / Linha 2: resultado       |
| Predecessor | `PRE <valor>`          | Encontra a maior chave na estrutura estritamente menor que `<valor>`.                                                       | Linha 1: `PRE <valor>` / Linha 2: resultado       |
| Imprimir    | `IMP`                  | Imprime o valor mínimo da estrutura juntamente com os valores presentes nos clusters não vazios.                            | Linha 1: `IMP` / Linha 2: `Min: val, C[val]: val, val, ...` |

> **Nota sobre `SUC`:** O valor de referência **não precisa** existir na árvore. Caso não exista sucessor, é impresso `+INF`.

> **Nota sobre `PRE`:** O valor de referência **não precisa** existir na árvore. Caso não exista sucessor, é impresso `-INF`.

---

## Divisão dos módulos do projeto

O projeto é separado em 3 módulos especiais (`parser.h`, `extensible_hash.h` e `van_emde_boas.h`) juntamente de um arquivo `main.c`.

### Módulo `parser.h`

Módulo responsável pela leitura e processamento das operações passadas via um arquivo de entrada localizado no diretório `input\`. Das funções presentes no módulo, tem-se:

| Função          | Assinatura                                                | Arquivo    | Descrição |
|-----------------|-----------------------------------------------------------|------------|-----------|
| `parse_and_run` | `int parse_and_run(char *filename, VanEmdeBoas *tree)` | `parser.c` | Abre o arquivo, lê linha a linha e despacha cada operação para o módulo `van_emde_boas`. Retorna `0` em sucesso ou `1` em erro de abertura do arquivo de entrada. |

---

### Módulo `van_emde_boas.h`

Módulo responsável pela criação, manuntenção e limpeza de uma árvore van Emde Boas, além disso, permite realizar as operações de consulta e atualização sobre a estrutura.

#### `VanEmdeBoas`

Contêiner que encapsula informações essenciais para a manuntenção de uma van Emde Boas. Apresenta de maneira bem similar os atributos essencias para se operar em uma estrutura vEB. O universo de chaves $U$ (cujo tamanho é representado por $w$ bits) é dividido em $\sqrt{u}$ clusters de tamanho $\sqrt{u}$. A profundidade da recursão, que é o principal fator de complexidade dos métodos implementados, é $O(\log \log u)$, ou $O(\log w)$. Devido a utilização de uma tabela de dispersão para os clusters, a complexidade de espaço é reduzida de $O(u)$ para $O(n)$.

```c
typedef struct VanEmdeBoas {
    uint8_t w;                    // Número de bits utilizados na representação dos elementos;
    uint32_t min;                 // Menor elemento presente salvo à parte da estrutura de clusters;
    uint32_t max;                 // Maior elemento presente;
    int is_empty;                 // Flag para indicar se a estrutura está vazia ou não;
    ExtensibleHash *clusters;     // Vetor de clusters, faz uso do ExtensibleHash para indexação das vEB associadas e ocasionar melhoria significativa de espaço;
    struct VanEmdeBoas *summary;  // Estrutura de resumo, responsável por manter rastreio dos clusters não vazios. 
} VanEmdeBoas;
```

Funções disponíveis ao usuário do módulo:

| Função                | Assinatura                                                                 | Arquivo             | Descrição                                                                                          |
|-----------------------|----------------------------------------------------------------------------|---------------------|----------------------------------------------------------------------------------------------------|
| `veb_create`          | `VanEmdeBoas *veb_create(uint8_t w)`                                       | `van_emde_boas.c`   | Cria os campos de uma vEB vazia.                                                                   |
| `veb_insert`          | `void veb_insert(VanEmdeBoas *vEB, uint32_t x)`                            | `van_emde_boas.c`   | Insere chave de valor x na estrutura vEB desejada. Valores duplicados são ignorados.               |
| `veb_remove`          | `void veb_remove(VanEmdeBoas *vEB, uint32_t x)`                            | `van_emde_boas.c`   | Remove chave de valor x da estrutura. Se não existir, nada é feito.                                |
| `veb_successor`       | `uint32_t veb_successor(VanEmdeBoas *vEB, uint32_t x, int* is_infinity)`   | `van_emde_boas.c`   | Imprime no terminal o valor do sucessor de x, ou `+INF` em caso desse não existir na estrutura.    |
| `veb_predecessor`     | `uint32_t veb_predecessor(VanEmdeBoas *vEB, uint32_t x, int* is_infinity)` | `van_emde_boas.c`   | Imprime no terminal o valor do predecessor de x, ou `-INF` em caso desse não existir na estrutura. |
| `veb_print`           | `void veb_print(VanEmdeBoas *vEB)`                                         | `van_emde_boas.c`   | Imprime o menor valor salvo pela estrutura juntamente com os clusters não-vazios.                  |
| `veb_destroy`         | `void veb_destroy(VanEmdeBoas *vEB)`                                       | `van_emde_boas.c`   | Desaloca a memória utilizada pela estrutura.                                                       |

Funções auxiliares (`static`, não são expostas no header!) criadas com o intuito de simplificar os algoritmos principais e propiciar maior facilidade de manuntenção do projeto:

| Função                    | Assinatura                                                                                          | Arquivo             | Descrição                                                                                                                          |
|---------------------------|-----------------------------------------------------------------------------------------------------|---------------------|------------------------------------------------------------------------------------------------------------------------------------|
| `print_values_recursive`  | `static void print_values_recursive(VanEmdeBoas *vEB, uint32_t offset, uint8_t w_root, int *first)` | `van_emde_boas.c`   | Função auxiliar na impressão da estrutura, se faz necessária para imprimir a estrutura recursiva dos clusters presentes na árvore. |

---

### Módulo `extensible_hash.h`

Módulo responsável pela criação, manuntenção e limpeza de uma tabela de dispersão extensível, ou seja, com tamanho dinâmico através das operações de doubling e halving, cujos os parâmetros de redimensionamento (`MAX_LOAD` e `MIN_LOAD`) podem ser alterados posteriormente. A estrutura foi implementada em um nível de granularidade superior para garantir o melhor gerenciamento possível de memória utilizada pela classe. A função de dispersão calcula com base no tamanho atual da tabela (`x mod m`), a escolha dessa função se dá pelo objetivo de termos uma maior simplicidade no processo. Pelo mesmo motivo apresentado anteriormente, o tratamento de colisões é feito por meio do **encadeamento** das entradas em uma lista dinâmica, a qual vai tendo seu tamanho dobrado a medida que suas entradas ficam cheias.

#### `HashEntry`

Registro representativo das entradas da tabela hash propriamente dito. Tem por finalidade representar o par `<chave, vEB>` dentro de um bucket, a qual esse pode possuir 1 ou mais entradas.

```c
typedef struct {
    uint32_t c;              // Representa o índice de cluster (bits superiores) dado a decomposição da chave em coordenadas hierárquicas key = <c, i>;
    struct VanEmdeBoas *vEB; // Estrutura (subárvore) van Emde Boas associada à entrada.
} HashEntry;
```

#### `Bucket`

Lista encadeada linear (array dinâmico) de entradas. Valores de chave com um mesmo hashing pertecem a um mesmo bucket. Sua implementação se deve à forma como o tratamento de colisões foi escolhido.

```c
typedef struct {
    HashEntry *entries; // Elementos de entrada na estrutura. Adaptado para receber estruturas van Emde Boas;
    uint32_t size;      // Quantidade de elementos no bucket;
    uint32_t capacity;  // Capacidade máxima alocada ao bucket.  
} Bucket;
```

#### `ExtensibleHash`

Tabela de dispersão (hash) extensível com doubling e halving. A função de dispersão adotada é simplesmente (key % hash_size), onde hash_size é assumido sempre ser uma potência de 2. Quando a taxa de ocupação ultrapassa MAX_LOAD, dobramos o tamanho da tabela (doubling); quando cai abaixo de MIN_LOAD, dividimos o tamanho pela metade (halving), ambos os procedimentos são feitos através do rehash. Entradas com um mesmo hash são encadeadas em um mesmo bucket.

```c
typedef struct {
    Bucket *buckets;          // "Tabela" (array) do hashing, é nele onde as entradas são armazenadas;
    uint32_t hash_size;       // Tamanho atual da tabela, m;
    uint32_t values_quantity; // Quantidade total de entradas (buckets) na tabela, n;
    uint8_t w;                // Número de bits adotados pelo modelo palavra RAM da estrutura.                
} ExtensibleHash;
```

Funções disponíveis ao usuário do módulo:

| Função                | Assinatura                                                                 | Arquivo               | Descrição                                                                                                                            |
|-----------------------|----------------------------------------------------------------------------|-----------------------|--------------------------------------------------------------------------------------------------------------------------------------|
| `eh_create`           | `ExtensibleHash *eh_create(uint8_t w)`                                     | `extensible_hash.c`   | Cria e inicializa os campos de um EH vazio.                                                                                          |
| `eh_search`           | `struct VanEmdeBoas *eh_search(ExtensibleHash *hash, uint32_t key)`        | `extensible_hash.c`   | Busca pela estrutura vEB associada a chave de pesquisa key. Retornado o ponteiro da estrutura, caso presente. NULL caso o contrário. |
| `eh_insert`           | `struct VanEmdeBoas *eh_insert(ExtensibleHash *hash, uint32_t key)`        | `extensible_hash.c`   | Insere uma nova estrutura vEB associada ao valor key (cluster) e retorna o ponteiro associado.                                       |
| `eh_remove`           | `void eh_remove(ExtensibleHash *hash, uint32_t key)`                       | `extensible_hash.c`   | Remoção da entrada associada à chave key da tabela de dispersão                                                                      |
| `eh_print`            | `void eh_print(ExtensibleHash *hash)`                                      | `extensible_hash.c`   | Imprime a tabela de dispersão a partir de suas entradas não vazias associadas à cada bucket.                                         |
| `eh_destroy`          | `void eh_destroy(ExtensibleHash *hash)`                                    | `extensible_hash.c`   | Imprime o menor valor salvo pela estrutura juntamente com os clusters não-vazios.                                                    |
| `veb_destroy`         | `void veb_destroy(VanEmdeBoas *vEB)`                                       | `extensible_hash.c`   | Libera a memória dos elementos utilizados pela estrutura.                                                                            |

Funções auxiliares (`static`, não são expostas no header!) criadas com o intuito de simplificar os algoritmos principais e propiciar maior facilidade de manuntenção do projeto. Em geral, são focadas no gerenciamento das entradas e buckets presentes em um EH:

| Função                | Assinatura                                                                 | Arquivo               | Descrição                                                                                                                            |
|-----------------------|----------------------------------------------------------------------------|-----------------------|--------------------------------------------------------------------------------------------------------------------------------------|
| `hash_function`       | `static uint32_t hash_function(ExtensibleHash *hash, uint32_t key)`        | `extensible_hash.c`   | Função de dispersão adotada nos cálculos de hash.                                                                                    |
| `bucket_find_index`   | `static int bucket_find_index(Bucket *bucket, uint32_t key)`               | `extensible_hash.c`   | Busca pelo valor ao longo de um bucket. Retorna -1 se não encontrado.                                                                |
| `bucket_push`         | `static void bucket_push(Bucket *bucket, HashEntry entry)`                 | `extensible_hash.c`   | Insere uma nova entrada ao final do bucket.                                                                                          |
| `bucket_remove_at`    | `static void bucket_remove_at(Bucket *bucket, uint32_t idx)`               | `extensible_hash.c`   | Remoção da entrada associada ao índice idx no bucket.                                                                                |
| `rehash`              | `static void rehash(ExtensibleHash *hash, uint32_t new_size)`              | `extensible_hash.c`   | Redistribui todas as entradas existentes para uma nova tabela de dispersão de tamanho new_size. É chamado após doubling ou halving.  |
| `eh_destroy`          | `void eh_destroy(ExtensibleHash *hash)`                                    | `extensible_hash.c`   | Imprime o menor valor salvo pela estrutura juntamente com os clusters não-vazios.                                                    |
| `veb_destroy`         | `void veb_destroy(VanEmdeBoas *vEB)`                                       | `extensible_hash.c`   | Libera a memória dos elementos utilizados pela estrutura.                                                                            |
| `check_load`          | `static void check_load(ExtensibleHash *hash)`                             | `extensible_hash.c`   | Verifica a taxa de ocupação da tabela hash e aplica o rehashing caso seja necessário.                                                |

---

### `main.c`

Ponto de entrada do programa. Responsável por realizar a leitura do arquivo de entrada passada como argumento, inicializar uma vEB através da chamada de `veb_create(32)`, passar o controle do processamento das operações ao parser através de `parser_and_run` e, por fim, liberar os recursos alocados via `veb_destroy()`.

---

## Exemplo de uso

Dado o arquivo `input\input_vEB.txt`, a qual contém as operações sobre a vEB:

```text
INC 10
INC 20
INC 5
SUC 4
SUC 5
PRE 21
PRE 5
INC 100
INC 65536
INC 65540
IMP
SUC 20
SUC 100
PRE 65536
REM 10
REM 5
IMP
SUC 20
PRE 100
INC 131072
INC 131073
IMP
REM 65536
REM 65540
IMP
SUC 100
PRE 131072
REM 20
IMP
REM 131072
REM 131073
IMP
REM 100
IMP
SUC 0
PRE 10
```

Temos como saída esperada:

```text
SUC 4
5
SUC 5
10
PRE 21
20
PRE 5
-INF
IMP
Min: 5, C[0]: 10, 20, 100, C[1]: 0, 4
SUC 20
100
SUC 100
65536
PRE 65536
100
IMP
Min: 20, C[0]: 100, C[1]: 0, 4
SUC 20
100
PRE 100
20
IMP
Min: 20, C[0]: 100, C[1]: 0, 4, C[2]: 0, 1
IMP
Min: 20, C[0]: 100, C[2]: 0, 1
SUC 100
131072
PRE 131072
100
IMP
Min: 100, C[2]: 0, 1
IMP
Min: 100
IMP

SUC 0
+INF
PRE 10
-INF
```

> A chave utilizada para salvar os clusters pode diferir com base na função de hashing utilizada!

> O uso do comando "IMP" pode retornar valores diferentes a depender do printf() ativo na impressão recursiva
