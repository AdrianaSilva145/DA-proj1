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

    static bool generateAssignment(const std::vector<Submission> &subs, const std::vector<Reviewer> &revs, const Parameters &params,
                                 const Control &ctrl, const std::string &outputFile);

    static std::vector<int> riskAnalysis1(const std::vector<Submission> &subs,
                                          const std::vector<Reviewer> &revs,
                                          const Parameters &params);
};

#endif //ASSIGNMENT_H
