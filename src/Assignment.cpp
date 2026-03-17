#include "Assignment.h"
#include "MaxFlow.h"
#include <fstream>
#include <iostream>

bool match(const Submission &s, const Reviewer &r) {
    return (s.primaryTopic == r.primaryExpertise ||
            s.primaryTopic == r.secondaryExpertise ||
            s.secondaryTopic == r.primaryExpertise ||
            s.secondaryTopic == r.secondaryExpertise);
}

void Assignment::generateAssignment(const vector<Submission>& submissions,
                                    const vector<Reviewer>& reviewers,
                                    const Parameters& params,
                                    const Control& control,
                                    const string& outputFile)
{
    // 1. Construir grafo
    Graph<int> g;

    int source = -1;
    int sink   = -2;

    g.addVertex(source);
    g.addVertex(sink);

    matchEdges.clear();

    // Criar nós das submissões
    vector<int> subNode(submissions.size());
    for (int i = 0; i < submissions.size(); i++) {
        subNode[i] = submissions[i].id + 1000;
        g.addVertex(subNode[i]);
        g.addEdge(source, subNode[i], params.MinReviewsPerSubmission);
    }

    // Criar nós dos reviewers
    vector<int> revNode(reviewers.size());
    for (int i = 0; i < reviewers.size(); i++) {
        revNode[i] = reviewers[i].id + 2000;
        g.addVertex(revNode[i]);
        g.addEdge(revNode[i], sink, params.MaxReviewsPerReviewer);
    }

    // Criar arestas SUBMISSION → REVIEWER
    for (int s = 0; s < submissions.size(); s++) {
        for (int r = 0; r < reviewers.size(); r++) {

            if (submissions[s].matches(reviewers[r])) {
                Edge<int>* e = g.addEdge(subNode[s], revNode[r], 1);
                matchEdges.push_back({s, r, e});
            }
        }
    }

    // 2. Correr MaxFlow
    int flow = MaxFlow::edmondsKarp(g, source, sink);

    // 3. Abrir ficheiro
    ofstream out(outputFile);
    if (!out.is_open()) return;

    // 4. Escrever SUBMISSION → REVIEWER
    out << "#SubmissionId,ReviewerId,Match\n";

    int total = 0;
    vector<int> count(submissions.size(), 0);

    for (auto &m : matchEdges) {
        if (m.edge->getFlow() == 1) {
            out << submissions[m.submissionIndex].id << ", "
                << reviewers[m.reviewerIndex].id << ", "
                << submissions[m.submissionIndex].primaryTopic << "\n";

            count[m.submissionIndex]++;
            total++;
        }
    }

    // 5. Escrever REVIEWER → SUBMISSION
    out << "#ReviewerId,SubmissionId,Match\n";

    for (auto &m : matchEdges) {
        if (m.edge->getFlow() == 1) {
            out << reviewers[m.reviewerIndex].id << ", "
                << submissions[m.submissionIndex].id << ", "
                << submissions[m.submissionIndex].primaryTopic << "\n";
        }
    }

    // 6. Total
    out << "#Total: " << total << "\n";

    // 7. Missing Reviews
    int required = submissions.size() * params.MinReviewsPerSubmission;

    if (flow != required) {
        out << "#SubmissionId,Domain,MissingReviews\n";

        for (int s = 0; s < submissions.size(); s++) {
            int missing = params.MinReviewsPerSubmission - count[s];
            if (missing > 0) {
                out << submissions[s].id << ", "
                    << submissions[s].primaryTopic << ", "
                    << missing << "\n";
            }
        }
    }

    // 8. Risk Analysis
    if (control.riskAnalysis == 1) {
        out << "#Risk Analysis: 1\n";

        vector<bool> used(reviewers.size(), false);

        for (auto &m : matchEdges) {
            if (m.edge->getFlow() == 1)
                used[m.reviewerIndex] = true;
        }

        for (int i = 0; i < reviewers.size(); i++) {
            if (!used[i]) {
                out << reviewers[i].id;
                if (i < reviewers.size() - 1) out << ", ";
            }
        }
        out << "\n";
    }

    out.close();
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
