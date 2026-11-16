/*
 * Driver dos experimentos: compara 5 implementacoes
 *   lista | aberto+divisao | aberto+alternativa | encadeado+divisao | encadeado+alternativa
 *
 * Uso: ./experimento <base.txt> <N> [saida.csv] [sem_lista]
 *   N = 100000, 1000000 ou 10000000
 *   sem_lista = pula a lista (util se a busca no 10M demorar demais)
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "aluno.h"
#include "hash_funcs.h"
#include "lista.h"
#include "hash_aberto.h"
#include "hash_encadeado.h"
#include "utils.h"

#define QTD_BUSCAS  10000
#define SEMENTE     12345u
#define N_ETAPAS    3
#define FATOR_TAB   1.3   /* m ~ 1.3 * N: no aberto, alfa final ~0,77 (<1) */

static const int PERCENTUAIS[N_ETAPAS] = {30, 60, 100};

/* ---------- Adaptador: permite tratar as 5 estruturas do mesmo jeito ---------- */
typedef struct {
    const char *nome;
    int   eh_hash;                      /* 0 = lista (sem colisao/fator de carga) */
    FuncaoHash funcao;                  /* NULL para a lista */
    void  *(*criar)(unsigned int m, FuncaoHash f);
    void   (*inserir)(void *e, Aluno a);
    void  *(*buscar)(void *e, int matricula);
    long   (*colisoes)(void *e);
    void   (*zerar_colisoes)(void *e);
    double (*fator_carga)(void *e);
    void   (*liberar)(void *e);
} Impl;

/* --- wrappers da lista --- */
static void  *w_l_criar(unsigned int m, FuncaoHash f) { (void)m; (void)f; return lista_criar(); }
static void   w_l_ins(void *e, Aluno a)  { lista_inserir(e, a); }
static void  *w_l_bus(void *e, int mat)  { return lista_buscar(e, mat); }
static void   w_l_lib(void *e)           { lista_liberar(e); }

/* --- wrappers do enderecamento aberto --- */
static void  *w_a_criar(unsigned int m, FuncaoHash f) { return ha_criar(m, f); }
static void   w_a_ins(void *e, Aluno a)  { ha_inserir(e, a); }
static void  *w_a_bus(void *e, int mat)  { return ha_buscar(e, mat); }
static long   w_a_col(void *e)           { return ha_colisoes(e); }
static void   w_a_zer(void *e)           { ha_zerar_colisoes(e); }
static double w_a_fc(void *e)            { return ha_fator_carga(e); }
static void   w_a_lib(void *e)           { ha_liberar(e); }

/* --- wrappers do encadeamento separado --- */
static void  *w_e_criar(unsigned int m, FuncaoHash f) { return he_criar(m, f); }
static void   w_e_ins(void *e, Aluno a)  { he_inserir(e, a); }
static void  *w_e_bus(void *e, int mat)  { return he_buscar(e, mat); }
static long   w_e_col(void *e)           { return he_colisoes(e); }
static void   w_e_zer(void *e)           { he_zerar_colisoes(e); }
static double w_e_fc(void *e)            { return he_fator_carga(e); }
static void   w_e_lib(void *e)           { he_liberar(e); }

