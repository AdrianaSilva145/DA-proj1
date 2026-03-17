#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

enum ParseState {
    NONE,
    SUBMISSIONS,
    REVIEWERS,
    PARAMETERS,
    CONTROL
};

bool parseCSV(const std::string& filepath) {

    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Error opening file " << filepath << std::endl;
        return false;
    }
    ParseState state = NONE;
    std::string line;

    while (std::getline(file, line)) {

        if (line.find("#Submissions") != std::string::npos) {state = SUBMISSIONS;}
        if (line.find("#Reviewers") != std::string::npos) {state = REVIEWERS;}
        if (line.find("#Parameters") != std::string::npos) { state = PARAMETERS;}
        if (line.find("#Control") != std::string::npos) {state = CONTROL;}

        if (line.empty() || line[0] == '#') continue;

        size_t pos = line.find('#');
        if (pos != std::string::npos) {
            line = line.substr(0, pos);
        }

        std::stringstream ss(line);
        std::string token;

        switch (state) {
            case SUBMISSIONS:
                std::getline(ss, token, ',');
                //TODO: instanciar a classe Submission, ler cada atributo com getlines
                break;
            case REVIEWERS:
                std::getline(ss, token, ',');
                //TODO: instanciar Reviewer, getlines
                break;
            case PARAMETERS:
                std::getline(ss, token, ',');
                //TODO: ler os parametros
                break;
            case CONTROL:
                std::getline(ss, token, ',');
                //TODO: ler GenerateAssignments, RiskAnalysis, etc
                break;
            default:
                break;
        }
    }
    file.close();
    return true;
};
void showMenu() {
    std::cout << "\n == Scientific Conference Organization Tool" << std::endl;
    std::cout << "1. Load Data File" << std::endl;
    std::cout << "2. Generate Assignments (Max-Flow)" << std::endl;
    std::cout << "3. Show Results" << std::endl;
    std::cout << "0. Exit" << std::endl;
    std::cout << "Choose an option: ";
}


