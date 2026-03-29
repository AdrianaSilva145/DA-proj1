#ifndef ASSIGNMENT_H
#define ASSIGNMENT_H

#include <vector>
#include <string>
#include "Submission.h"
#include "Reviewer.h"
#include "Parameters.h"
#include "Control.h"
#include "Graph.h"

struct Assignment {

    /**
     * @brief Generates the review assignment using Max-Flow (Edmonds-Karp).
     *        Also handles Risk Analysis if control.riskAnalysis > 0.
     *        Time Complexity: O(R * V * E^2) where R = riskAnalysis value (combinations),
     *                         V = vertices, E = edges in the flow network.
     * @param subs       List of submissions
     * @param revs       List of reviewers
     * @param params     Problem parameters (min/max reviews)
     * @param ctrl       Control parameters (mode, risk, output file)
     * @param outputFile Output file path
     * @return true if assignment was generated successfully
     */
    static bool generateAssignment(const std::vector<Submission> &subs,
                                   const std::vector<Reviewer> &revs,
                                   const Parameters &params,
                                   const Control &ctrl,
                                   const std::string &outputFile);
};

#endif //ASSIGNMENT_H