#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <vector>
#include "Submission.h"
#include "Reviewer.h"
#include "Parameters.h"
#include "Control.h"

struct Parser {
    static bool parseFile(const std::string &filename, std::vector<Submission> &submissions,
                        std::vector<Reviewer> &reviewers, Parameters &params, Control &control);

};

#endif
