#ifndef HASH_FUNCS_H
#define HASH_FUNCS_H

/* Toda funcao hash recebe a chave e o tamanho da tabela (m)
   e devolve um indice em [0, m-1]. */
typedef unsigned int (*FuncaoHash)(int chave, unsigned int m);

/* 1a funcao: metodo da divisao -> chave % m (m deve ser primo) */
unsigned int hash_divisao(int chave, unsigned int m);

/* 2a funcao: outro metodo pesquisado (sugestao: FNV-1a sobre os 4 bytes
   da chave, ou folding/multiplicacao com constante diferente). */
unsigned int hash_alternativa(int chave, unsigned int m);

#endif
