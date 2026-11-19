#include "hash_funcs.h"

unsigned int hash_divisao(int chave, unsigned int m) {
    if (chave < 0) {
        chave = -chave;
    }
    return (unsigned int)(chave % m);
}

// Metodo da dobra
unsigned int hash_alternativa(int chave, unsigned int m) {
    if (chave < 0) {
        chave = -chave;
    }
    
    unsigned int parte1 = chave / 1000;
    unsigned int parte2 = chave % 1000;
    unsigned int soma = parte1 + parte2;
    
    return soma % m;
}