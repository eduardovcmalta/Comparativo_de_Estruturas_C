// Evita falta de clock_gettime em Linux com -std=c11 
#if !defined(_WIN32) && !defined(_POSIX_C_SOURCE)
#define _POSIX_C_SOURCE 199309L
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "utils.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <time.h>
#endif

// Cronometro
double agora(void)
{
#ifdef _WIN32
    LARGE_INTEGER freq, cont;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&cont);
    return (double)cont.QuadPart / (double)freq.QuadPart;
#else
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (double)t.tv_sec + (double)t.tv_nsec / 1e9;
#endif
}

// Leitura da base (formato: matricula;nome;curso) 

static void copiar_campo(char *dest, size_t tam, const char *ini, const char *fim)
{
    size_t n = (size_t)(fim - ini);
    if (n >= tam) n = tam - 1;          // trunca se passar do limite 
    memcpy(dest, ini, n);
    dest[n] = '\0';
}

Aluno *ler_base(const char *caminho, long max, long *lidos)
{
    *lidos = 0;
    FILE *f = fopen(caminho, "r");
    if (!f) { perror("fopen"); return NULL; }

    Aluno *v = malloc((size_t)max * sizeof(Aluno));
    if (!v) {
        fprintf(stderr, "Sem memoria para %ld registros\n", max);
        fclose(f);
        return NULL;
    }

    char linha[512];
    long n = 0;
    while (n < max && fgets(linha, sizeof linha, f)) {
        // tira \r\n do final 
        size_t len = strlen(linha);
        while (len > 0 && (linha[len - 1] == '\n' || linha[len - 1] == '\r'))
            linha[--len] = '\0';

        char *fim_num;
        long mat = strtol(linha, &fim_num, 10);
        if (fim_num == linha || *fim_num != ';') continue;   // linha invalida 

        const char *ini_nome  = fim_num + 1;
        const char *pv        = strchr(ini_nome, ';');
        if (!pv) continue;
        const char *ini_curso = pv + 1;

        Aluno *a = &v[n];
        a->matricula = (int)mat;
        copiar_campo(a->nome,  TAM_NOME,  ini_nome,  pv);
        copiar_campo(a->curso, TAM_CURSO, ini_curso, ini_curso + strlen(ini_curso));
        n++;
    }
    fclose(f);
    *lidos = n;
    return v;
}

// Sorteio das matriculas                                           
// Gerador proprio (xorshift64*): o rand() do Windows tem RAND_MAX  
// de apenas 32767 e muda de sistema para sistema. Assim o sorteio e
// identico em qualquer maquina com a mesma semente.                
static uint64_t xorshift64s(uint64_t *s)
{
    uint64_t x = *s;
    x ^= x >> 12;
    x ^= x << 25;
    x ^= x >> 27;
    *s = x;
    return x * 2685821657736338717ULL;
}

int *sortear_matriculas(const Aluno *base, long n_inseridos, int qtd, unsigned int semente)
{
    int *r = malloc((size_t)qtd * sizeof(int));
    if (!r) return NULL;

    uint64_t estado = 0x9E3779B97F4A7C15ULL ^ ((uint64_t)semente * 0xBF58476D1CE4E5B9ULL);
    if (estado == 0) estado = 88172645463325252ULL;

    for (int i = 0; i < qtd; i++) {
        long idx = (long)(xorshift64s(&estado) % (uint64_t)n_inseridos);
        r[i] = base[idx].matricula;
    }
    return r;
}

// Menor primo >= n                                                    
static int eh_primo(unsigned long long n)
{
    if (n < 2) return 0;
    if (n % 2 == 0) return n == 2;
    for (unsigned long long d = 3; d * d <= n; d += 2)
        if (n % d == 0) return 0;
    return 1;
}

unsigned int proximo_primo(unsigned int n)
{
    unsigned long long c = n < 2 ? 2 : n;
    while (!eh_primo(c)) c++;
    return (unsigned int)c;
}
