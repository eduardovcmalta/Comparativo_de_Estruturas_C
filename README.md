# Projeto 2ºGQ: Comparação de Estruturas de Dados

Comparação de desempenho entre estruturas de dados para **inserção** e **busca por matrícula** em um Cadastro Nacional de Estudantes com até **10 milhões de registros**, implementada em C.

**Disciplina:** _<preencher>_  
**Professor(a):** _<preencher>_  
**Equipe:**
- _<Eduardo>_
- _<Dariwn >_
- _<Arthur >_

---

## 1. Base de dados

Arquivo TXT, um registro por linha, campos separados por `;`:

```
86694336;Valentina Martins;Farmacia
50140796;Joao Barbosa;Engenharia Eletrica
```

| Campo      | Tipo         | Observação                  |
|------------|--------------|-----------------------------|
| matrícula  | `int`        | única, usada como chave     |
| nome       | `char[64]`   | nome completo               |
| curso      | `char[32]`   | nome do curso               |

A base **não** acompanha o repositório (tamanho). Coloque o arquivo na raiz com o nome `base.txt` (ou informe outro caminho na execução).

## 2. Implementações

Todas usam alocação dinâmica e são organizadas como TAD (tipo opaco no `.h`, detalhes no `.c`).

| Módulo | Descrição |
|---|---|
| `aluno.h` | Struct do registro (matrícula, nome, curso) |
| `lista.c/.h` | Estrutura linear: lista _<simples / dupla / circular>_ encadeada |
| `hash_aberto.c/.h` | Tabela hash com endereçamento aberto, sondagem _<linear / quadrática / dupla>_ |
| `hash_encadeado.c/.h` | Tabela hash com encadeamento separado (lista encadeada por posição) |
| `hash_funcs.c/.h` | Funções hash (ver abaixo) |
| `utils.c/.h` | Leitura do TXT, sorteio de matrículas, cronômetro, cálculo de primo |
| `main.c` | Driver dos experimentos |

### Funções hash

1. **`hash_divisao`**: método da divisão, `h(k) = k mod m`, com `m` primo.
2. **`hash_alternativa`**: _<descrever o método escolhido, ex.: FNV-1a sobre os 4 bytes da chave, e a referência usada>_.

As duas funções são testadas com **sondagem** e com **encadeamento separado**, gerando 4 configurações de tabela hash, além da lista:

| # | Configuração |
|---|---|
| 1 | Lista encadeada |
| 2 | Endereçamento aberto + divisão |
| 3 | Endereçamento aberto + alternativa |
| 4 | Encadeamento separado + divisão |
| 5 | Encadeamento separado + alternativa |

## 3. Metodologia dos experimentos

- **Volumes:** 100 mil, 1 milhão e 10 milhões de registros, sempre na ordem original do arquivo.
- **Etapas:** as 5 estruturas começam vazias. Os registros são inseridos progressivamente, com medições após **30%, 60% e 100%** do volume.
- **Métricas por etapa:**
  - tempo de inserção do intervalo (0–30%, 30–60%, 60–100%);
  - tempo total de **10.000 buscas** por matrículas existentes, sorteadas entre os registros já inseridos;
  - **colisões** no intervalo: vezes em que uma nova matrícula gera um índice hash inicial já produzido por outra;
  - **fator de carga** `α = n / m` (apenas tabelas hash).
- **Condições de justiça:**
  - mesmos registros, na mesma ordem, nas 5 implementações;
  - mesmas 10.000 matrículas (mesma ordem) em cada etapa, sorteadas com semente fixa;
  - leitura do arquivo e sorteio das matrículas **fora** da medição de tempo;
  - as 4 configurações hash usam o mesmo `m`, fixo durante todo o experimento.

### Tamanho da tabela hash

`m` = menor número primo maior ou igual a `1,3 × N`.

- **Endereçamento aberto** exige `α ≤ 1`, então `m` precisa ser maior que `N`.
- Com `m ≈ 1,3 N`, o `α` final fica em torno de **0,77**: a tabela comporta todos os registros e ainda mostra o efeito das colisões e do agrupamento.
- Usar primo melhora a distribuição do método da divisão.
- O mesmo `m` nas 4 configurações permite comparar funções hash e estratégias de colisão em igualdade de condições.

| Volume (N) | m |
|---|---|
| 100.000 | _<preencher após executar>_ |
| 1.000.000 | _<preencher>_ |
| 10.000.000 | _<preencher>_ |

## 4. Compilação e execução

**Requisitos:** `gcc` e `make` em Linux (ou WSL no Windows). O cronômetro usa `clock_gettime`.

```bash
# compilar
make

# executar um volume
./experimento base.txt 100000   resultados/100k.csv
./experimento base.txt 1000000  resultados/1m.csv
./experimento base.txt 10000000 resultados/10m.csv

# atalhos equivalentes
make run100k
make run1m
make run10m

# 10M sem a lista (se a busca linear demorar demais)
./experimento base.txt 10000000 resultados/10m.csv sem_lista

# limpar
make clean
```

**Atenção à memória:** com 10 milhões de registros, a base em memória e as estruturas ocupam vários GB. O programa executa uma implementação por vez e libera tudo antes da próxima, mas confira a RAM disponível antes de rodar o volume de 10M.

### Formato da saída (CSV)

```
volume,m,estrutura,etapa_pct,tempo_insercao_s,tempo_busca_s,colisoes,fator_carga
```

Para a lista, `colisoes` e `fator_carga` saem como `-1` (não se aplicam).

## 5. Resultados

Os CSVs completos estão em [`resultados/`](resultados/) e os slides em [`slides/`](slides/).

_<Inserir aqui as tabelas resumidas e as principais conclusões após os experimentos.>_

## 6. Estrutura do repositório

```
.
├── src/            código-fonte (módulos .c/.h e main.c)
├── resultados/     CSVs gerados pelos experimentos
├── slides/         apresentação
├── Makefile
└── README.md
```

## 7. Organização do trabalho

| Integrante | Responsabilidades |
|---|---|
| _<Integrante 1>_ | `aluno`, `utils`, `lista`, `main.c` |
| _<Integrante 2>_ | `hash_aberto`, `hash_funcs` |
| _<Integrante 3>_ | `hash_encadeado`, README, Makefile, slides |
