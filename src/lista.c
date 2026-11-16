/*
 * lista.c: fila circular unicamente encadeada (TAD opaco)
 *
 * Cada no aponta apenas para o proximo, e o ultimo no aponta de volta
 * para o primeiro. A lista guarda um ponteiro para o ULTIMO no (fim),
 * entao o primeiro no e sempre fim->prox.
 *
 *   inserir: O(1)  (entra no fim da fila)
 *   buscar:  O(n)  (percorre do primeiro no ate voltar a ele)
 */
#include <stdio.h>
#include <stdlib.h>

#include "lista.h"

typedef struct no {
    Aluno dado;
    struct no *prox;
} No;

struct lista {
    No *fim;
    long n;     
};

Lista *lista_criar(void)
{
    Lista *l = malloc(sizeof(Lista));
    if (!l) return NULL;
    l->fim = NULL;
    l->n = 0;
    return l;
}

void lista_inserir(Lista *l, Aluno a)
{
    No *novo = malloc(sizeof(No));
    if (!novo) {
        fprintf(stderr, "lista_inserir: sem memoria (n=%ld)\n", l->n);
        exit(EXIT_FAILURE);
    }
    novo->dado = a;

    if (l->fim == NULL) {
        
        novo->prox = novo;
    } else {
        
        novo->prox = l->fim->prox;
        l->fim->prox = novo;
    }
    l->fim = novo;   
    l->n++;
}

Aluno *lista_buscar(Lista *l, int matricula)
{
    if (l->fim == NULL) return NULL;

    No *primeiro = l->fim->prox;
    No *p = primeiro;
    do {
        if (p->dado.matricula == matricula)
            return &p->dado;   
        p = p->prox;
    } while (p != primeiro);   

    return NULL;
}

long lista_tamanho(const Lista *l)
{
    return l->n;
}

void lista_liberar(Lista *l)
{
    if (!l) return;

    if (l->fim != NULL) {
        No *p = l->fim->prox;  
        l->fim->prox = NULL;    
        while (p != NULL) {
            No *prox = p->prox;
            free(p);
            p = prox;
        }
    }
    free(l);
}