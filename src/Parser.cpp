#include "Reviewer.h"
#include "Submission.h"
#include "Parameters.h"
#include "Control.h"
#include "Parser.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cctype>
#include <algorithm>

// Estados possiveis do parser durante a leitura do ficheiro CSV.
enum ParseState {
    NONE,
    SUBMISSIONS,
    REVIEWERS,
    PARAMETERS,
    CONTROL
};

// Remove espacos em branco no inicio/fim e aspas de uma string. Complexidade: O(n)
static std::string clean(const std::string &s) {
    std::string out = s;
    while (!out.empty() && std::isspace(out.front()))
        out.erase(0, 1);
    while (!out.empty() && std::isspace(out.back()))
        out.pop_back();
    if (out.size() >= 2 && out.front() == '"' && out.back() == '"')
        out = out.substr(1, out.size() - 2);
    return out;
}

bool Parser::parseCSV(const std::string& filepath, std::vector<Submission>& submissions,
                      std::vector<Reviewer>& reviewers, Parameters& params, Control& control)
{
    submissions.clear();
    reviewers.clear();

    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Erro ao abrir o ficheiro " << filepath << std::endl;
        return false;
    }

    ParseState state = NONE;
    std::string line;

    while (std::getline(file, line)) {

        if (line.find("#Submissions") != std::string::npos) { state = SUBMISSIONS; continue; }
        if (line.find("#Reviewers") != std::string::npos)   { state = REVIEWERS; continue; }
        if (line.find("#Parameters") != std::string::npos)  { state = PARAMETERS; continue; }
        if (line.find("#Control") != std::string::npos)     { state = CONTROL; continue; }

        if (line.empty() || line[0] == '#') continue;

        size_t pos = line.find('#');
        if (pos != std::string::npos)
            line = line.substr(0, pos);

        std::stringstream ss(line);
        std::string token;

        switch (state) {

        case SUBMISSIONS: {
            int id = -1, primary = -1, secondary = -1;
            std::string title, authors, email;

            std::getline(ss, token, ',');
            token = clean(token);
            id = token.empty() ? -1 : std::stoi(token);

            std::getline(ss, title, ',');
            std::getline(ss, authors, ',');
            std::getline(ss, email, ',');

            std::getline(ss, token, ',');
            token = clean(token);
            primary = token.empty() ? -1 : std::stoi(token);

            if (std::getline(ss, token, ',')) {
                token = clean(token);
                secondary = token.empty() ? -1 : std::stoi(token);
            }

            submissions.emplace_back(id, title, authors, email, primary, secondary);
            break;
        }

        case REVIEWERS: {
            int id = -1, primary = -1, secondary = -1;
            std::string name, email;

            std::getline(ss, token, ',');
            token = clean(token);
            id = token.empty() ? -1 : std::stoi(token);

            std::getline(ss, name, ',');
            std::getline(ss, email, ',');

            std::getline(ss, token, ',');
            token = clean(token);
            primary = token.empty() ? -1 : std::stoi(token);

            if (std::getline(ss, token, ',')) {
                token = clean(token);
                secondary = token.empty() ? -1 : std::stoi(token);
            }

            reviewers.emplace_back(id, name, email, primary, secondary);
            break;
        }

        case PARAMETERS: {
            std::string key, value;
            std::getline(ss, key, ',');
            std::getline(ss, value, ',');
            key = clean(key);
            value = clean(value);

            if (key == "MinReviewsPerSubmission")
                params.MinReviewsPerSubmission = value.empty() ? 1 : std::stoi(value);
            else if (key == "MaxReviewsPerReviewer")
                params.MaxReviewsPerReviewer = value.empty() ? 1 : std::stoi(value);
            else if (key == "PrimaryReviewerExpertise")
                params.primaryReviewerExpertise = value.empty() ? -1 : std::stoi(value);
            else if (key == "SecondaryReviewerExpertise")
                params.secondaryReviewerExpertise = value.empty() ? -1 : std::stoi(value);
            else if (key == "PrimarySubmissionDomain")
                params.primarySubmissionDomain = value.empty() ? -1 : std::stoi(value);
            else if (key == "SecondarySubmissionDomain")
                params.secondarySubmissionDomain = value.empty() ? -1 : std::stoi(value);
            break;
        }

        case CONTROL: {
            std::string key, value;
            std::getline(ss, key, ',');
            std::getline(ss, value, ',');
            key = clean(key);
            value = clean(value);

            if (key == "GenerateAssignments")
                control.generateAssignments = value.empty() ? 0 : std::stoi(value);
            else if (key == "RiskAnalysis")
                control.riskAnalysis = value.empty() ? 0 : std::stoi(value);
            else if (key == "OutputFileName") {
                if (!value.empty() && value.front() == '"') value.erase(0,1);
                if (!value.empty() && value.back() == '"') value.pop_back();
                control.outputFileName = value;
            }
            break;
        }

        default:
            break;
        }
    }

    file.close();
    return true;
}