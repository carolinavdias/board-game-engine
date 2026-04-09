#ifndef JOGO_H
#define JOGO_H

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

typedef struct {
    int linha;
    int coluna;
    char valor;
    int eh_marca; // 1 se for uma marca, 0 se for uma ação normal
} AcaoHistorico;

typedef struct {
    int capacidade; // capacidade atual no array
    int tamanho; // número de ações atualmente na stack
    AcaoHistorico *acoes; //array dinâmico de ações
} StackHistorico;

typedef struct {
    int linhas;
    int colunas;
    char **tabuleiro;
    char **tabuleiro_original;
} Jogo;

//funções de manipulação do tabuleiro
void mostrar_tabuleiro(Jogo *j);
void pintar_branco(Jogo *j, int linha, int coluna, StackHistorico *stack);
void riscar(Jogo *j, int linha, int coluna, StackHistorico *stack);
void ler_jogo(Jogo *j, const char *ficheiro);
void escrever_jogo(Jogo *j, const char *ficheiro);
void libertar_jogo(Jogo *j);

// funções de manipulação da stack histórico
void inicializar_stack(StackHistorico *stack);
void push_stack(StackHistorico *stack, AcaoHistorico acao);
void push_marca(StackHistorico *stack);
int pop_stack(StackHistorico *stack, AcaoHistorico *acao);
int desfazer_comando(Jogo *j, StackHistorico *stack);
void liberar_stack(StackHistorico *stack);

//funções de verificação
void verificar_estado(Jogo *j);
int conectado_ortogonalmente(Jogo *j, int linha, int coluna);

// inferência e resolução
int ajuda_inferir(Jogo *j, StackHistorico *stack);
void aplicar_ajuda_completa(Jogo *j, StackHistorico *stack);
void restaurar_tabuleiro_original(Jogo *j);
int resolver_jogo(Jogo *j, StackHistorico *stack);
int estado_valido(Jogo *j);

#endif // JOGO_H
