#include "hash_funcs.h"

unsigned int hash_divisao(int chave, unsigned int m) {
    if (chave < 0) {
        chave = -chave;
    }
    return (unsigned int)(chave % m);
}

// FNV-1a (32 bits) sobre os 4 bytes da chave
unsigned int hash_alternativa(int chave, unsigned int m) {
    unsigned int h = 2166136261u;
    unsigned int v = (unsigned int)chave;
    for (int i = 0; i < 4; i++) {
        h ^= (v >> (8 * i)) & 0xFF;
        h *= 16777619u;
    }
    return h % m;
}