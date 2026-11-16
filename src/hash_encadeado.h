#ifndef HASH_ENCADEADO_H
#define HASH_ENCADEADO_H
#include "aluno.h"
#include "hash_funcs.h"

/* Tabela hash com encadeamento separado (lista encadeada por posicao). */
typedef struct hash_encadeado HashEncadeado;

HashEncadeado *he_criar(unsigned int m, FuncaoHash f);
void           he_inserir(HashEncadeado *h, Aluno a);
Aluno         *he_buscar(HashEncadeado *h, int matricula);
long           he_colisoes(const HashEncadeado *h);
void           he_zerar_colisoes(HashEncadeado *h);
double         he_fator_carga(const HashEncadeado *h);  /* pode ser > 1 */
void           he_liberar(HashEncadeado *h);

#endif
