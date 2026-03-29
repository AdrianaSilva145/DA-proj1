#ifndef ASSIGNMENT_H
#define ASSIGNMENT_H

#include <vector>
#include <string>
#include "Parser.h"
#include "MaxFlow.h"

class Assignment {
public:
    static bool generateAssignment(const std::vector<Submission>& submissions,
                                   const std::vector<Reviewer>& reviewers,
                                   const Parameters& params,
                                   const Control& control,
                                   const std::string& outputFile);

    static bool generateRiskAnalysis(const std::vector<Submission>& submissions,
                                     const std::vector<Reviewer>& reviewers,
                                     const Parameters& params,
                                     const Control& control,
                                     const std::string& riskFile);
};

#endif