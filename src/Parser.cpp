#include "Reviewer.h"
#include "Submission.h"
#include "Parameters.h"
#include "Control.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

enum ParseState {
    NONE,
    SUBMISSIONS,
    REVIEWERS,
    PARAMETERS,
    CONTROL
};

bool parseCSV(const std::string& filepath, std::vector<Submission>& submissions,
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


        if (line.empty() || line[0] == '#') continue;


        size_t pos = line.find('#');
        if (pos != std::string::npos)
            line = line.substr(0, pos);

        std::stringstream ss(line);
        std::string token;

        switch (state) {


        // SUBMISSIONS
        case SUBMISSIONS: {
            int id, primary, secondary = -1;
            std::string title, authors, email;

            std::getline(ss, token, ',');
            id = std::stoi(token);

            std::getline(ss, title, ',');
            std::getline(ss, authors, ',');
            std::getline(ss, email, ',');

            std::getline(ss, token, ',');
            primary = std::stoi(token);

            if (std::getline(ss, token, ',') && !token.empty())
                secondary = std::stoi(token);

            submissions.emplace_back(id, title, authors, email, primary, secondary);
            break;
        }


        // REVIEWERS
        case REVIEWERS: {
            int id, primary, secondary = -1;
            std::string name, email;

            std::getline(ss, token, ',');
            id = std::stoi(token);

            std::getline(ss, name, ',');
            std::getline(ss, email, ',');

            std::getline(ss, token, ',');
            primary = std::stoi(token);

            if (std::getline(ss, token, ',') && !token.empty())
                secondary = std::stoi(token);

            reviewers.emplace_back(id, name, email, primary, secondary);
            break;
        }


        // PARAMETERS
        case PARAMETERS: {
            std::string key, value;
            std::getline(ss, key, ',');
            std::getline(ss, value, ',');

            if (key == "MinReviewsPerSubmission")
                params.MinReviewsPerSubmission = std::stoi(value);
            else if (key == "MaxReviewsPerReviewer")
                params.MaxReviewsPerReviewer = std::stoi(value);
            else if (key == "PrimaryReviewerExpertise")
                params.primaryReviewerExpertise = std::stoi(value);
            else if (key == "SecondaryReviewerExpertise")
                params.secondaryReviewerExpertise = std::stoi(value);
            else if (key == "PrimarySubmissionDomain")
                params.primarySubmissionDomain = std::stoi(value);
            else if (key == "SecondarySubmissionDomain")
                params.secondarySubmissionDomain = std::stoi(value);

            break;
        }


        // CONTROL
        case CONTROL: {
            std::string key, value;
            std::getline(ss, key, ',');
            std::getline(ss, value, ',');

            if (key == "GenerateAssignments")
                control.generateAssignments = std::stoi(value);
            else if (key == "RiskAnalysis")
                control.riskAnalysis = std::stoi(value);
            else if (key == "OutputFileName") {

                if (value.size() > 2 && value.front() == '"' && value.back() == '"') // se existir aspas remover
                    value = value.substr(1, value.size() - 2);
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


