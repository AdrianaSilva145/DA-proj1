# Scientific Conference Assignments Tool

Ferramenta de linha de comandos para atribuição automática de revisões de artigos científicos a revisores, desenvolvida no âmbito do Projecto I de **Desenho de Algoritmos (DA)** — L\.EIC016, Primavera 2026.

O problema é formulado como uma rede de fluxo e resolvido com o algoritmo de **Max-Flow de Edmonds-Karp**.

---

## Estrutura do Projeto

```
src/
├── main.cpp           — Ponto de entrada e menu interativo / modo batch
├── Parser.h/.cpp      — Leitura e validação do ficheiro CSV de entrada
├── Assignment.h/.cpp  — Geração de atribuições e análise de risco
├── Graph.h            — Estrutura de grafo (lista de adjacência)
├── MaxFlow.h          — Algoritmo de Edmonds-Karp
├── Submission.h       — Estrutura de dados de uma submissão
├── Reviewer.h         — Estrutura de dados de um revisor
├── Parameters.h       — Parâmetros do problema (mínimo/máximo de revisões)
└── Control.h          — Configurações de controlo (modo, risco, ficheiro de saída)
```

---

## Como Compilar e Executar

### Modo Interativo
```bash
./assignTool
```

### Modo Batch
```bash
./assignTool -b input.csv risk.csv
```
- input\.csv - ficheiro de entrada com submissões, revisores e parâmetros.
- risk\.csv - ficheiro de saída para a análise de risco (opcional).

---

## Formato do Ficheiro de Entrada

O ficheiro de entrada é um CSV estruturado em secções delimitadas por `#`. Os campos são separados por vírgulas e tudo o que apareça após `#` numa linha é ignorado como comentário.

```
#Submissions
#Id, Title, Authors, E-mail, Primary, Secondary
31, "The Eternal Wheel of Reincarnation", Ralph Kholer, kholer@gmail.com, 3, 4
87, "GoDiva: A PIM Architecture", Jeff Draper, draper@usc.edu, 1,
#
#Reviewers
#Id, Name, E-mail, Primary, Secondary
1, Jaqueline N. Chame, jchame@yahoo.com, 1,
2, Mary W. Hall, mhall@hotmail.edu, 1, 4
#Parameters
MinReviewsPerSubmission, 5
MaxReviewsPerReviewer, 4
PrimaryReviewerExpertise, 1
SecondaryReviewerExpertise, 0
PrimarySubmissionDomain, 1
SecondarySubmissionDomain, 1
#Control
GenerateAssignments, 1
RiskAnalysis, 0
OutputFileName, "assign.csv"
```

---

## Parâmetros de Controlo

### GenerateAssignments

Controla que domínios são considerados na atribuição:

| Valor | Descrição |
|-------|-----------|
| 0 | Atribuição calculada mas não reportada no ficheiro de saída. |
| 1 | Apenas domínios **primários** de submissões e revisores. |
| 2 | Domínios **primário e secundário** da submissão; apenas primário do revisor. |
| 3 | Todos os domínios primários e secundários de ambos os lados. |

### RiskAnalysis

Determina o tipo de análise de risco:

| Valor | Descrição |
|-------|-----------|
| 0 | Sem análise de risco. |
| 1 | Identifica revisores cuja ausência individual inviabiliza a atribuição completa. |
| K > 1 | Identifica combinações de K revisores cuja ausência conjunta inviabiliza a atribuição completa. |

---

## Tarefas Implementadas

### T1 — Configuração Base
- Menu interativo e modo batch (`-b`).
- Leitura e parsing do ficheiro CSV de entrada.
- Documentação Doxygen com análise de complexidade temporal.

### T2 — Atribuição de Revisões
- Formulação Max-Flow sem análise de risco — `O(V · E²)` (Edmonds-Karp).
- Análise de risco com K=1 — `O(N · V · E²)`.
- Análise de risco com K>1 — `O(C(N,K) · V · E²)`, por enumeração de combinações com `std::next_permutation`.
- Formulação geral com domínios primários e secundários (modos 2 e 3).

---

## Formulação do Problema como Max-Flow

A rede de fluxo é construída da seguinte forma:

