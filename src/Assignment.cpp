#include "Assignment.h"
#include "MaxFlow.h"
#include <fstream>
#include <iostream>
#include <vector>
bool match(const Submission &s, const Reviewer &r) {
    return (s.primaryTopic == r.primaryExpertise);
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
        Edge<int>* edge;
    };

    std::vector<MatchEdge> matchEdges;

    // Criar arestas SUBMISSION → REVIEWER (ordem correta)
    for (size_t s = 0; s < submissions.size(); s++) {
        for (size_t r = 0; r < sortedReviewers.size(); r++) {

            if (match(submissions[s], sortedReviewers[r])) {

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

                matchEdges.push_back({ (int)s, sortedReviewers[r].id, e });
            }
        }
    }

    // MaxFlow
    int flow = MaxFlow::edmondsKarp(g, source, sink);

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
                << submissions[m.submissionIndex].primaryTopic << "\n";

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
            << submissions[m.submissionIndex].primaryTopic << "\n";
    }

    // Total
    out << "#Total: " << total << "\n";

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
