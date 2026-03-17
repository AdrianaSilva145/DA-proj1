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

enum ParseState {
    NONE,
    SUBMISSIONS,
    REVIEWERS,
    PARAMETERS,
    CONTROL
};

// Função auxiliar simples para limpar espaços e aspas
static std::string clean(const std::string &s) {
    std::string out = s;

    // remover espaços no início
    while (!out.empty() && std::isspace(out.front()))
        out.erase(0, 1);

    // remover espaços no fim
    while (!out.empty() && std::isspace(out.back()))
        out.pop_back();

    // remover aspas
    if (out.size() >= 2 && out.front() == '"' && out.back() == '"')
        out = out.substr(1, out.size() - 2);

    return out;
}

bool Parser::parseCSV(const std::string& filepath, std::vector<Submission>& submissions,
                      std::vector<Reviewer>& reviewers, Parameters& params, Control& control)
{
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Error opening file " << filepath << std::endl;
        return false;
    }

    ParseState state = NONE;
    std::string line;

    while (std::getline(file, line)) {

        // Identificar secções
        if (line.find("#Submissions") != std::string::npos) { state = SUBMISSIONS; continue; }
        if (line.find("#Reviewers") != std::string::npos)   { state = REVIEWERS; continue; }
        if (line.find("#Parameters") != std::string::npos)  { state = PARAMETERS; continue; }
        if (line.find("#Control") != std::string::npos)     { state = CONTROL; continue; }

        // Ignorar linhas vazias ou comentários
        if (line.empty() || line[0] == '#') continue;

        // Remover comentários no fim da linha
        size_t pos = line.find('#');
        if (pos != std::string::npos)
            line = line.substr(0, pos);

        std::stringstream ss(line);
        std::string token;

        switch (state) {

        // ============================
        // SUBMISSIONS
        // ============================
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

        // ============================
        // REVIEWERS
        // ============================
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

        // ============================
        // PARAMETERS
        // ============================
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

        // ============================
        // CONTROL
        // ============================
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
            else if (key == "OutputFileName")
                control.outputFileName = value;

            break;
        }

        default:
            break;
        }
    }

    file.close();
    return true;
}
