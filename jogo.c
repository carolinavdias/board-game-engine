#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "jogo.h"


void mostrar_tabuleiro(Jogo *j) {
    for (int i = 0; i < j->linhas; i++) {
        for (int k = 0; k < j->colunas; k++) {
            printf("%c", j->tabuleiro[i][k]);
        }
        printf("\n");
    }
}


void pintar_branco(Jogo *j, int linha, int coluna, StackHistorico *stack) {
    if (linha >= 0 && linha < j->linhas && coluna >= 0 && coluna < j->colunas){
        AcaoHistorico acao;
        acao.linha = linha;
        acao.coluna = coluna;
        acao.valor = j->tabuleiro[linha][coluna];
        acao.eh_marca = 0;
        push_stack(stack, acao);
        j->tabuleiro[linha][coluna] = toupper(j->tabuleiro[linha][coluna]);
    }
}

void riscar(Jogo *j, int linha, int coluna, StackHistorico *stack) {
    if (linha >= 0 && linha < j->linhas && coluna >= 0 && coluna < j->colunas) {
        AcaoHistorico acao;
        acao.linha = linha;
        acao.coluna = coluna;
        acao.valor = j->tabuleiro[linha][coluna];
        acao.eh_marca = 0;
        push_stack(stack, acao);
        j->tabuleiro[linha][coluna] = '#';
    }
}


void ler_jogo(Jogo *j, const char *ficheiro) {
    FILE *f = fopen(ficheiro, "r");
    if (!f) {
        perror("Erro ao abrir ficheiro");
        return;
    }
    fscanf(f, "%d %d", &j->linhas, &j->colunas);
    j->tabuleiro = malloc(j->linhas * sizeof(char *));
    for (int i = 0; i < j->linhas; i++) {
        j->tabuleiro[i] = malloc(j->colunas * sizeof(char));
        for (int k = 0; k < j->colunas; k++) {
            fscanf(f, " %c", &j->tabuleiro[i][k]);
        }
    }
    fclose(f);

    j->tabuleiro_original = malloc(j->linhas * sizeof(char *));
    for (int i = 0; i < j->linhas; i++) {
        j->tabuleiro_original[i] = malloc(j->colunas * sizeof(char));
        for (int k = 0; k < j->colunas; k++) {
            j->tabuleiro_original[i][k] = j->tabuleiro[i][k];
        }
    }
}

void escrever_jogo(Jogo *j, const char *ficheiro) {
    FILE *f = fopen(ficheiro, "w");
    if (!f) {
        perror("Erro ao gravar ficheiro");
        return;
    }
    fprintf(f, "%d %d\n", j->linhas, j->colunas);
    for (int i = 0; i < j->linhas; i++) {
        for (int k = 0; k < j->colunas; k++) {
            fprintf(f, "%c", j->tabuleiro[i][k]);
        }
        fprintf(f, "\n");
    }
    fclose(f);
}

void libertar_jogo(Jogo *j) {
    for (int i = 0; i < j->linhas; i++) {
        free(j->tabuleiro[i]);
        free(j->tabuleiro_original[i] );
    }
    free(j->tabuleiro);
    free(j->tabuleiro_original);
}

// funções de manipulação da stack de histórico
void inicializar_stack(StackHistorico *stack) {
    stack->capacidade = 10;
    stack->tamanho = 0;
    stack->acoes = malloc(stack->capacidade * sizeof(AcaoHistorico));
    if (!stack->acoes) {
        perror("Erro ao alocar stack de histórico");
        exit(EXIT_FAILURE);
    }
}

void push_stack(StackHistorico *stack, AcaoHistorico acao) {
    if (stack->tamanho == stack->capacidade) {
        stack->capacidade *= 2;
        stack->acoes = realloc(stack->acoes, stack->capacidade * sizeof(AcaoHistorico));
        if (!stack->acoes) {
            perror("Erro ao realocar stack de histórico");
            exit(EXIT_FAILURE);
        }
    }
    stack->acoes[stack->tamanho++] = acao; //insere no fim e aumenta o tamanho da stack
}


void push_marca(StackHistorico *stack) {
    AcaoHistorico marca;
    marca.linha = -1;
    marca.coluna = -1;
    marca.valor = '\0';
    marca.eh_marca = 1;
    push_stack(stack, marca);
}

