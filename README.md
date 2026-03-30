# Scientific Conference Assignment Tool

Ferramenta de linha de comandos para atribuição automática de revisões de artigos científicos a revisores, desenvolvida no âmbito do Projeto I de **Análise e Síntese de Algoritmos (DA)** — L.EIC016, Spring 2026.

O problema é formulado como uma rede de fluxo e resolvido com o algoritmo de **Max-Flow de Edmonds-Karp**.

---

## Estrutura do Projeto

O projeto está organizado da seguinte forma:

```
src/
├── main.cpp           - Ponto de entrada e menu interativo / modo batch
├── Parser.h/.cpp      - Leitura e validação do ficheiro CSV de entrada
├── Assignment.h/.cpp  - Geração de atribuições e análise de risco
├── Graph.h            - Estrutura de grafo (lista de adjacência)
├── MaxFlow.h/.cpp     - Algoritmo de Edmonds-Karp
├── Submission.h       - Estrutura de dados de uma submissão
├── Reviewer.h         - Estrutura de dados de um revisor
├── Parameters.h       - Parâmetros do problema (min/max revisões)
└── Control.h          - Configurações de controlo (modo, risco, output)
```

---

## Como Compilar e Executar

### Modo Interativo
```bash
./assignTool input.csv
```

### Modo Batch
```bash
./assignTool -b input.csv risk.csv
```
- `input.csv` — ficheiro de entrada com submissões, revisores e parâmetros
- `risk.csv` — ficheiro de saída para a análise de risco (opcional)

---

## Formato do Ficheiro de Entrada

O ficheiro de entrada é um CSV estruturado em secções delimitadas por `#`. Os campos são separados por vírgulas e tudo após `#` numa linha é ignorado.

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

### `GenerateAssignments`
Controla que domínios são considerados na atribuição:

| Valor | Descrição |
|-------|-----------|
| 0 | Atribuição calculada mas não reportada |
| 1 | Apenas domínios **primários** de submissões e revisores |
| 2 | Domínios primário **e secundário** da submissão; apenas primário do revisor |
| 3 | Todos os domínios primários e secundários de ambos os lados |

### `RiskAnalysis`
Determina o tipo de análise de risco:

| Valor | Descrição |
|-------|-----------|
| 0 | Sem análise de risco |
| 1 | Identifica revisores cuja ausência individual inviabiliza a atribuição |
| K > 1 | Identifica combinações de K revisores cuja ausência conjunta inviabiliza a atribuição |

---

## Tarefas Implementadas

### T1 — Configuração Base
- **T1.1** Menu interativo e modo batch (`-b`)
- **T1.2** Leitura e parsing do ficheiro CSV de entrada
- **T1.3** Documentação Doxygen com análise de complexidade temporal

### T2 — Atribuição de Revisões
- **T2.1** Formulação Max-Flow sem análise de risco — `O(V · E²)` (Edmonds-Karp)
- **T2.2** Análise de risco com K=1 — `O(N · V · E²)`
- **T2.3** Análise de risco com K>1 — `O(C(N,K) · V · E²)` (enumeração de combinações)
- **T2.4** Formulação geral com domínios primários e secundários — abordagem Min-Cost Max-Flow discutida

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

Um fluxo máximo igual a `|Submissões| × MinReviewsPerSubmission` indica uma atribuição completa e válida. Caso contrário, as submissões com revisões insuficientes são reportadas no ficheiro de saída.

---

## Análise de Risco

Para **K=1**, cada revisor é temporariamente removido da rede (capacidade da sua aresta para o Sink colocada a 0) e o Max-Flow é re-executado. Se o fluxo resultante for inferior ao necessário, esse revisor é considerado crítico.

Para **K>1**, todas as combinações `C(N,K)` de K revisores são enumeradas com `std::next_permutation`. A complexidade é exponencial em K, tornando-se inviável para valores elevados. Uma abordagem alternativa baseada em **corte mínimo de vértices** é discutida na documentação de `Assignment::generateRiskAnalysis`.

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

---

## Autores

Projeto desenvolvido para a unidade curricular **L.EIC016 — Design of Algorithms**, Faculdade de Engenharia da Universidade do Porto (FEUP), Spring 2026.