```
Source ──(cap = MinReviewsPerSubmission)──► [Submissão i]
                                                  │
                                         (cap = 1, se domínio compatível)
                                                  │
                                                  ▼
                                           [Revisor j] ──(cap = MaxReviewsPerReviewer)──► Sink
```

Cada vértice de submissão recebe capacidade `MinReviewsPerSubmission` a partir da source. Cada par submissão–revisor compatível é ligado por uma aresta de capacidade 1. Cada revisor tem uma aresta de saída para o sink com capacidade `MaxReviewsPerReviewer`.

Um fluxo máximo igual a `|Submissões| × MinReviewsPerSubmission` indica uma atribuição completa e válida. Caso contrário, as submissões com revisões insuficientes são reportadas no ficheiro de saída.

Os nós de submissão recebem identificadores `submissionId + 1000` e os de revisor `reviewerId + 2000`, de forma a evitar colisões com os nós especiais `-1` (source) e `-2` (sink).

---

## Análise de Risco

A análise de risco determina quais os revisores (ou conjuntos de K revisores) cuja ausência impede que todas as submissões recebam o número mínimo de revisões exigido.

O algoritmo funciona da seguinte forma para cada combinação de K revisores:

1. Para cada revisor na combinação, a capacidade da aresta revisor→sink é colocada a 0, simulando a sua ausência.
2. Todos os fluxos do grafo são repostos a zero.
3. O algoritmo de Edmonds-Karp é executado novamente sobre o grafo modificado.
4. O fluxo obtido é comparado com o fluxo esperado (`|Submissões| × MinReviewsPerSubmission`). Se for inferior, essa combinação de revisores é considerada crítica.
5. A capacidade da aresta revisor→sink é restaurada ao valor original (`MaxReviewsPerReviewer`) antes de se passar à combinação seguinte.

As `C(N, K)` combinações são enumeradas através de um vector seletor booleano de tamanho N (número de revisores), com K posições verdadeiras, iterando por todas as permutações com `std::next_permutation`.

### Complexidade

- **K = 1:** `O(N · V · E²)`, onde N é o número de revisores, V o número de vértices e E o número de arestas do grafo de fluxo.
- **K > 1:** `O(C(N, K) · V · E²)` — cresce exponencialmente com K, tornando-se inviável para valores elevados de K.

---

## Formato do Ficheiro de Saída

### Atribuição bem-sucedida

```
#SubmissionId,ReviewerId,Match
31, 2, 4
87, 1, 1
#ReviewerId,SubmissionId,Match
1, 87, 1
2, 31, 4
#Total: 2
```

### Atribuição incompleta

Quando o fluxo máximo é inferior ao necessário, o ficheiro de saída inclui adicionalmente:

```
#SubmissionId,Domain,MissingReviews
31, 3, 2
87, 1, 2
```

### Análise de risco

```
#Risk Analysis: 1
1, 2
```

Para K > 1, cada linha contém os identificadores dos K revisores que formam uma combinação crítica.

---

## Documentação das Classes e Estruturas

### Submission

Representa uma submissão de artigo científico. Cada submissão tem um identificador único, título, autor(es), endereço de correio eletrónico e um ou dois tópicos científicos (domínio primário obrigatório, domínio secundário opcional representado por -1 quando ausente).

### Reviewer

Representa um revisor científico. Cada revisor tem um identificador único, nome, endereço de correio eletrónico e uma ou duas áreas de especialização (primária obrigatória, secundária opcional representada por -1 quando ausente).

### Parameters

Estrutura que agrega os parâmetros numéricos do problema, lidos da secção `#Parameters` do ficheiro CSV: número mínimo de revisões por submissão, número máximo de revisões por revisor, e os pesos associados a domínios primários e secundários de submissões e revisores.

### Control

Estrutura que agrega as configurações de controlo do programa, lidas da secção `#Control` do ficheiro CSV: modo de geração de atribuições (GenerateAssignments), nível de análise de risco (RiskAnalysis) e nome do ficheiro de saída (OutputFileName).

### Parser

Classe responsável pela leitura e parsing do ficheiro CSV de entrada. Itera o ficheiro linha a linha, identificando a secção corrente pelo cabeçalho (`#Submissions`, `#Reviewers`, `#Parameters`, `#Control`) e processando cada linha de dados em conformidade. Os comentários (linhas iniciadas por `#` ou conteúdo após `#`) e as linhas vazias são ignorados. Os campos opcionais (domínio secundário) são representados por -1 quando ausentes.