int pop_stack(StackHistorico *stack, AcaoHistorico *acao) {//retira a ultima ação gravada
    if (stack->tamanho == 0)
        return 0;
    *acao = stack->acoes[--stack->tamanho]; //retira no topo
    return 1;
}


int desfazer_comando(Jogo *j, StackHistorico *stack) {
    if (stack->tamanho == 0) {
        return 0; //nada para desfazer
    }

    int acoes_desfeitas = 0;
    AcaoHistorico acao;
    int encontrou_marca = 0;

    //desfaz ações até encontrar uma marca ou esvaziar a stack
    while (!encontrou_marca && pop_stack(stack, &acao)) {
        if (acao.eh_marca) {
            encontrou_marca = 1;
        } else {
            j->tabuleiro[acao.linha][acao.coluna] = acao.valor;
            acoes_desfeitas++;
        }
    }

    return acoes_desfeitas;
}

void liberar_stack(StackHistorico *stack) {
    free(stack->acoes);
    stack->acoes = NULL;
    stack->tamanho = 0;
    stack->capacidade = 0;
}

//funçao auxiliar para converter uma cordenada em string
static void coord_str(int linha, int coluna, char *s) {
    sprintf(s, "%c%d", 'a' + coluna, linha + 1);
}

//funçao para verificar o estado do jogo e apontar restrições violadas
void verificar_estado(Jogo *j) {
    int valido = 1;
    char coord1[10], coord2[10];
    printf("Verificando o estado do jogo...\n");

    int casas_riscadas_vistas[j->linhas][j->colunas];
    memset(casas_riscadas_vistas, 0, sizeof(casas_riscadas_vistas));

    // duplicações em linhas
    for (int i = 0; i < j->linhas; i++) {
        for (int a = 0; a < j->colunas; a++) {
            char c = j->tabuleiro[i][a];
            if (isupper(c)) {
                for (int b = a + 1; b < j->colunas; b++) {
                    if (j->tabuleiro[i][b] == c) {
                        coord_str(i, a, coord1);
                        coord_str(i, b, coord2);
                        printf("%s e %s são ambas brancas (linha %d duplicada com '%c').\n",
                               coord1, coord2, i + 1, c);
                        valido = 0;
                    }
                }
            }
        }
    }

    //duplicações em colunas
    for (int col = 0; col < j->colunas; col++) {
        for (int i = 0; i < j->linhas; i++) {
            char c = j->tabuleiro[i][col];
            if (isupper(c)) {
                for (int k = i + 1; k < j->linhas; k++) {
                    if (j->tabuleiro[k][col] == c) {
                        coord_str(i, col, coord1);
                        coord_str(k, col, coord2);
                        printf("%s e %s são ambas brancas (coluna '%c' duplicada com '%c').\n",
                               coord1, coord2, 'a' + col, c);
                        valido = 0;
                    }
                }
            }
        }
    }

    //riscadas lado a lado
    int dx[4] = { -1, 1, 0, 0 };
    int dy[4] = { 0, 0, -1, 1 };
    for (int i = 0; i < j->linhas; i++) {
        for (int col = 0; col < j->colunas; col++) {
            if (j->tabuleiro[i][col] == '#') {
                for (int d = 0; d < 4; d++) {
                    int ni = i + dx[d], ncol = col + dy[d];
                    if (ni >= 0 && ni < j->linhas && ncol >= 0 && ncol < j->colunas) {
                        if (j->tabuleiro[ni][ncol] == '#') {
                            if (!casas_riscadas_vistas[i][col] && !casas_riscadas_vistas[ni][ncol]) {
                                coord_str(i, col, coord1);
                                coord_str(ni, ncol, coord2);
                                printf("Casas riscadas lado a lado em %s e %s.\n", coord1, coord2);
                                casas_riscadas_vistas[i][col] = casas_riscadas_vistas[ni][ncol] = 1;
                                valido = 0;
                            }
                        }
                    }
                }
            }
        }
    }

    //minúsculas isoladas
    for (int i = 0; i < j->linhas; i++) {
        for (int col = 0; col < j->colunas; col++) {
            char c = j->tabuleiro[i][col];
            if (islower(c)) {
                int isolado = 1;
                for (int d = 0; d < 4; d++) {
                    int ni = i + dx[d], ncol = col + dy[d];
                    if (ni >= 0 && ni < j->linhas && ncol >= 0 && ncol < j->colunas) {
                        if (j->tabuleiro[ni][ncol] != '#') {
                            isolado = 0;
                            break;
                        }
                    }
                }
                if (isolado) {
                    coord_str(i, col, coord1);
                    printf("A casa %s está isolada.\n", coord1);
                    valido = 0;
                }
            }
        }
    }

    //conexão ortogonal das brancas
    int conectado = 1;
    for (int i = 0; i < j->linhas; i++) {
        for (int a = 0; a < j->colunas; a++) {
            if (isupper(j->tabuleiro[i][a])) {
                if (!conectado_ortogonalmente(j, i, a)) {
                    coord_str(i, a, coord1);
                    printf("Violação: a casa %s não está conectada ortogonalmente a todas as outras casas brancas.\n", coord1);
                    conectado = 0;
                }
            }
        }
    }

    if (valido && conectado) {
        printf("Estado do jogo válido: nenhuma restrição violada.\n");
    }
}

