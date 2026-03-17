#ifndef CONTROL_H
#define CONTROL_H

#include <string>

struct Control {
    int generateAssignments = 0;
    int riskAnalysis = 0;
    std::string outputFileName = "output.csv"; // verificar valor default
};

#endif //CONTROL_H
