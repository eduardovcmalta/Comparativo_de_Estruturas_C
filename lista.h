#ifndef LISTA_H
#define LISTA_H
#include "aluno.h"

/* Estrutura linear escolhida: lista encadeada (simples, dupla ou circular).
   Detalhes do no ficam escondidos no .c (TAD opaco). */
typedef struct lista Lista;

Lista *lista_criar(void);
void   lista_inserir(Lista *l, Aluno a);
Aluno *lista_buscar(Lista *l, int matricula);   /* NULL se nao achar */
long   lista_tamanho(const Lista *l);
void   lista_liberar(Lista *l);

#endif
