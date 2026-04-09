CC = gcc
CFLAGS = -Wall -Wextra -pedantic -O1 -fsanitize=address -fno-omit-frame-pointer -g
LDFLAGS = -lcunit

# Compilar o programa principal
jogo: main.o jogo.o
        $(CC) $(CFLAGS) -o jogo main.o jogo.o

main.o: main.c jogo.h
        $(CC) $(CFLAGS) -c main.c

jogo.o: jogo.c jogo.h
        $(CC) $(CFLAGS) -c jogo.c

# Compilar e correr testes com CUnit
testar: teste
        ./teste

teste: testes.o jogo.o
        $(CC) $(CFLAGS) -o teste testes.o jogo.o $(LDFLAGS)

testes.o: testes.c jogo.h
        $(CC) $(CFLAGS) -c testes.c

# Limpar ficheiros temporários
clean:
        rm -f *.o jogo teste