**Complexidade temporal:** `O(L · F)`, onde L é o número de linhas do ficheiro e F o número de campos por linha.

### Graph<T>

Estrutura de grafo genérico com lista de adjacência, parametrizado pelo tipo do identificador dos vértices. Suporta grafos dirigidos, com operações de adição e remoção de vértices e arestas. O método `addEdge()` cria automaticamente a aresta forward e a correspondente aresta residual (reverse com capacidade 0), ligadas pelo ponteiro `reverse`, tornando o grafo imediatamente utilizável para algoritmos de fluxo. Inclui também campos auxiliares para outros algoritmos (Floyd-Warshall, Dijkstra, SCC).

### Vertex<T>

Representa um vértice no grafo. Contém o valor identificador, a lista de arestas de saída (`adj`) e de entrada (`incoming`), e campos auxiliares utilizados por vários algoritmos: `visited` e `processing` para DFS/BFS, `low` e `num` para SCCs (Tarjan), `dist` e `path` para Dijkstra/Prim, e `queueIndex` para a fila de prioridade mutável.

### Edge<T>

Representa uma aresta dirigida no grafo. Contém os vértices de origem e destino, a capacidade (`weight`), o fluxo atual (`flow`), e um ponteiro para a aresta residual (`reverse`). O campo `flow` é inicializado a zero e atualizado pelo algoritmo de Edmonds-Karp.

### MaxFlow

Estrutura que encapsula o algoritmo de Edmonds-Karp para cálculo de fluxo máximo em redes. O algoritmo implementa a variante BFS do método de Ford-Fulkerson: em cada iteração, o BFS encontra o caminho aumentante mais curto (em número de arestas) da source ao sink com capacidade residual positiva, e aumenta o fluxo pelo valor do bottleneck desse caminho. O processo repete-se até não existirem mais caminhos aumentantes.

**Complexidade temporal:** `O(V · E²)`, onde V é o número de vértices e E o número de arestas. Esta complexidade é garantida independentemente das capacidades das arestas, ao contrário do Ford-Fulkerson genérico.

### Assignment

Classe principal que implementa as duas funcionalidades centrais do projeto.

**generateAssignment** constrói a rede de fluxo, executa o Edmonds-Karp e escreve o ficheiro de saída com a atribuição. Suporta os modos 1, 2 e 3 de correspondência de domínios. Em caso de atribuição incompleta, reporta as submissões com revisões em falta. Se `control.riskAnalysis > 0`, executa também a análise de risco e escreve os resultados no mesmo ficheiro de saída.

**Complexidade temporal:** `O(S·R + V·E²)`, onde S é o número de submissões, R o número de revisores, V o número de vértices e E o número de arestas. O passo de Edmonds-Karp é dominante.

**generateRiskAnalysis** reconstrói a rede de fluxo de raiz e executa a análise de risco de forma autónoma, escrevendo os resultados num ficheiro separado. Utiliza o mesmo algoritmo descrito na secção de Análise de Risco acima.

**Complexidade temporal:** `O(C(N, K) · V · E²)`.

### MatchEdge (estrutura interna de Assignment.cpp)

Estrutura auxiliar que representa uma aresta de correspondência entre uma submissão e um revisor. Guarda os índices e identificadores necessários para reconstruir a atribuição após a execução do Max-Flow, juntamente com um ponteiro para a aresta forward correspondente no grafo de fluxo.

### MutablePriorityQueue<T>

Implementação de uma fila de prioridade mutável (min-heap), utilizada pelo algoritmo de Dijkstra. Suporta as operações `insert`, `extractMin` e `decreaseKey`. A classe T parametrizante deve ter o campo `int queueIndex` acessível e o operador `<` definido.

---

## Autores

Projeto desenvolvido para a unidade curricular **L\.EIC016 — Design of Algorithms** por:
- Ana Carolina (up202403634@up.pt)
- Adriana Silva (up202405385@up.pt)
- Mariana Gouveia (up202406702@up.pt)

Faculdade de Engenharia da Universidade do Porto (FEUP), Março 2026.