// teste_lista.c: testes da fila circular unicamente encadeada.
// Fica na raiz, fora de src/, para nao entrar no build do projeto.
//
// Compilar e rodar (PowerShell, na raiz do repositorio):
//  gcc -std=c11 -Wall -Wextra -g -Isrc -o teste_lista.exe tests/teste_lista.c src/lista.c
#include <stdio.h>
#include <string.h>

#include "lista.h"

static int total = 0;
static int falhas = 0;

#define CHECA(cond, descricao)                                              \
    do {                                                                    \
        total++;                                                            \
        if (cond) {                                                         \
            printf("  [ok]    %s\n", descricao);                            \
        } else {                                                            \
            falhas++;                                                       \
            printf("  [FALHA] %s  (linha %d)\n", descricao, __LINE__);      \
        }                                                                   \
    } while (0)

static Aluno novo_aluno(int matricula)
{
    Aluno a;
    memset(&a, 0, sizeof a);
    a.matricula = matricula;
    snprintf(a.nome,  TAM_NOME,  "Aluno %d", matricula);
    snprintf(a.curso, TAM_CURSO, "Curso %d", matricula % 7);
    return a;
}

static void teste_lista_vazia(void)
{
    printf("\n1) Lista vazia\n");
    Lista *l = lista_criar();
    CHECA(l != NULL,                  "lista_criar devolve ponteiro valido");
    CHECA(lista_tamanho(l) == 0,      "tamanho inicial e 0");
    CHECA(lista_buscar(l, 1) == NULL, "buscar em lista vazia devolve NULL");
    lista_liberar(l);
    CHECA(1, "liberar lista vazia nao quebra");
}

static void teste_um_elemento(void)
{
    printf("\n2) Um elemento\n");
    Lista *l = lista_criar();
    lista_inserir(l, novo_aluno(42));

    CHECA(lista_tamanho(l) == 1,        "tamanho = 1 apos uma insercao");
    Aluno *a = lista_buscar(l, 42);
    CHECA(a != NULL,                    "acha a matricula inserida");
    CHECA(a && a->matricula == 42,      "registro achado tem a matricula certa");
    CHECA(a && strcmp(a->nome, "Aluno 42") == 0, "nome do registro foi preservado");
    CHECA(lista_buscar(l, 43) == NULL,  "matricula inexistente devolve NULL");
    CHECA(lista_buscar(l, 42) != NULL,  "buscar de novo ainda acha (circular nao trava)");
    lista_liberar(l);
}

static void teste_varios_elementos(void)
{
    printf("\n3) Varios elementos (1..1000)\n");
    Lista *l = lista_criar();
    for (int i = 1; i <= 1000; i++) lista_inserir(l, novo_aluno(i));

    CHECA(lista_tamanho(l) == 1000,          "tamanho = 1000");
    CHECA(lista_buscar(l, 1) != NULL,        "acha o primeiro inserido");
    CHECA(lista_buscar(l, 500) != NULL,      "acha um do meio");
    CHECA(lista_buscar(l, 1000) != NULL,     "acha o ultimo inserido");
    CHECA(lista_buscar(l, 0) == NULL,        "matricula 0 (antes do inicio) nao existe");
    CHECA(lista_buscar(l, 1001) == NULL,     "matricula 1001 (depois do fim) nao existe");
    CHECA(lista_buscar(l, -5) == NULL,       "matricula negativa nao existe");

    int todos = 1;
    for (int i = 1; i <= 1000; i++) {
        Aluno *a = lista_buscar(l, i);
        if (!a || a->matricula != i) { todos = 0; break; }
    }
    CHECA(todos, "todas as 1000 matriculas sao encontradas");
    lista_liberar(l);
}

static void teste_circularidade(void)
{
    printf("\n4) Circularidade e buscas repetidas\n");
    Lista *l = lista_criar();
    for (int i = 1; i <= 10; i++) lista_inserir(l, novo_aluno(i * 10));

    int ok = 1;
    for (int volta = 0; volta < 200; volta++) {
        if (!lista_buscar(l, 10))  ok = 0;   // primeiro
        if (!lista_buscar(l, 100)) ok = 0;   // ultimo
        if (lista_buscar(l, 55))   ok = 0;   // inexistente: percorre a volta toda
    }
    CHECA(ok, "200 rodadas de busca (primeiro, ultimo, inexistente) sem travar");
    CHECA(lista_tamanho(l) == 10, "buscas nao alteram o tamanho");
    lista_liberar(l);
}

static void teste_ponteiro_interno(void)
{
    printf("\n5) Busca devolve ponteiro para o dado na lista (sem copia)\n");
    Lista *l = lista_criar();
    lista_inserir(l, novo_aluno(7));

    Aluno *a = lista_buscar(l, 7);
    if (a) strcpy(a->curso, "Alterado");
    Aluno *b = lista_buscar(l, 7);
    CHECA(b && strcmp(b->curso, "Alterado") == 0,
          "alteracao feita pelo ponteiro persiste na lista");
    lista_liberar(l);
}

static void teste_recriar_apos_liberar(void)
{
    printf("\n6) Liberar e recriar\n");
    Lista *l = lista_criar();
    for (int i = 1; i <= 50; i++) lista_inserir(l, novo_aluno(i));
    lista_liberar(l);

    l = lista_criar();
    CHECA(lista_tamanho(l) == 0,      "nova lista comeca vazia");
    lista_inserir(l, novo_aluno(5));
    CHECA(lista_buscar(l, 5) != NULL, "nova lista funciona normalmente");
    CHECA(lista_buscar(l, 1) == NULL, "dados da lista anterior nao aparecem");
    lista_liberar(l);

    lista_liberar(NULL);
    CHECA(1, "lista_liberar(NULL) nao quebra");
}

static void teste_volume(void)
{
    printf("\n7) Volume (100.000 registros)\n");
    Lista *l = lista_criar();
    for (int i = 1; i <= 100000; i++) lista_inserir(l, novo_aluno(i * 3));

    CHECA(lista_tamanho(l) == 100000,        "tamanho = 100000");
    CHECA(lista_buscar(l, 3) != NULL,        "acha o primeiro");
    CHECA(lista_buscar(l, 150000) != NULL,   "acha um do meio");
    CHECA(lista_buscar(l, 300000) != NULL,   "acha o ultimo");
    CHECA(lista_buscar(l, 4) == NULL,        "nao acha matricula que nao foi inserida");
    lista_liberar(l);
}

int main(void)
{
    printf("=== Testes da fila circular unicamente encadeada ===\n");

    teste_lista_vazia();
    teste_um_elemento();
    teste_varios_elementos();
    teste_circularidade();
    teste_ponteiro_interno();
    teste_recriar_apos_liberar();
    teste_volume();

    printf("\n=== Resultado: %d verificacoes, %d falha(s) ===\n", total, falhas);
    return falhas == 0 ? 0 : 1;
}
