#ifndef UTILS_H
#define UTILS_H
#include "aluno.h"

/* Le ate 'max' registros do TXT, na ordem original.
   Devolve vetor alocado e preenche *lidos. (Fora da medicao de tempo.) */
Aluno *ler_base(const char *caminho, long max, long *lidos);

/* Sorteia 'qtd' matriculas entre os 'n_inseridos' primeiros registros,
   com semente fixa, para reutilizar a mesma lista nas 5 implementacoes. */
int *sortear_matriculas(const Aluno *base, long n_inseridos, int qtd, unsigned int semente);

/* Cronometro (clock_gettime com CLOCK_MONOTONIC), retorna segundos. */
double agora(void);

/* Menor primo >= n, para escolher o tamanho da tabela hash. */
unsigned int proximo_primo(unsigned int n);

#endif