int conectado_ortogonalmente(Jogo *j, int linha, int coluna) {
    int visitado[j->linhas][j->colunas];
    memset(visitado, 0, sizeof(visitado));

    int fila[j->linhas * j->colunas][2];
    int inicio = 0, fim = 0;
    fila[fim][0] = linha;
    fila[fim][1] = coluna;
    fim++;

    visitado[linha][coluna] = 1;

    int dx[4] = { -1, 1, 0, 0 };
    int dy[4] = { 0, 0, -1, 1 };
    while (inicio < fim) {
        int x = fila[inicio][0];
        int y = fila[inicio][1];
        inicio++;

        for (int i = 0; i < 4; i++) {
            int nx = x + dx[i], ny = y + dy[i];
            if (nx >= 0 && nx < j->linhas && ny >= 0 && ny < j->colunas &&
                !visitado[nx][ny] && j->tabuleiro[nx][ny] != '#') {
                visitado[nx][ny] = 1;
                fila[fim][0] = nx;
                fila[fim][1] = ny;
                fim++;
            }
        }
    }

    for (int i = 0; i < j->linhas; i++) {
        for (int k = 0; k < j->colunas; k++) {
            if (j->tabuleiro[i][k] != '#' && !visitado[i][k]) {
                return 0;
            }
        }
    }

    return 1;
}


//tenta inferir todas as casas que podem ser riscadas ou pintadas
int ajuda_inferir(Jogo *j, StackHistorico *stack) {
    int mudou = 0;

    //riscar réplicas da mesma letra branca em linha/coluna
    for (int l = 0; l < j->linhas; l++) {
        for (int c = 0; c < j->colunas; c++) {
            if (isupper(j->tabuleiro[l][c])) {
                char letra = j->tabuleiro[l][c];
                for (int i = 0; i < j->linhas; i++) {
                    if (i != l && j->tabuleiro[i][c] == tolower(letra)) {
                        riscar(j, i, c, stack);
                        mudou = 1;
                    }
                }
                for (int k = 0; k < j->colunas; k++) {
                    if (k != c && j->tabuleiro[l][k] == tolower(letra)) {
                        riscar(j, l, k, stack);
                        mudou = 1;
                    }
                }
            }
        }
    }

    //pintar de branco vizinhas de cada casa riscada
    for (int l = 0; l < j->linhas; l++) {
        for (int c = 0; c < j->colunas; c++) {
            if (j->tabuleiro[l][c] == '#') {
                int dx2[4] = { -1, 1, 0, 0 };
                int dy2[4] = { 0, 0, -1, 1 };
                for (int d = 0; d < 4; d++) {
                    int nl = l + dx2[d], nc = c + dy2[d];
                    if (nl >= 0 && nl < j->linhas && nc >= 0 && nc < j->colunas &&
                        islower(j->tabuleiro[nl][nc])) {
                        pintar_branco(j, nl, nc, stack);
                        mudou = 1;
                    }
                }
            }
        }
    }

    //pintar de branco casas que, se riscadas, isolariam brancas
    for (int l = 0; l < j->linhas; l++) {
        for (int c = 0; c < j->colunas; c++) {
            if (islower(j->tabuleiro[l][c])) {
                char temp = j->tabuleiro[l][c];
                j->tabuleiro[l][c] = '#';
                if (!conectado_ortogonalmente(j, l, c)) {
                    j->tabuleiro[l][c] = temp;
                    pintar_branco(j, l, c, stack);
                    mudou = 1;
                } else {
                    j->tabuleiro[l][c] = temp;
                }
            }
        }
    }

    return mudou;
}