/* ---------- Execucao de uma implementacao ---------- */
static void rodar(const Impl *im, const Aluno *base, long N, unsigned int m,
                  int *const buscas[N_ETAPAS], FILE *csv)
{
    printf("\n== %s ==\n", im->nome);
    void *e = im->criar(m, im->funcao);
    if (!e) { fprintf(stderr, "Falha ao criar %s\n", im->nome); return; }

    long inicio = 0;
    for (int et = 0; et < N_ETAPAS; et++) {
        long fim = N * PERCENTUAIS[et] / 100;

        if (im->eh_hash) im->zerar_colisoes(e);

        /* --- insercao do intervalo [inicio, fim) --- */
        double t0 = agora();
        for (long i = inicio; i < fim; i++) im->inserir(e, base[i]);
        double t_ins = agora() - t0;

        /* --- 10.000 buscas (mesmas matriculas em todas as implementacoes) --- */
        long achados = 0;
        t0 = agora();
        for (int k = 0; k < QTD_BUSCAS; k++)
            if (im->buscar(e, buscas[et][k])) achados++;
        double t_bus = agora() - t0;

        long   col = im->eh_hash ? im->colisoes(e)    : -1;
        double fc  = im->eh_hash ? im->fator_carga(e) : -1.0;

        if (achados != QTD_BUSCAS)
            fprintf(stderr, "ATENCAO: %s achou %ld de %d buscas!\n", im->nome, achados, QTD_BUSCAS);

        printf("  %3d%%  ins=%.4fs  busca=%.4fs  colisoes=%ld  alfa=%.4f\n",
               PERCENTUAIS[et], t_ins, t_bus, col, fc);

        fprintf(csv, "%ld,%u,%s,%d,%.6f,%.6f,%ld,%.6f\n",
                N, m, im->nome, PERCENTUAIS[et], t_ins, t_bus, col, fc);
        fflush(csv);

        inicio = fim;
    }
    im->liberar(e);   /* libera tudo antes da proxima (economiza RAM) */
}

int main(int argc, char **argv)
{
    if (argc < 3) {
        fprintf(stderr, "Uso: %s <base.txt> <N> [saida.csv] [sem_lista]\n", argv[0]);
        return 1;
    }
    const char *caminho = argv[1];
    long N = atol(argv[2]);
    const char *saida = (argc >= 4) ? argv[3] : "resultado.csv";
    int usar_lista = !(argc >= 5 && strcmp(argv[4], "sem_lista") == 0);

    /* Leitura da base (FORA da medicao) */
    long lidos = 0;
    Aluno *base = ler_base(caminho, N, &lidos);
    if (!base || lidos < N) {
        fprintf(stderr, "Base insuficiente: lidos %ld de %ld\n", lidos, N);
        return 1;
    }

    /* Tamanho da tabela: primo >= FATOR_TAB * N (justificar nos slides) */
    unsigned int m = proximo_primo((unsigned int)(N * FATOR_TAB));
    printf("N=%ld  m=%u  (m/N=%.2f)\n", N, m, (double)m / N);

    /* Sorteio das matriculas (FORA da medicao), uma vez so, reutilizado nas 5 impls */
    int *buscas[N_ETAPAS];
    for (int et = 0; et < N_ETAPAS; et++) {
        long n_ins = N * PERCENTUAIS[et] / 100;
        buscas[et] = sortear_matriculas(base, n_ins, QTD_BUSCAS, SEMENTE + et);
    }

    FILE *csv = fopen(saida, "w");
    if (!csv) { perror("csv"); return 1; }
    fprintf(csv, "volume,m,estrutura,etapa_pct,tempo_insercao_s,tempo_busca_s,colisoes,fator_carga\n");

    Impl impls[] = {
        {"lista",               0, NULL,
         w_l_criar, w_l_ins, w_l_bus, NULL,    NULL,    NULL,   w_l_lib},
        {"aberto_divisao",      1, hash_divisao,
         w_a_criar, w_a_ins, w_a_bus, w_a_col, w_a_zer, w_a_fc, w_a_lib},
        {"aberto_alternativa",  1, hash_alternativa,
         w_a_criar, w_a_ins, w_a_bus, w_a_col, w_a_zer, w_a_fc, w_a_lib},
        {"encadeado_divisao",   1, hash_divisao,
         w_e_criar, w_e_ins, w_e_bus, w_e_col, w_e_zer, w_e_fc, w_e_lib},
        {"encadeado_alternativa", 1, hash_alternativa,
         w_e_criar, w_e_ins, w_e_bus, w_e_col, w_e_zer, w_e_fc, w_e_lib},
    };
    int n_impls = (int)(sizeof impls / sizeof impls[0]);

    for (int i = 0; i < n_impls; i++) {
        if (i == 0 && !usar_lista) { printf("\n(lista pulada)\n"); continue; }
        rodar(&impls[i], base, N, m, buscas, csv);
    }

    fclose(csv);
    for (int et = 0; et < N_ETAPAS; et++) free(buscas[et]);
    free(base);
    printf("\nResultados salvos em %s\n", saida);
    return 0;
}
