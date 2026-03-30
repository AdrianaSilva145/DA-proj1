#include "Assignment.h"
#include <fstream>
#include <vector>
#include <algorithm>

/**
 * @brief Determina o domínio de correspondência entre uma submissão e um revisor.
 *
 * Verifica se existe compatibilidade de domínios entre a submissão e o revisor,
 * de acordo com o modo de correspondência especificado:
 * - Modo 1: apenas domínios primários de ambos os lados.
 * - Modo 2: domínio primário ou secundário da submissão contra o domínio primário do revisor.
 * - Modo 3: todos os domínios primários e secundários de ambos os lados.
 *
 * @param s    Submissão a verificar.
 * @param r    Revisor a verificar.
 * @param mode Modo de correspondência (1, 2 ou 3).
 * @return O número do domínio correspondente (> 0) se houver compatibilidade; 0 caso contrário.
 *
 * @note Complexidade temporal: O(1).
 */
static int match(const Submission &s, const Reviewer &r, int mode) {
    if (mode == 1) {
        if (s.primaryTopic == r.primaryExpertise) return s.primaryTopic;
    }
    else if (mode == 2) {
        if (s.primaryTopic == r.primaryExpertise) return s.primaryTopic;
        if (s.secondaryTopic != -1 && s.secondaryTopic == r.primaryExpertise) return s.secondaryTopic;
    }
    else if (mode == 3) {
        if (s.primaryTopic == r.primaryExpertise) return s.primaryTopic;
        if (s.secondaryTopic != -1 && s.secondaryTopic == r.primaryExpertise) return s.secondaryTopic;
        if (r.secondaryExpertise != -1 && s.primaryTopic == r.secondaryExpertise) return s.primaryTopic;
        if (s.secondaryTopic != -1 && r.secondaryExpertise != -1 && s.secondaryTopic == r.secondaryExpertise) return s.secondaryTopic;
    }
    return 0;
}

/**
 * @brief Repõe todos os fluxos do grafo a zero.
 *
 * Percorre todos os vértices e todas as arestas do grafo, repondo o fluxo a zero
 * em cada aresta forward (peso > 0) e na respectiva aresta residual. Utilizado
 * antes de cada re-execução do Edmonds-Karp na análise de risco.
 *
 * @param g Grafo de fluxo a repor.
 *
 * @note Complexidade temporal: O(V + E), onde V = vértices e E = arestas do grafo.
 */
static void resetFlows(Graph<int> &g) {
    for (auto v : g.getVertexSet()) {
        for (auto e : v->getAdj()) {
            if (e->getWeight() > 0) {
                e->setFlow(0);
                if (e->getReverse())
                    e->getReverse()->setFlow(0);
            }
        }
    }
}

/**
 * @brief Estrutura interna que representa uma aresta de correspondência submissão–revisor.
 *
 * Guarda os índices e identificadores necessários para reconstruir a atribuição após
 * a execução do fluxo máximo, juntamente com um ponteiro para a aresta forward
 * correspondente no grafo de fluxo.
 */
struct MatchEdge {
    int submissionIndex; ///< Índice no vector de submissões.
    int reviewerIndex;   ///< Índice no vector de revisores ordenados.
    int reviewerId;      ///< Identificador real do revisor (dos dados de entrada).
    int matchDomain;     ///< Número do domínio correspondente.
    Edge<int>* edge;     ///< Ponteiro para a aresta forward no grafo de fluxo.

    /**
     * @brief Constrói um MatchEdge com todos os campos.
     * @param si  Índice da submissão.
     * @param ri  Índice do revisor.
     * @param rid Identificador do revisor.
     * @param d   Domínio correspondente.
     * @param e   Ponteiro para a aresta de fluxo.
     */
    MatchEdge(int si, int ri, int rid, int d, Edge<int>* e)
        : submissionIndex(si), reviewerIndex(ri), reviewerId(rid), matchDomain(d), edge(e) {}
};

/**
 * @brief Liberta toda a memória alocada para o grafo de fluxo.
 *
 * Efectua a libertação em duas passagens para evitar use-after-free: primeiro
 * elimina todas as arestas de cada vértice e só depois elimina os vértices.
 *
 * @param g Grafo de fluxo a libertar.
 *
 * @note Complexidade temporal: O(V + E), onde V = vértices e E = arestas do grafo.
 */
