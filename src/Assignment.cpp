#include "Assignment.h"
#include "MaxFlow.h"
#include <fstream>
#include <vector>


int match(const Submission &s, const Reviewer &r, int mode) {
    if (mode==1) {
        if (s.primaryTopic == r.primaryExpertise) return s.primaryTopic;
    }
    else if (mode==2) {
        if (s.primaryTopic == r.primaryExpertise) return s.primaryTopic;
        if (s.secondaryTopic !=0 && s.secondaryTopic == r.primaryExpertise) return s.secondaryTopic;
    }
    else if (mode==3) {
        if (s.primaryTopic == r.primaryExpertise) return s.primaryTopic;
        if (s.secondaryTopic != 0 && s.secondaryTopic == r.primaryExpertise) return s.secondaryTopic;
        if (r.secondaryExpertise != 0 && s.primaryTopic == r.secondaryExpertise) return s.primaryTopic;
        if (s.secondaryTopic != 0 && r.secondaryExpertise !=0 && s.secondaryTopic == r.secondaryExpertise) return s.secondaryTopic;
    }
    return 0;

}


struct MatchEdge {
    int submissionIndex;
    int reviewerIndex;
    Edge<int>* edge;
    MatchEdge(size_t s, int r, Edge<int>* e) : submissionIndex(s), reviewerIndex(r),edge(e) {}
};
bool Assignment::generateAssignment(const std::vector<Submission>& submissions,
                                    const std::vector<Reviewer>& reviewers,
                                    const Parameters& params,
                                    const Control& control,
                                    const std::string& outputFile)
{
    // Ordenar reviewers por ID
    std::vector<Reviewer> sortedReviewers = reviewers;
    std::sort(sortedReviewers.begin(), sortedReviewers.end(),
              [](const Reviewer &a, const Reviewer &b) {
                  return a.id < b.id;
              });

    Graph<int> g;

    int source = -1;
    int sink   = -2;

    g.addVertex(source);
    g.addVertex(sink);

    std::vector<int> subNode(submissions.size());
    std::vector<int> revNode(sortedReviewers.size());

    // Criar nós das submissões
    for (size_t i = 0; i < submissions.size(); i++) {
        subNode[i] = submissions[i].id + 1000;
        g.addVertex(subNode[i]);
        g.addEdge(source, subNode[i], params.MinReviewsPerSubmission);
    }

    // Criar nós dos reviewers
    for (size_t i = 0; i < sortedReviewers.size(); i++) {
        revNode[i] = sortedReviewers[i].id + 2000;
        g.addVertex(revNode[i]);
        g.addEdge(revNode[i], sink, params.MaxReviewsPerReviewer);
    }

    // Estrutura para guardar matches
    struct MatchEdge {
        int submissionIndex;
        int reviewerId;
        int matchDomain;
        Edge<int>* edge;

        MatchEdge(size_t s, int r, int d, Edge<int>* e) : submissionIndex(s), reviewerId(r), matchDomain(d), edge(e) {}
    };

    std::vector<MatchEdge> matchEdges;

    // Criar arestas SUBMISSION → REVIEWER (ordem correta)
    for (size_t s = 0; s < submissions.size(); s++) {
        for (size_t r = 0; r < sortedReviewers.size(); r++) {

            int mDomain = match(submissions[s], sortedReviewers[r], control.generateAssignments);

            if (mDomain>0) {

                g.addEdge(subNode[s], revNode[r], 1);

                // Encontrar a aresta criada
                Vertex<int>* vs = g.getVertex(subNode[s]);
                Edge<int>* e = nullptr;

                for (auto edge : vs->getAdj()) {
                    if (edge->getDest()->getInfo() == revNode[r]) {
                        e = edge;
                        break;
                    }
                }

                if (!e) continue;

                matchEdges.push_back({ s, sortedReviewers[r].id, mDomain, e });
            }
        }
    }

    // MaxFlow
    int flow = MaxFlow::edmondsKarp(g, source, sink);

    // se o generateAssignments for 0, o flow corre na mesma mas não há output gerado
    if (control.generateAssignments == 0 && control.riskAnalysis == 0) {
        return false;
    }

    // Abrir ficheiro
    std::ofstream out(outputFile);
    if (!out.is_open()) return false;

    // ORDENAR matches antes de imprimir
    std::sort(matchEdges.begin(), matchEdges.end(),
              [&](const MatchEdge &a, const MatchEdge &b) {
                  int sa = submissions[a.submissionIndex].id;
                  int sb = submissions[b.submissionIndex].id;
                  if (sa != sb) return sa < sb;
                  return a.reviewerId < b.reviewerId;
              });

    // SUBMISSION → REVIEWER
    out << "#SubmissionId,ReviewerId,Match\n";

    int total = 0;
    std::vector<int> count(submissions.size(), 0);

    for (auto &m : matchEdges) {
        if (m.edge->getFlow() == 1) {
            out << submissions[m.submissionIndex].id << ", "
                << m.reviewerId << ", "
                << m.matchDomain << "\n";       //Corrigi aqui

            count[m.submissionIndex]++;
            total++;
        }
    }

    // REVIEWER → SUBMISSION (ordenado)
    out << "#ReviewerId,SubmissionId,Match\n";

    std::vector<MatchEdge> reviewerOrder;
    for (auto &m : matchEdges)
        if (m.edge->getFlow() == 1)
            reviewerOrder.push_back(m);

    std::sort(reviewerOrder.begin(), reviewerOrder.end(),
              [&](const MatchEdge &a, const MatchEdge &b) {
                  if (a.reviewerId != b.reviewerId)
                      return a.reviewerId < b.reviewerId;
                  return submissions[a.submissionIndex].id < submissions[b.submissionIndex].id;
              });

    for (auto &m : reviewerOrder) {
        out << m.reviewerId << ", "
            << submissions[m.submissionIndex].id << ", "
            << m.matchDomain << "\n";
    }

    // Total
    out << "#Total: " << total << "\n";

    //Parte do RiskAnalysis

    int maxPossibleFlow = submissions.size() * params.MinReviewsPerSubmission;

    if (total < maxPossibleFlow) {
        out << "#SubmissionId,Domain,MissingReviews\n";
        for (size_t i = 0; i< submissions.size(); i++) {int missing = params.MinReviewsPerSubmission - count[i];
            if (missing > 0) {
                    out << submissions[i].id << ", "
                    << submissions[i].primaryTopic << ", "
                    << missing << "\n";
                }
            }
        }
    else if (control.riskAnalysis >= 1) {
        int K = control.riskAnalysis;
        auto resetFlows = [&g] {
            for (auto v : g.getVertexSet()) {
                for (auto e : v->getAdj()) {
                    e->setFlow(0);
                }
            }
        };

        std::vector<std::vector<int>> criticalCombinations;
        int N = sortedReviewers.size();

        if (K>N) K = N;     //apenas prevencao: se pedirem para falhar mais reviewers do que aqueles que existem

        std::vector<bool> selector(N, false);
        std::fill(selector.end() - K, selector.end(), true);    //gerar combinacoes

        do {
            std::vector<int> droppedRev;
            std::vector<int> droppedNodes;

            for (int i = 0; i < N; i++) {
                if (selector[i]) {
                    droppedRev.push_back(sortedReviewers[i].id);
                    droppedNodes.push_back(revNode[i]);
                    g.removeEdge(revNode[i], sink);
                }
            }

            resetFlows();
            int newFlow = MaxFlow::edmondsKarp(g, source, sink);
            if (newFlow < maxPossibleFlow) {        //se o desaparecimento comprometeu o fluxo
                criticalCombinations.push_back(droppedRev);
            }

            for (int rNode : droppedNodes) {
                g.addEdge(rNode,sink, params.MaxReviewsPerReviewer);
            }
        } while (std::next_permutation(selector.begin(), selector.end()));

        out << "#Risk Analysis: " << K << "\n";

        std::sort(criticalCombinations.begin(), criticalCombinations.end());

        if (!criticalCombinations.empty()) {

            if (K==1) {
                for (size_t i = 0; i < criticalCombinations.size(); i++) {
                    out << criticalCombinations[i][0] << (i == criticalCombinations.size() - 1 ? "" : ", ");
                }
                out << "\n";
            }
            else {
                for (const auto &c : criticalCombinations) {
                    for (size_t i = 0; i < c.size(); i++) {
                        out << c[i] << (i == c.size() - 1 ? "" : ", ");
                    }
                    out << "\n";
                }
            }
        }


    }
    out.close();
    return true;
}

std::vector<int> Assignment::riskAnalysis1(const std::vector<Submission> &subs,
                                           const std::vector<Reviewer> &revs,
                                           const Parameters &params)
{
    std::vector<int> riskySubmissions;

    // Exemplo simples: marcar submissões sem reviewers compatíveis
    for (const auto &s : subs) {
        bool hasMatch = false;

        for (const auto &r : revs) {
            if (s.primaryTopic == r.primaryExpertise ||
                s.primaryTopic == r.secondaryExpertise ||
                s.secondaryTopic == r.primaryExpertise ||
                s.secondaryTopic == r.secondaryExpertise) {
                hasMatch = true;
                break;
                }
        }

        if (!hasMatch)
            riskySubmissions.push_back(s.id);
    }

    return riskySubmissions;
}
