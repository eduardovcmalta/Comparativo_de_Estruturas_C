# Projeto 2ºGQ: Comparação de Estruturas de Dados

Comparação de desempenho entre estruturas de dados para **inserção** e **busca por matrícula** em um Cadastro Nacional de Estudantes com até **10 milhões de registros**, implementada em C.

**Disciplina:** Estrutura de Dados Lineares  
**Professor:** Paulo Henrique  
**Equipe:**
- Eduardo Malta
- Darwin Gabriel
- Arthur Holanda

---

## 1. Base de dados

Arquivo TXT, um registro por linha, campos separados por `;`:

```
86694336;Valentina Martins;Farmacia
50140796;Joao Barbosa;Engenharia Eletrica
```

| Campo     | Tipo       | Observação              |
|-----------|------------|-------------------------|
| matrícula | `int`      | única, usada como chave |
| nome      | `char[64]` | nome completo           |
| curso     | `char[32]` | nome do curso           |

Na base completa, o maior nome tem 20 caracteres e o maior curso tem 23. Os limites de 64 e 32 deixam folga.

A base **não** acompanha o repositório (tamanho acima do limite do GitHub e listada no `.gitignore`). Coloque o arquivo na raiz com o nome `base.txt` ou informe outro caminho na execução.

Existe um **único arquivo de base**. Os volumes de 100 mil, 1 milhão e 10 milhões de registros não são arquivos separados: são os **primeiros N registros desse mesmo arquivo**, na ordem original. O programa lê apenas os N primeiros, então o conjunto de 100 mil está contido no de 1 milhão, que está contido no de 10 milhões.

## 2. Implementações

Todas usam alocação dinâmica e são organizadas como TAD (tipo opaco no `.h`, detalhes no `.c`).

| Módulo | Descrição |
|---|---|
| `aluno.h` | Struct do registro (matrícula, nome, curso) |
| `lista.c/.h` | Estrutura linear: **fila circular unicamente encadeada** |
| `hash_aberto.c/.h` | Tabela hash com endereçamento aberto e **sondagem linear** |
| `hash_encadeado.c/.h` | Tabela hash com encadeamento separado (lista encadeada por posição) |
| `hash_funcs.c/.h` | Funções hash (ver abaixo) |
| `utils.c/.h` | Leitura do TXT, sorteio de matrículas, cronômetro, cálculo de primo |
| `main.c` | Driver dos experimentos |

### Estrutura linear: fila circular unicamente encadeada

Cada nó aponta apenas para o próximo, e o último nó aponta de volta para o primeiro. A fila mantém um ponteiro para o **último** nó (`fim`), de modo que o primeiro é sempre `fim->prox`. Assim:

- **Inserção** no fim em O(1), sem percorrer a estrutura, preservando a ordem de chegada dos registros (comportamento de fila);
- **Busca** em O(n): percorre os nós a partir do primeiro até voltar ao ponto de partida.

### Hash com endereçamento aberto: sondagem linear

Em caso de colisão, testa as posições seguintes: `(h(k) + i) mod m`, com `i = 0, 1, 2, ...`. Foi escolhida por ser a mais simples e por tornar visível o **agrupamento primário** (clusters) quando o fator de carga cresce, o que enriquece a análise (α final ≈ 0,77).

### Funções hash

1. **`hash_divisao`**: método da divisão, `h(k) = k mod m`, com `m` primo.
2. **`hash_alternativa`**: **FNV-1a** (32 bits) aplicado aos 4 bytes da chave, com redução final `mod m`. É um hash não criptográfico amplamente usado, que mistura os bits da chave (XOR seguido de multiplicação pelo primo `16777619`, partindo do valor inicial `2166136261`). A intenção é comparar a divisão simples, que depende diretamente do padrão numérico das matrículas, com uma função que espalha melhor os bits.

As duas funções são testadas com **sondagem** e com **encadeamento separado**, gerando 4 configurações de tabela hash, além da fila:

