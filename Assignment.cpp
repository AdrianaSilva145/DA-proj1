#include "Assignment.h"
#include "MaxFlow.h"
#include <fstream>
#include <iostream>

bool match(const Submission &s, const Reviewer &r, const Parameters &p) {
    return (s.primaryTopic == r.primaryExpertise ||
            s.primaryTopic == r.secondaryExpertise ||
            s.secondaryTopic == r.primaryExpertise ||
            s.secondaryTopic == r.secondaryExpertise);
}

bool Assignment::generateAssignment(const std::vector<Submission> &subs,
                                    const std::vector<Reviewer> &revs,
                                    const Parameters &params,
                                    const Control &ctrl,
                                    const std::string &outputFile)
{
    Graph<int> g;

    int SOURCE = -1;
    int SINK   = -2;

    g.addVertex(SOURCE);
    g.addVertex(SINK);

    for (auto &s : subs) g.addVertex(s.id);
    for (auto &r : revs) g.addVertex(r.id);

    // source → submissions
    for (auto &s : subs) {
        auto v1 = g.findVertex(SOURCE);
        auto v2 = g.findVertex(s.id);

        auto e1 = v1->addEdge(v2, params.minReviewsPerSubmission);
        auto e2 = v2->addEdge(v1, 0);

        e1->setReverse(e2);
        e2->setReverse(e1);
        e1->setFlow(0);
        e2->setFlow(0);
    }

    // submissions → reviewers
    for (auto &s : subs) {
        for (auto &r : revs) {
            if (match(s, r, params)) {
                auto v1 = g.findVertex(s.id);
                auto v2 = g.findVertex(r.id);

                auto e1 = v1->addEdge(v2, 1);
                auto e2 = v2->addEdge(v1, 0);

                e1->setReverse(e2);
                e2->setReverse(e1);
                e1->setFlow(0);
                e2->setFlow(0);
            }
        }
    }

    // reviewers → sink
    for (auto &r : revs) {
        auto v1 = g.findVertex(r.id);
        auto v2 = g.findVertex(SINK);

        auto e1 = v1->addEdge(v2, params.maxReviewsPerReviewer);
        auto e2 = v2->addEdge(v1, 0);

        e1->setReverse(e2);
        e2->setReverse(e1);
        e1->setFlow(0);
        e2->setFlow(0);
    }

    int flow = MaxFlow::edmondsKarp(g, SOURCE, SINK);

    int required = subs.size() * params.minReviewsPerSubmission;

    if (flow != required) {
        std::cout << "Assignment impossivel.\n";
        return false;
    }

    // escrever output
    std::ofstream out(outputFile);
    out << "Submission,Reviewer\n";

    for (auto v : g.getVertexSet()) {
        int sid = v->getInfo();
        if (sid < 0) continue;

        for (auto e : v->getAdj()) {
            if (e->getFlow() == 1) {
                out << sid << "," << e->getDest()->getInfo() << "\n";
            }
        }
    }

    out.close();
    std::cout << "Assignment gerado em " << outputFile << "\n";
    return true;
}
