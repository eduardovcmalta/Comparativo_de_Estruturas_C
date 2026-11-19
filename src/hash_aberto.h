#ifndef HASH_ABERTO_H
#define HASH_ABERTO_H
#include "aluno.h"
#include "hash_funcs.h"

/* Tabela hash com enderecamento aberto (linear, quadratica ou dupla). */
typedef struct hash_aberto HashAberto;

HashAberto *ha_criar(unsigned int m, FuncaoHash f);
int         ha_inserir(HashAberto *h, Aluno a);   /* 1 ok, 0 tabela cheia */
Aluno      *ha_buscar(HashAberto *h, int matricula);
long        ha_colisoes(const HashAberto *h);     /* acumuladas desde o ultimo zerar */
void        ha_zerar_colisoes(HashAberto *h);     /* chamar a cada intervalo (30/60/100%) */
double      ha_fator_carga(const HashAberto *h);  /* n / m */
void        ha_liberar(HashAberto *h);

#endif