# Arquivo de entrada
INPUT = input_vEB.txt

# Flags de compilação, arquivos fontes e objetos
CFLAGS  = -std=c11 -Wall -Wextra -Werror -pedantic
SRCS    = src\main.c src\van_emde_boas.c src\extensible_hash.c src\parser.c
OBJS    = build\main.o build\van_emde_boas.o build\extensible_hash.o build\parser.o
LIBS	= include

# Nome do executável 
TARGET  = bin\program.exe

# Compila e executa (build + run)
all: build run

# Compilação do código
build: build-single
	gcc $(CFLAGS) $(OBJS) -o $(TARGET)

build-single: $(SRCS)
	gcc $(CFLAGS) -c src\van_emde_boas.c -I $(LIBS)  -o build\van_emde_boas.o
	gcc $(CFLAGS) -c src\extensible_hash.c -I $(LIBS)  -o build\extensible_hash.o
	gcc $(CFLAGS) -c src\parser.c -I $(LIBS) -o build\parser.o
	gcc $(CFLAGS) -c src\main.c -I $(LIBS) -o build\main.o

# Execução do programa
# Uso: make run INPUT=meu_arquivo.txt
run: $(TARGET)
	$(TARGET) input\$(INPUT)
