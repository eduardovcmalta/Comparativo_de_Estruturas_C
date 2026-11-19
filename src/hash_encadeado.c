#include <stdio.h>
#include <stdlib.h>
#include "hash_encadeado.h"

typedef struct no {
    Aluno aluno;
    struct no *prox;
} No;

// Estrutura principal da Tabela Hash Encadeada
struct hash_encadeado {
    unsigned int m;         // Tamanho da tabela (número de baldes/buckets)
    No **tabela;            // Vetor de ponteiros para os nós (listas encadeadas)
    FuncaoHash funcao_hash; // Função hash escolhida
    long total_colisoes;    // Contador de colisões acumuladas
    long total_elementos;   // Quantidade de elementos inseridos (para fator de carga)
};

// Cria a tabela hash encadeada
HashEncadeado* he_criar(unsigned int m, FuncaoHash f) {
    HashEncadeado *h = (HashEncadeado*) malloc(sizeof(HashEncadeado));
    if (!h) return NULL;

    h->m = m;
    h->funcao_hash = f;
    h->total_colisoes = 0;
    h->total_elementos = 0;

    h->tabela = (No**) calloc(m, sizeof(No*));
    if (!h->tabela) {
        free(h);
        return NULL;
    }

    return h;
}

// Insere um aluno na tabela hash (tratamento por encadeamento separado)
void he_inserir(HashEncadeado *h, Aluno a) {
    if (!h) return;

    // Calcula o índice usando a função hash fornecida
    unsigned int indice = h->funcao_hash(a.matricula, h->m);

    // Verifica se já existe elemento nessa posição (o que caracteriza colisão)
    if (h->tabela[indice] != NULL) {
        h->total_colisoes++;
    }

    No *novo = (No*) malloc(sizeof(No));
    if (!novo) return;
    novo->aluno = a;
    novo->prox = h->tabela[indice]; // Insere no início da lista daquele balde

    h->tabela[indice] = novo;
    h->total_elementos++;
}

// Busca um aluno pela matrícula na tabela hash
Aluno* he_buscar(HashEncadeado *h, int matricula) {
    if (!h) return NULL;

    unsigned int indice = h->funcao_hash(matricula, h->m);
    No *atual = h->tabela[indice];

    // Percorre a lista encadeada daquele índice
    while (atual != NULL) {
        if (atual->aluno.matricula == matricula) {
            return &(atual->aluno);
        }
        atual = atual->prox;
    }

    return NULL; // Não encontrado
}

// Retorna o total de colisões registradas
long he_colisoes(const HashEncadeado *h) {
    return h ? h->total_colisoes : 0; //forma abreviada de escrever if/else
}

// Zera o contador de colisões
void he_zerar_colisoes(HashEncadeado *h) {
    if (h) {
        h->total_colisoes = 0;
    }
}

// Calcula o fator de carga atual (alfa = n / m)
double he_fator_carga(const HashEncadeado *h) {
    if (!h || h->m == 0) return 0.0;
    return (double) h->total_elementos / h->m;
}

// Libera toda a memória alocada pela tabela hash
void he_liberar(HashEncadeado *h) {
    if (!h) return;

    for (unsigned int i = 0; i < h->m; i++) {
        No *atual = h->tabela[i];
        while (atual != NULL) {
            No *proximo = atual->prox;
            free(atual);
            atual = proximo;
        }
    }

    free(h->tabela);
    free(h);
}