//repete ajuda_inferir até estabilizar
void aplicar_ajuda_completa(Jogo *j, StackHistorico *stack) {
    while (ajuda_inferir(j, stack));
}

//resolve por inferência + backtracking
int estado_valido(Jogo *j) {
    //verifica duplicações em linhas
    for (int i = 0; i < j->linhas; i++) {
        for (int a = 0; a < j->colunas; a++) {
            char c = j->tabuleiro[i][a];
            if (isupper(c)) {
                for (int b = a + 1; b < j->colunas; b++) {
                    if (j->tabuleiro[i][b] == c)
                        return 0;
                }
            }
        }
    }

    //verifica duplicações em colunas
    for (int col = 0; col < j->colunas; col++) {
        for (int i = 0; i < j->linhas; i++) {
            char c = j->tabuleiro[i][col];
            if (isupper(c)) {
                for (int k = i + 1; k < j->linhas; k++) {
                    if (j->tabuleiro[k][col] == c)
                        return 0;
                }
            }
        }
    }

    //riscadas lado a lado
    int dx[4] = { -1, 1, 0, 0 }, dy[4] = { 0, 0, -1, 1 };
    for (int i = 0; i < j->linhas; i++) {
        for (int col = 0; col < j->colunas; col++) {
            if (j->tabuleiro[i][col] == '#') {
                for (int d = 0; d < 4; d++) {
                    int ni = i + dx[d], ncol = col + dy[d];
                    if (ni >= 0 && ni < j->linhas && ncol >= 0 && ncol < j->colunas) {
                        if (j->tabuleiro[ni][ncol] == '#')
                            return 0;
                    }
                }
            }
        }
    }

    //minúsculas isoladas
    for (int i = 0; i < j->linhas; i++) {
        for (int col = 0; col < j->colunas; col++) {
            char c = j->tabuleiro[i][col];
            if (islower(c)) {
                int isolado = 1;
                for (int d = 0; d < 4; d++) {
                    int ni = i + dx[d], ncol = col + dy[d];
                    if (ni >= 0 && ni < j->linhas && ncol >= 0 && ncol < j->colunas) {
                        if (j->tabuleiro[ni][ncol] != '#') {
                            isolado = 0;
                            break;
                        }
                    }
                }
                if (isolado)
                    return 0;
            }
        }
    }

    return 1;
}

void restaurar_tabuleiro_original(Jogo *j) {
    for (int i = 0; i < j->linhas; i++) {
        for (int k = 0; k < j->colunas; k++) {
            j->tabuleiro[i][k] = j->tabuleiro_original[i][k];
        }
    }
}


int resolver_jogo(Jogo *j, StackHistorico *stack) {
    //aplicar ajuda completa (tenta  pintar e riscar)
    aplicar_ajuda_completa(j, stack);

    //percorrer o tabuleiro
    for (int l = 0; l < j->linhas; l++) {
        for (int c = 0; c < j->colunas; c++) {
            if (islower(j->tabuleiro[l][c])) {

                //tentar pintar a casa de branco
                int tam_stack = stack->tamanho;
                pintar_branco(j, l, c, stack);
                aplicar_ajuda_completa(j, stack);

                if (estado_valido(j) && resolver_jogo(j, stack)) return 1;
                while (stack->tamanho > tam_stack) {
                    AcaoHistorico a;
                    pop_stack(stack, &a);
                    j->tabuleiro[a.linha][a.coluna] = a.valor;  //restaurar a célula original
                }

                //tentar riscar a casa
                tam_stack = stack->tamanho;
                riscar(j, l, c, stack);
                aplicar_ajuda_completa(j, stack);

                if (estado_valido(j) && resolver_jogo(j, stack)) return 1;
                //r everter alterações caso a opção de riscar não tenha funcionado
                while (stack->tamanho > tam_stack) {
                    AcaoHistorico a;
                    pop_stack(stack, &a);
                    j->tabuleiro[a.linha][a.coluna] = a.valor;//restaurar a célula original
                }

                return 0;
            }
        }
    }

    //verifica ligação ortogonal final das casas brancas
    for (int i = 0; i < j->linhas; i++) {
        for (int k = 0; k < j->colunas; k++) {
            if (isupper(j->tabuleiro[i][k])) {
                return conectado_ortogonalmente(j, i, k);  //verificar conexão ortogonal
            }
        }
    }

    return 1;  //se todas as casas brancas estão conectadas, o estado é válido
}