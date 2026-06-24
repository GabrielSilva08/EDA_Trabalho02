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

| Função                    | Arquivo             | Descrição                                                                                                                          |
|---------------------------|---------------------|------------------------------------------------------------------------------------------------------------------------------------|
| `print_values_recursive`  | `van_emde_boas.c`   | Função auxiliar na impressão da estrutura, se faz necessária para imprimir a estrutura recursiva dos clusters presentes na árvore. |

---

### Módulo `extensible_hash.h`

Módulo responsável pela criação, manuntenção e limpeza de uma tabela de dispersão extensível, ou seja, com tamanho dinâmico através das operações de doubling e halving...

#### `HashEntry`

[...]

#### `Bucket`

[...]

#### `ExtensibleHash`

[...]


### `main.c`

Ponto de entrada do programa. Responsável por realizar a leitura do arquivo de entrada passada como argumento, inicializar uma vEB através da chamada de `veb_create(32)`, passar o controle do processamento das operações ao parser através de `parser_and_run` e, por fim, liberar os recursos alocados via `veb_destroy()`.

---

## Exemplo de uso

Dado o arquivo `input\input_vEB.txt`, a qual contém as operações sobre a vEB:

```text
INC 655364
INC 65535
SUC 100000
IMP
```

Temos como saída esperada:

```text
SUC 100000
655364
IMP
Min: 65535, C[10]: 4
```

> A chave utilizada para salvar os clusters pode diferir com base na função de hashing utilizada!
