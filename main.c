#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "jogo.h"

int main() {
    Jogo j;
    StackHistorico historico;
    char comando[100];
    int jogo_carregado = 0;

    inicializar_stack(&historico);

    printf("Comandos disponíveis:\n");
    printf("l ficheiro.txt      - ler jogo\n");
    printf("g ficheiro.txt      - gravar jogo\n");
    printf("b a1                - pintar casa de branco\n");
    printf("r a1                - riscar casa\n");
    printf("v                   - verificar estado do jogo\n");
    printf("a                   - aplicar inferências (comando a)\n");
    printf("A                   - aplicar inferências até fixar (comando A)\n");
    printf("R                   - resolver o jogo automaticamente\n");
    printf("d                   - desfazer última ação\n");
    printf("s                   - sair\n");

    while (1) {
        printf("> ");
        if (!fgets(comando, sizeof(comando), stdin))
            break;
        comando[strcspn(comando, "\n")] = 0;

        if (comando[0] == 's' && strlen(comando) == 1) {
            break;

        } else if (comando[0] == 'l') {
            char nome[100];
            if (sscanf(comando + 1, "%s", nome) == 1) {
                ler_jogo(&j, nome);
                jogo_carregado = 1;
                mostrar_tabuleiro(&j);
            } else {
                printf("Erro: comando 'l' requer nome de ficheiro.\n");
            }

        } else if (comando[0] == 'g') {
            char nome[100];
            if (jogo_carregado && sscanf(comando + 1, "%s", nome) == 1) {
                escrever_jogo(&j, nome);
                printf("Jogo gravado em %s\n", nome);
            } else {
                printf("Erro: jogo não carregado ou nome inválido.\n");
            }

        } else if (comando[0] == 'b') {
            char col; int lin;
            if (jogo_carregado && sscanf(comando + 1, " %c%d", &col, &lin) == 2) {
                int c = col - 'a';
                if (lin < 1 || lin > j.linhas || c < 0 || c >= j.colunas) {
                    printf("Erro: Coordenada fora do tabuleiro.\n");
                    continue;
                }
                push_marca(&historico);
                pintar_branco(&j, lin - 1, c, &historico);
                mostrar_tabuleiro(&j);
            } else {
                printf("Erro: formato inválido ou jogo não carregado.\n");
            }

        } else if (comando[0] == 'r') {
            char col; int lin;
            if (jogo_carregado && sscanf(comando + 1, " %c%d", &col, &lin) == 2) {
                int c = col - 'a';
                if (lin < 1 || lin > j.linhas || c < 0 || c >= j.colunas) {
                    printf("Erro: Coordenada fora do tabuleiro.\n");
                    continue;
                }
                push_marca(&historico);
                riscar(&j, lin - 1, c, &historico);
                mostrar_tabuleiro(&j);
            } else {
                printf("Erro: formato inválido ou jogo não carregado.\n");
            }

        } else if (comando[0] == 'v' && strlen(comando) == 1) {
            if (jogo_carregado) {
                verificar_estado(&j);
            } else {
                printf("Erro: jogo não carregado.\n");
            }

        } else if (comando[0] == 'a' && strlen(comando) == 1) {
            if (jogo_carregado) {
                push_marca(&historico);
                ajuda_inferir(&j, &historico);
                mostrar_tabuleiro(&j);
            } else {
                printf("Erro: jogo não carregado.\n");
            }
        } else if (comando[0] == 'A' && strlen(comando) == 1) {
            if (jogo_carregado) {
                push_marca(&historico);
                aplicar_ajuda_completa(&j, &historico);
                mostrar_tabuleiro(&j);
            } else {
                printf("Erro: jogo não carregado.\n");
            }

        } else if (comando[0] == 'R' && strlen(comando) == 1) {
            if (jogo_carregado) {
                push_marca(&historico);
                restaurar_tabuleiro_original(&j);
                if (resolver_jogo(&j, &historico)) {
                    printf("Jogo resolvido!\n");
                    mostrar_tabuleiro(&j);
                } else {
                    printf("Não foi possível resolver o jogo automaticamente.\n");
                }
            } else {
                printf("Erro: jogo não carregado.\n");
            }

        } else if (comando[0] == 'd' && strlen(comando) == 1) {
            if (jogo_carregado) {
                int acoes_desfeitas = desfazer_comando(&j, &historico);
                if (acoes_desfeitas > 0) {
                    printf("Desfeitas %d ações.\n", acoes_desfeitas);
                    mostrar_tabuleiro(&j);
                } else {
                    printf("Não há comandos para desfazer.\n");
                }
            } else {
                printf("Erro: jogo não carregado.\n");
            }

        } else if (jogo_carregado && strlen(comando) >= 2 &&
                   islower(comando[0]) && isdigit(comando[1])) {
            char col; int lin;
            if (sscanf(comando, "%c%d", &col, &lin) == 2) {
                int c = col - 'a';
                if (lin < 1 || lin > j.linhas || c < 0 || c >= j.colunas) {
                    printf("Coordenada fora do tabuleiro.\n");
                } else {
                    printf("Conteúdo: %c\n", j.tabuleiro[lin - 1][c]);
                }
            } else {
                printf("Coordenada inválida.\n");
            }

        } else {
            printf("Comando desconhecido.\n");
        }
    }

    if (jogo_carregado) {
        libertar_jogo(&j);
    }
    liberar_stack(&historico);
    printf("Jogo terminado.\n");
    return 0;
}