| # | Configuração |
|---|---|
| 1 | Fila circular encadeada |
| 2 | Endereçamento aberto (linear) + divisão |
| 3 | Endereçamento aberto (linear) + FNV-1a |
| 4 | Encadeamento separado + divisão |
| 5 | Encadeamento separado + FNV-1a |

## 3. Metodologia dos experimentos

- **Volumes:** 100 mil, 1 milhão e 10 milhões de registros, sempre os primeiros N registros do mesmo `base.txt`, na ordem original.
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
- **Cronômetro:** `QueryPerformanceCounter` no Windows e `clock_gettime(CLOCK_MONOTONIC)` no Linux, ambos de alta resolução e monotônicos.
- **Sorteio reprodutível:** as matrículas são sorteadas com um gerador próprio (xorshift64*) e semente fixa, em vez de `rand()`. O `rand()` do Windows só chega a 32767 e varia entre sistemas, o que quebraria a reprodutibilidade com 10 milhões de registros.

### Tamanho da tabela hash

`m` = menor número primo maior ou igual a `1,3 × N`.

- **Endereçamento aberto** exige `α ≤ 1`, então `m` precisa ser maior que `N`.
- Com `m ≈ 1,3 N`, o `α` final fica em torno de **0,77**: a tabela comporta todos os registros e ainda mostra o efeito das colisões e do agrupamento.
- Usar primo melhora a distribuição do método da divisão.
- O mesmo `m` nas 4 configurações permite comparar funções hash e estratégias de colisão em igualdade de condições.

| Volume (N) | m | α final |
|---|---|---|
| 100.000 | 130.003 | 0,7692 |
| 1.000.000 | 1.300.021 | 0,7692 |
| 10.000.000 | 13.000.027 | 0,7692 |

## 4. Compilação e execução

### Windows (PowerShell)

**Requisito:** `gcc` 64 bits (MinGW-w64). Instalação:

```powershell
winget install MSYS2.MSYS2
```

Abra o terminal **MSYS2 UCRT64** e rode:

```bash
pacman -S mingw-w64-ucrt-x86_64-gcc
```

Adicione `C:\msys64\ucrt64\bin` ao `PATH` do Windows, reabra o PowerShell e confirme com `gcc --version`. Confirme também que o compilador é de 64 bits: `gcc -dumpmachine` deve mostrar `x86_64-w64-mingw32`. Um `gcc` de 32 bits (o MinGW antigo mostra apenas `mingw32`) fica sem memória no volume de 10 milhões.

**Compilar** (na raiz do repositório):

```powershell
gcc -O2 -Wall -Wextra -std=c11 -o experimento.exe (Get-ChildItem src/*.c)
```

**Executar** (a pasta `resultados/` já existe no repositório). As três execuções usam o mesmo `base.txt`; o segundo argumento é quantos registros ler (N), e cada execução grava o CSV do seu volume:

```powershell
.\experimento.exe base.txt 100000   resultados/100k.csv
.\experimento.exe base.txt 1000000  resultados/1m.csv
.\experimento.exe base.txt 10000000 resultados/10m.csv

# 10M sem a fila (se a busca linear demorar demais)
.\experimento.exe base.txt 10000000 resultados/10m.csv sem_lista
```

### Linux / WSL

```bash
make                  # compila
make run100k          # ou run1m, run10m
make clean
```

### Testes da fila

Windows (PowerShell):

```powershell
gcc -std=c11 -Wall -Wextra -g -Isrc -o teste_lista.exe tests/teste_lista.c src/lista.c
.\teste_lista.exe
```

Linux / WSL:

```bash
gcc -std=c11 -Wall -Wextra -g -Isrc -o teste_lista tests/teste_lista.c src/lista.c && ./teste_lista
```

O resultado esperado termina com `0 falha(s)`.

### Observações

- Use a **mesma máquina e o mesmo método de execução** em todos os experimentos, para que os tempos sejam comparáveis. Feche outros programas pesados durante as medições.
- **Memória:** com 10 milhões de registros, a base em memória e as estruturas ocupam vários GB. O programa executa uma implementação por vez e libera tudo antes da próxima, mas confira a RAM disponível antes de rodar o volume de 10M. É necessário compilar em **64 bits**.
- Se estiver usando o OneDrive, prefira manter o repositório fora da pasta sincronizada para evitar travamentos do Git.

