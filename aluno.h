#ifndef ALUNO_H
#define ALUNO_H

/* Ajustar apos conferir o maior nome/curso presente no TXT. */
#define TAM_NOME  64
#define TAM_CURSO 32

typedef struct {
    int  matricula;
    char nome[TAM_NOME];
    char curso[TAM_CURSO];
} Aluno;

#endif
