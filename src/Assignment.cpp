#include "Assignment.h"
#include "MaxFlow.h"
#include <fstream>
#include <vector>
#include <algorithm>

/**
 * @brief Determines the matching domain between a submission and a reviewer.
 *        Time Complexity: O(1)
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
 * @brief Resets all flows to 0. Only touches forward edges (weight > 0)
 *        to avoid accessing deleted/dangling reverse edges.
 *        Time Complexity: O(V + E)
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
 * @brief Generates the review assignment using Max-Flow (Edmonds-Karp).
 *        Also handles Risk Analysis if control.riskAnalysis > 0.
 *        Time Complexity: O(V*E^2) base; O(C(N,K) * V*E^2) with risk analysis.
 */
bool Assignment::generateAssignment(const std::vector<Submission>& submissions,
                                    const std::vector<Reviewer>& reviewers,
                                    const Parameters& params,
                                    const Control& control,
                                    const std::string& outputFile)
{
    std::vector<Reviewer> sortedReviewers = reviewers;
    std::sort(sortedReviewers.begin(), sortedReviewers.end(),
              [](const Reviewer &a, const Reviewer &b) { return a.id < b.id; });

    Graph<int> g;

    const int SOURCE = -1;
    const int SINK   = -2;

    g.addVertex(SOURCE);
    g.addVertex(SINK);

    std::vector<int> subNode(submissions.size());
    std::vector<int> revNode(sortedReviewers.size());

    // Source -> Submission (cap = MinReviewsPerSubmission)
    for (size_t i = 0; i < submissions.size(); i++) {
        subNode[i] = submissions[i].id + 1000;
        g.addVertex(subNode[i]);
        g.addEdge(SOURCE, subNode[i], params.MinReviewsPerSubmission);
    }

    // Reviewer -> Sink (cap = MaxReviewsPerReviewer)
    for (size_t i = 0; i < sortedReviewers.size(); i++) {
        revNode[i] = sortedReviewers[i].id + 2000;
        g.addVertex(revNode[i]);
        g.addEdge(revNode[i], SINK, params.MaxReviewsPerReviewer);
    }

    struct MatchEdge {
        int submissionIndex;
        int reviewerIndex;
        int reviewerId;
        int matchDomain;
        Edge<int>* edge;
        MatchEdge(int si, int ri, int rid, int d, Edge<int>* e)
            : submissionIndex(si), reviewerIndex(ri), reviewerId(rid), matchDomain(d), edge(e) {}
    };

    std::vector<MatchEdge> matchEdges;

    // Submission -> Reviewer (cap = 1 por par compativel)
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

            matchEdges.push_back({ (int)s, (int)r, sortedReviewers[r].id, mDomain, e });
        }
    }

    // Correr Max-Flow
    int flow = MaxFlow::edmondsKarp(g, SOURCE, SINK);
    int maxPossibleFlow = (int)submissions.size() * params.MinReviewsPerSubmission;

    std::ofstream out(outputFile);
    if (!out.is_open()) return false;

    // ASSIGNMENT OUTPUT
    if (control.generateAssignments != 0) {

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

        // Submissoes com reviews insuficientes
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

    // RISK ANALYSIS
    if (control.riskAnalysis >= 1) {
        int K = control.riskAnalysis;
        int N = (int)sortedReviewers.size();
        if (K > N) K = N;

        std::vector<int> criticalReviewers;
        std::vector<std::vector<int>> criticalCombinations;

        // Guardar ponteiros para as arestas reviewer->sink.
        // Usamos setWeight(0) em vez de removeEdge() para evitar use-after-free:
        // removeEdge faz delete na forward edge mas a reverse edge fica dangling
        // no adj do sink, e o resetFlows acederia a memoria ja libertada.
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

            // Restaurar capacidades
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

    out.flush();
    out.close();

    // Libertar memoria do grafo de forma segura:
    // Primeiro apagar todas as arestas de cada vertice sem tentar aceder
    // ao incoming do destino (que pode ja estar deletado se usarmos clear())
    for (auto v : g.getVertexSet()) {
        for (auto e : v->getAdj())
            delete e;
    }
    // Agora apagar os vertices (adj e incoming sao apenas ponteiros, ja deletados)
    for (auto v : g.getVertexSet())
        delete v;

    return true;
}