### Formato da saída (CSV)

```
volume,m,estrutura,etapa_pct,tempo_insercao_s,tempo_busca_s,colisoes,fator_carga
```

Para a fila, `colisoes` e `fator_carga` saem como `-1` (não se aplicam).

## 5. Resultados

Resumo dos resultados (tempos em segundos). Os CSVs completos, com as etapas de 30%, 60% e 100%, estão em [`resultados/`](resultados/) e os slides em [`slides/apresentacao.pdf`](slides/apresentacao.pdf).

### Tempo de busca (10.000 buscas, etapa de 100%)

| Estrutura | 100 mil | 1 milhão | 10 milhões |
|---|---|---|---|
| Fila circular | 2,83 | 61,34 | 627,99 |
| Aberto + divisão | 0,0007 | 0,0005 | 0,0005 |
| Aberto + FNV-1a | 0,0004 | 0,0006 | 0,0007 |
| Encadeado + divisão | 0,0002 | 0,0004 | 0,0005 |
| Encadeado + FNV-1a | 0,0008 | 0,0009 | 0,0008 |

### Tempo de inserção (soma das três etapas)

| Estrutura | 100 mil | 1 milhão | 10 milhões |
|---|---|---|---|
| Fila circular | 0,0051 | 0,0577 | 0,5749 |
| Aberto + divisão | 0,0059 | 0,0725 | 0,7755 |
| Aberto + FNV-1a | 0,0054 | 0,0683 | 0,8296 |
| Encadeado + divisão | 0,0069 | 0,1017 | 1,06 |
| Encadeado + FNV-1a | 0,0061 | 0,1013 | 1,11 |

### Colisões (soma das três etapas) e fator de carga

| Configuração | 100 mil | 1 milhão | 10 milhões |
|---|---|---|---|
| Aberto + divisão | 30.166 | 299.655 | 2.755.476 |
| Aberto + FNV-1a | 30.091 | 301.115 | 2.933.519 |
| Encadeado + divisão | 30.166 | 299.655 | 2.755.476 |
| Encadeado + FNV-1a | 30.091 | 301.115 | 2.933.519 |
| Fator de carga final | 0,7692 | 0,7692 | 0,7692 |
| Tamanho da tabela (m) | 130.003 | 1.300.021 | 13.000.027 |

### Conclusões

- **Busca:** a fila cresce com o volume (2,83 s, 61,3 s e 628 s) e quase linearmente com o número de registros dentro de cada volume. As tabelas hash levam de 0,0001 s a 0,0009 s em qualquer volume, cerca de um milhão de vezes menos que a fila no 10M.
- **Inserção:** cresce de forma aproximadamente linear com N. O endereçamento aberto é mais rápido que o encadeamento separado, que paga uma alocação por nó.
- **Funções hash:** colisões quase iguais em 100 mil e 1 milhão (diferença menor que 0,5%). No 10M a divisão teve cerca de 6% menos colisões que o FNV-1a.
- **Melhor combinação:** endereçamento aberto com divisão foi a mais consistente, mas as diferenças entre as quatro hashes são pequenas.
- **Ressalva:** as buscas nas hashes levam menos de 1 ms, e diferenças abaixo de cerca de 0,0003 s estão perto do limite de medição.

## 6. Estrutura do repositório

```
.
├── src/            código-fonte (módulos .c/.h e main.c)
├── resultados/     CSVs gerados pelos experimentos
├── slides/         apresentação
├── tests/          testes da fila circular
├── Makefile        compilação no Linux/WSL
├── .gitignore
└── README.md
```

## 7. Organização do trabalho

| Responsabilidade | Responsável |
|---|---|
| `aluno`, `utils`, fila circular (`lista`), `main.c` | Eduardo Malta |
| `hash_aberto`, `hash_funcs` | Darwin Gabriel |
| `hash_encadeado`, README, slides | Arthur Holanda |