static void freeGraph(Graph<int> &g) {
    for (auto v : g.getVertexSet())
        for (auto e : v->getAdj())
            delete e;
    for (auto v : g.getVertexSet())
        delete v;
}

bool Assignment::generateAssignment(const std::vector<Submission>& submissions,
                                    const std::vector<Reviewer>& reviewers,
                                    const Parameters& params,
                                    const Control& control,
                                    const std::string& outputFile)
{
    // Ordenar revisores por ID para garantir ordem determinística na rede de fluxo.
    std::vector<Reviewer> sortedReviewers = reviewers;
    std::sort(sortedReviewers.begin(), sortedReviewers.end(),
              [](const Reviewer &a, const Reviewer &b) { return a.id < b.id; });

    Graph<int> g;
    const int SOURCE = -1;
    const int SINK = -2;

    g.addVertex(SOURCE);
    g.addVertex(SINK);

    std::vector<int> subNode(submissions.size());
    std::vector<int> revNode(sortedReviewers.size());
    std::vector<MatchEdge> matchEdges;

    // Arestas source -> nós de submissão (capacidade = MinReviewsPerSubmission).
    for (size_t i = 0; i < submissions.size(); i++) {
        subNode[i] = submissions[i].id + 1000;
        g.addVertex(subNode[i]);
        g.addEdge(SOURCE, subNode[i], params.MinReviewsPerSubmission);
    }

    // Arestas nós de revisor -> sink (capacidade = MaxReviewsPerReviewer).
    for (size_t i = 0; i < sortedReviewers.size(); i++) {
        revNode[i] = sortedReviewers[i].id + 2000;
        g.addVertex(revNode[i]);
        g.addEdge(revNode[i], SINK, params.MaxReviewsPerReviewer);
    }

    // Arestas submissão -> revisor (capacidade 1) para pares de domínios compatíveis.
    for (size_t s = 0; s < submissions.size(); s++) {
        for (size_t r = 0; r < sortedReviewers.size(); r++) {
            int mDomain = match(submissions[s], sortedReviewers[r], control.generateAssignments);
            if (mDomain == 0) continue;

            g.addEdge(subNode[s], revNode[r], 1);

            // Guardar ponteiro para a aresta recém-criada para reconstrução posterior.
            Vertex<int>* vs = g.findVertex(subNode[s]);
            Edge<int>* e = nullptr;
            for (auto edge : vs->getAdj()) {
                if (edge->getDest()->getInfo() == revNode[r] && edge->getWeight() == 1)
                    e = edge;
            }
            if (!e) continue;

            matchEdges.push_back({(int)s, (int)r, sortedReviewers[r].id, mDomain, e});
        }
    }

    int flow = MaxFlow::edmondsKarp(g, SOURCE, SINK);
    int maxPossibleFlow = (int)submissions.size() * params.MinReviewsPerSubmission;

    std::ofstream out(outputFile, std::ios::out | std::ios::trunc);
    if (!out.is_open()) { freeGraph(g); return false; }

    if (control.generateAssignments != 0) {
        // Ordenar por ID de submissão, depois por ID de revisor.
        std::sort(matchEdges.begin(), matchEdges.end(),
                  [&](const MatchEdge &a, const MatchEdge &b) {
                      int sa = submissions[a.submissionIndex].id;
                      int sb = submissions[b.submissionIndex].id;
                      if (sa != sb) return sa < sb;
                      return a.reviewerId < b.reviewerId;
                  });

        out << "#SubmissionId,ReviewerId,Match\n";

        std::vector<int> count(submissions.size(), 0);
        int total = 0;

        for (auto &m : matchEdges) {
            if (m.edge->getFlow() == 1) {
                out << submissions[m.submissionIndex].id << ", "
                    << m.reviewerId << ", "
                    << m.matchDomain << "\n";
                count[m.submissionIndex]++;
                total++;
            }
        }

        // Vista dual: ordenada por ID de revisor.
        out << "#ReviewerId,SubmissionId,Match\n";

        std::vector<MatchEdge> reviewerOrder;
        for (auto &m : matchEdges)
            if (m.edge->getFlow() == 1)
                reviewerOrder.push_back(m);

        std::sort(reviewerOrder.begin(), reviewerOrder.end(),
                  [&](const MatchEdge &a, const MatchEdge &b) {
                      if (a.reviewerId != b.reviewerId) return a.reviewerId < b.reviewerId;
                      return submissions[a.submissionIndex].id < submissions[b.submissionIndex].id;
                  });

        for (auto &m : reviewerOrder) {
            out << m.reviewerId << ", "
                << submissions[m.submissionIndex].id << ", "
                << m.matchDomain << "\n";
        }

        out << "#Total: " << total << "\n";

        // Reportar submissões com revisões insuficientes.
        if (flow < maxPossibleFlow) {
            out << "#SubmissionId,Domain,MissingReviews\n";
            for (size_t i = 0; i < submissions.size(); i++) {
                int missing = params.MinReviewsPerSubmission - count[i];
                if (missing > 0) {
                    out << submissions[i].id << ", "
                        << submissions[i].primaryTopic << ", "
                        << missing << "\n";
                }
            }
        }
    }

    // --- Análise de risco ---
    // Para cada combinação de K revisores:
    //   1. A capacidade da aresta revisor->sink é colocada a 0 (simula ausência do revisor).
    //   2. Todos os fluxos são repostos a zero.
    //   3. O Edmonds-Karp é re-executado sobre o grafo modificado.
    //   4. Se o fluxo obtido for inferior ao esperado, a combinação é crítica.
    //   5. A capacidade é restaurada antes da iteração seguinte.
    // As C(N,K) combinações são enumeradas com std::next_permutation num vector selector.
    if (control.riskAnalysis > 0) {
        int K = control.riskAnalysis;
        int N = (int)sortedReviewers.size();
        if (K > N) K = N;

        std::vector<int> criticalReviewers;
        std::vector<std::vector<int>> criticalCombinations;

        // Recolher ponteiros para as arestas revisor->sink de cada revisor.
        std::vector<Edge<int>*> revToSinkEdges(N, nullptr);
        for (int i = 0; i < N; i++) {
            Vertex<int>* vr = g.findVertex(revNode[i]);
            for (auto e : vr->getAdj()) {
                if (e->getDest()->getInfo() == SINK) {
                    revToSinkEdges[i] = e;
                    break;
                }
            }
        }

        // Vector selector booleano: K posições verdadeiras indicam os revisores removidos.
        std::vector<bool> selector(N, false);
        std::fill(selector.end() - K, selector.end(), true);

        do {
            std::vector<int> droppedIds;
            for (int i = 0; i < N; i++) {
                if (selector[i]) {
                    droppedIds.push_back(sortedReviewers[i].id);
                    if (revToSinkEdges[i])
                        revToSinkEdges[i]->setWeight(0);
                }
            }

            resetFlows(g);
            int newFlow = MaxFlow::edmondsKarp(g, SOURCE, SINK);

            if (newFlow < maxPossibleFlow) {
                if (K == 1)
                    criticalReviewers.push_back(droppedIds[0]);
                else
                    criticalCombinations.push_back(droppedIds);
            }

            // Restaurar capacidades para a iteração seguinte.
            for (int i = 0; i < N; i++) {
                if (selector[i] && revToSinkEdges[i])
                    revToSinkEdges[i]->setWeight(params.MaxReviewsPerReviewer);
            }

        } while (std::next_permutation(selector.begin(), selector.end()));

        out << "#Risk Analysis: " << K << "\n";

        if (K == 1) {
            std::sort(criticalReviewers.begin(), criticalReviewers.end());
            for (size_t i = 0; i < criticalReviewers.size(); i++) {
                out << criticalReviewers[i];
                if (i + 1 < criticalReviewers.size()) out << ", ";
            }
            if (!criticalReviewers.empty()) out << "\n";
        } else {
            std::sort(criticalCombinations.begin(), criticalCombinations.end());
            for (const auto &c : criticalCombinations) {
                for (size_t i = 0; i < c.size(); i++) {
                    out << c[i];
                    if (i + 1 < c.size()) out << ", ";
                }
                out << "\n";
            }
        }
    }

    out.close();
    freeGraph(g);
    return true;
}

