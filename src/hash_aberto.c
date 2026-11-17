#include <stdlib.h>
#include "hash_aberto.h"

typedef enum { LIVRE, OCUPADO } StatusSlot;

typedef struct {
    Aluno aluno;
    StatusSlot status;
} Slot;

struct hash_aberto {
    Slot *tabela;
    unsigned int m;
    unsigned int n;
    long colisoes;
    FuncaoHash hash_func;
};

HashAberto *ha_criar(unsigned int m, FuncaoHash f) {
    HashAberto *h = (HashAberto *)malloc(sizeof(HashAberto));
    if (h == NULL) return NULL;

    h->m = m;
    h->n = 0;
    h->colisoes = 0;
    h->hash_func = f;

    h->tabela = (Slot *)malloc(m * sizeof(Slot));
    if (h->tabela == NULL) {
        free(h);
        return NULL;
    }

    for (unsigned int i = 0; i < m; i++) {
        h->tabela[i].status = LIVRE;
    }

    return h;
}

int ha_inserir(HashAberto *h, Aluno a) {
    if (h->n == h->m) {
        return 0; 
    }

    unsigned int indice = h->hash_func(a.matricula, h->m);

    for (unsigned int i = 0; i < h->m; i++) {
        unsigned int pos = (indice + i) % h->m;

        if (h->tabela[pos].status == LIVRE) {
            h->tabela[pos].aluno = a;
            h->tabela[pos].status = OCUPADO;
            h->n++;
            return 1;
        }
        
        h->colisoes++;
    }

    return 0;
}

Aluno *ha_buscar(HashAberto *h, int matricula) {
    unsigned int indice = h->hash_func(matricula, h->m);

    for (unsigned int i = 0; i < h->m; i++) {
        unsigned int pos = (indice + i) % h->m;

        if (h->tabela[pos].status == LIVRE) {
            return NULL;
        }

        if (h->tabela[pos].status == OCUPADO && h->tabela[pos].aluno.matricula == matricula) {
            return &(h->tabela[pos].aluno);
        }
    }

    return NULL;
}

long ha_colisoes(const HashAberto *h) {
    return h->colisoes;
}

void ha_zerar_colisoes(HashAberto *h) {
    h->colisoes = 0;
}

double ha_fator_carga(const HashAberto *h) {
    return (double)h->n / h->m;
}

void ha_liberar(HashAberto *h) {
    if (h != NULL) {
        free(h->tabela);
        free(h);
    }
}