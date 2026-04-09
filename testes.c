#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include "jogo.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

//testes de ficheiros
void test_ler_e_gravar_jogo(void) {
    Jogo j;
    ler_jogo(&j, "t.txt");
    CU_ASSERT_EQUAL(j.linhas, 5);
    CU_ASSERT_EQUAL(j.colunas, 5);
    CU_ASSERT_EQUAL(j.tabuleiro[0][0], 'e');
    CU_ASSERT_EQUAL(j.tabuleiro[4][4], 'b');
    escrever_jogo(&j, "saida.txt");
    libertar_jogo(&j);
}

void test_ler_erro(void) {
    Jogo j;
    ler_jogo(&j, "inexistente.txt");
}

//testes de stack
void test_stack_basico(void) {
    StackHistorico s;
    inicializar_stack(&s);
    AcaoHistorico a = {1, 1, 'c', 0};  //eh_marca inicializado para 0
    push_stack(&s, a);
    CU_ASSERT_EQUAL(s.tamanho, 1);
    AcaoHistorico r;
    CU_ASSERT_TRUE(pop_stack(&s, &r));
    CU_ASSERT_EQUAL(r.valor, 'c');
    liberar_stack(&s);
}

//testes de pintar/riscar
void test_pintar_e_riscar(void) {
    Jogo j;
    StackHistorico s;
    inicializar_stack(&s);
    ler_jogo(&j, "t.txt");
    pintar_branco(&j, 1, 1, &s);
    CU_ASSERT_TRUE(isupper(j.tabuleiro[1][1]));
    riscar(&j, 2, 2, &s);
    CU_ASSERT_EQUAL(j.tabuleiro[2][2], '#');
    libertar_jogo(&j);
    liberar_stack(&s);
}

//testes de verificação de estado
void test_verificar_estado(void) {
    Jogo j;
    StackHistorico s;
    inicializar_stack(&s);
    ler_jogo(&j, "t.txt");
    pintar_branco(&j, 0, 0, &s);
    pintar_branco(&j, 0, 1, &s); // duplica E
    verificar_estado(&j);
    libertar_jogo(&j);
    liberar_stack(&s);
}

//testes de inferencia
void test_ajuda_inferir(void) {
    Jogo j;
    StackHistorico s;
    inicializar_stack(&s);
    ler_jogo(&j, "t.txt");
    int r = ajuda_inferir(&j, &s);
    CU_ASSERT_TRUE(r >= 0);
    libertar_jogo(&j);
    liberar_stack(&s);
}

void test_aplicar_ajuda_completa(void) {
    Jogo j;
    StackHistorico s;
    inicializar_stack(&s);
    ler_jogo(&j, "t.txt");
    aplicar_ajuda_completa(&j, &s);
    libertar_jogo(&j);
    liberar_stack(&s);
}

//testes de resolver
void test_resolver_jogo(void) {
    Jogo j;
    StackHistorico s;
    inicializar_stack(&s);
    ler_jogo(&j, "t.txt");
    restaurar_tabuleiro_original(&j);
    int r = resolver_jogo(&j, &s);
    CU_ASSERT_TRUE(r);
    libertar_jogo(&j);
    liberar_stack(&s);
}

//outros testes
void test_conectado(void) {
    Jogo j;
    j.linhas = 2; j.colunas = 2;
    j.tabuleiro = malloc(2 * sizeof(char*));
    for (int i = 0; i < 2; i++) j.tabuleiro[i] = malloc(2);
    j.tabuleiro[0][0] = 'E'; j.tabuleiro[0][1] = 'E';
    j.tabuleiro[1][0] = 'E'; j.tabuleiro[1][1] = 'E';
    CU_ASSERT_TRUE(conectado_ortogonalmente(&j, 0, 0));
    for (int i = 0; i < 2; i++) free(j.tabuleiro[i]);
    free(j.tabuleiro);
}

void test_restaurar_original(void) {
    Jogo j;
    StackHistorico s;
    inicializar_stack(&s);
    ler_jogo(&j, "t.txt");
    char original = j.tabuleiro[0][0];
    pintar_branco(&j, 0, 0, &s);
    restaurar_tabuleiro_original(&j);
    CU_ASSERT_EQUAL(j.tabuleiro[0][0], original);
    libertar_jogo(&j);
    liberar_stack(&s);
}

//teste de desfazer comando
void test_desfazer_comando(void) {
    Jogo j;
    StackHistorico s;
    inicializar_stack(&s);
    ler_jogo(&j, "t.txt");

    char antes = j.tabuleiro[1][1]; // guarda o valor antes da ação

    push_marca(&s);
    pintar_branco(&j, 1, 1, &s);

    int desfeitas = desfazer_comando(&j, &s);
    CU_ASSERT_EQUAL(desfeitas, 1);

    //verifica que o tabuleiro voltou ao valor inicial
    CU_ASSERT_EQUAL(j.tabuleiro[1][1], antes);

    libertar_jogo(&j);
    liberar_stack(&s);
}

//main
int main() {
    CU_initialize_registry();
    CU_pSuite s = CU_add_suite("Testes_Jogo", NULL, NULL);

    CU_add_test(s, "Ler e Gravar", test_ler_e_gravar_jogo);
    CU_add_test(s, "Erro ler ficheiro", test_ler_erro);
    CU_add_test(s, "Stack básica", test_stack_basico);
    CU_add_test(s, "Pintar e riscar", test_pintar_e_riscar);
    CU_add_test(s, "Verificar estado", test_verificar_estado);
    CU_add_test(s, "Ajuda inferir", test_ajuda_inferir);
    CU_add_test(s, "Ajuda completa", test_aplicar_ajuda_completa);
    CU_add_test(s, "Resolver jogo", test_resolver_jogo);
    CU_add_test(s, "Conectado ortogonal", test_conectado);
    CU_add_test(s, "Restaurar tabuleiro", test_restaurar_original);
    CU_add_test(s, "Desfazer comando", test_desfazer_comando);

    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();

    return 0;
}