bool Assignment::generateRiskAnalysis(const std::vector<Submission>& submissions,
                                     const std::vector<Reviewer>& reviewers,
                                     const Parameters& params,
                                     const Control& control,
                                     const std::string& riskFile)
{
    // Ordenar revisores por ID para garantir ordem determinística na rede de fluxo.
    std::vector<Reviewer> sortedReviewers = reviewers;
    std::sort(sortedReviewers.begin(), sortedReviewers.end(),
              [](const Reviewer &a, const Reviewer &b) { return a.id < b.id; });

    Graph<int> g;
    const int SOURCE = -1;
    const int SINK = -2;

    g.addVertex(SOURCE);
    g.addVertex(SINK);

    std::vector<int> subNode(submissions.size());
    std::vector<int> revNode(sortedReviewers.size());
    std::vector<MatchEdge> matchEdges;

    for (size_t i = 0; i < submissions.size(); i++) {
        subNode[i] = submissions[i].id + 1000;
        g.addVertex(subNode[i]);
        g.addEdge(SOURCE, subNode[i], params.MinReviewsPerSubmission);
    }

    for (size_t i = 0; i < sortedReviewers.size(); i++) {
        revNode[i] = sortedReviewers[i].id + 2000;
        g.addVertex(revNode[i]);
        g.addEdge(revNode[i], SINK, params.MaxReviewsPerReviewer);
    }

    for (size_t s = 0; s < submissions.size(); s++) {
        for (size_t r = 0; r < sortedReviewers.size(); r++) {
            int mDomain = match(submissions[s], sortedReviewers[r], control.generateAssignments);
            if (mDomain == 0) continue;

            g.addEdge(subNode[s], revNode[r], 1);

            Vertex<int>* vs = g.findVertex(subNode[s]);
            Edge<int>* e = nullptr;
            for (auto edge : vs->getAdj()) {
                if (edge->getDest()->getInfo() == revNode[r] && edge->getWeight() == 1)
                    e = edge;
            }
            if (!e) continue;

            matchEdges.push_back({(int)s, (int)r, sortedReviewers[r].id, mDomain, e});
        }
    }

    int maxPossibleFlow = (int)submissions.size() * params.MinReviewsPerSubmission;

    // Execução inicial do fluxo máximo (necessária para inicializar o estado do grafo).
    MaxFlow::edmondsKarp(g, SOURCE, SINK);

    std::ofstream out(riskFile, std::ios::out | std::ios::trunc);
    if (!out.is_open()) { freeGraph(g); return false; }

    int K = control.riskAnalysis;
    int N = (int)sortedReviewers.size();
    if (K > N) K = N;

    std::vector<int> criticalReviewers;
    std::vector<std::vector<int>> criticalCombinations;

    // Recolher ponteiros para as arestas revisor->sink de cada revisor.
    std::vector<Edge<int>*> revToSinkEdges(N, nullptr);
    for (int i = 0; i < N; i++) {
        Vertex<int>* vr = g.findVertex(revNode[i]);
        for (auto e : vr->getAdj()) {
            if (e->getDest()->getInfo() == SINK) {
                revToSinkEdges[i] = e;
                break;
            }
        }
    }

    // Vector selector booleano: K posições verdadeiras indicam os revisores removidos.
    std::vector<bool> selector(N, false);
    std::fill(selector.end() - K, selector.end(), true);

    do {
        std::vector<int> droppedIds;

        for (int i = 0; i < N; i++) {
            if (selector[i]) {
                droppedIds.push_back(sortedReviewers[i].id);
                if (revToSinkEdges[i])
                    revToSinkEdges[i]->setWeight(0);
            }
        }

        resetFlows(g);
        int newFlow = MaxFlow::edmondsKarp(g, SOURCE, SINK);

        if (newFlow < maxPossibleFlow) {
            if (K == 1)
                criticalReviewers.push_back(droppedIds[0]);
            else
                criticalCombinations.push_back(droppedIds);
        }

        // Restaurar capacidades para a iteração seguinte.
        for (int i = 0; i < N; i++) {
            if (selector[i] && revToSinkEdges[i])
                revToSinkEdges[i]->setWeight(params.MaxReviewsPerReviewer);
        }

    } while (std::next_permutation(selector.begin(), selector.end()));

    out << "#Risk Analysis: " << K << "\n";

    if (K == 1) {
        std::sort(criticalReviewers.begin(), criticalReviewers.end());
        for (size_t i = 0; i < criticalReviewers.size(); i++) {
            out << criticalReviewers[i];
            if (i + 1 < criticalReviewers.size()) out << ", ";
        }
        if (!criticalReviewers.empty()) out << "\n";
    } else {
        std::sort(criticalCombinations.begin(), criticalCombinations.end());
        for (const auto &c : criticalCombinations) {
            for (size_t i = 0; i < c.size(); i++) {
                out << c[i];
                if (i + 1 < c.size()) out << ", ";
            }
            out << "\n";
        }
    }

    out.close();
    freeGraph(g);
    return true;
}