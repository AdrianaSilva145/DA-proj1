#include <iostream>
#include <string>
#include "Parser.h"
#include "Assignment.h"

using namespace std;

void showMenu() {
    cout << "\n===== MAIN MENU =====\n";
    cout << "1. Load input file\n";
    cout << "2. Show submissions\n";
    cout << "3. Show reviewers\n";
    cout << "4. Show parameters\n";
    cout << "5. Run assignment\n";
    cout << "6. Run risk analysis\n";
    cout << "0. Exit\n";
    cout << "Option: ";
}

int main(int argc, char* argv[]) {
    vector<Submission> submissions;
    vector<Reviewer> reviewers;
    Parameters params;
    Control control;

    bool fileLoaded = false;

    if (argc >= 3 && string(argv[1]) == "-b") {
        string inputFile = argv[2];
        string riskOutputFile = (argc >= 4 ? argv[3] : "risk.csv");

        if (!Parser::parseCSV(inputFile, submissions, reviewers, params, control)) {
            cerr << "Error: could not read input file: " << inputFile << "\n";
            return 1;
        }

        string assignOutputFile = control.outputFileName.empty() ? "output.csv" : control.outputFileName;

        if (control.generateAssignments != 0) {
            if (!Assignment::generateAssignment(submissions, reviewers, params, control, assignOutputFile)) {
                cerr << "Error: assignment generation failed.\n";
                return 1;
            }
            cout << "Assignment written to: " << assignOutputFile << "\n";
        }

        if (control.riskAnalysis > 0) {
            if (!Assignment::generateRiskAnalysis(submissions, reviewers, params, control, riskOutputFile)) {
                cerr << "Error: risk analysis failed.\n";
                return 1;
            }
            cout << "Risk analysis written to: " << riskOutputFile << "\n";
        }

        return 0;
    }

    int option = -1;

    while (option != 0) {
        showMenu();

        if (!(cin >> option)) {
            cin.clear();
            cin.ignore(10000, '\n');
            continue;
        }

        switch (option) {
        case 1: {
            string filename;
            cout << "Enter file path: ";
            cin >> filename;

            if (Parser::parseCSV(filename, submissions, reviewers, params, control)) {
                cout << "File loaded successfully.\n";
                cout << "  Submissions: " << submissions.size() << "\n";
                cout << "  Reviewers:   " << reviewers.size() << "\n";
                fileLoaded = true;
            } else {
                cerr << "Error: could not load file.\n";
            }
            break;
        }

        case 2: {
            if (!fileLoaded) { cout << "Please load a file first.\n"; break; }
            cout << "\n--- Submissions ---\n";
            for (const auto &s : submissions)
                cout << "  [" << s.id << "] " << s.title
                     << " | Primary: " << s.primaryTopic
                     << " | Secondary: " << (s.secondaryTopic == -1 ? 0 : s.secondaryTopic) << "\n";
            break;
        }

        case 3: {
            if (!fileLoaded) { cout << "Please load a file first.\n"; break; }
            cout << "\n--- Reviewers ---\n";
            for (const auto &r : reviewers)
                cout << "  [" << r.id << "] " << r.name
                     << " | Primary: " << r.primaryExpertise
                     << " | Secondary: " << (r.secondaryExpertise == -1 ? 0 : r.secondaryExpertise) << "\n";
            break;
        }

        case 4: {
            if (!fileLoaded) { cout << "Please load a file first.\n"; break; }
            cout << "\n--- Parameters ---\n";
            cout << "  MinReviewsPerSubmission: " << params.MinReviewsPerSubmission << "\n";
            cout << "  MaxReviewsPerReviewer:   " << params.MaxReviewsPerReviewer << "\n";
            cout << "  GenerateAssignments:     " << control.generateAssignments << "\n";
            cout << "  RiskAnalysis:            " << control.riskAnalysis << "\n";
            cout << "  OutputFileName:          " << control.outputFileName << "\n";
            break;
        }

        case 5: {
            if (!fileLoaded) { cout << "Please load a file first.\n"; break; }
            string outFile = control.outputFileName.empty() ? "output.csv" : control.outputFileName;
            cout << "Running assignment (mode " << control.generateAssignments << ")...\n";
            if (Assignment::generateAssignment(submissions, reviewers, params, control, outFile)) {
                cout << "Done. Results saved to: " << outFile << "\n";
            } else {
                cerr << "Error during assignment.\n";
            }
            break;
        }

        case 6: {
            if (!fileLoaded) { cout << "Please load a file first.\n"; break; }
            if (control.riskAnalysis == 0) {
                cout << "RiskAnalysis is set to 0 in the input file. Nothing to do.\n";
                break;
            }
            string riskFile = "risk.csv";
            cout << "Running risk analysis (K=" << control.riskAnalysis << ")...\n";
            if (Assignment::generateRiskAnalysis(submissions, reviewers, params, control, riskFile)) {
                cout << "Done. Risk analysis saved to: " << riskFile << "\n";
            } else {
                cerr << "Error during risk analysis.\n";
            }
            break;
        }

        case 0:
            cout << "Exiting...\n";
            break;

        default:
            cout << "Invalid option.\n";
        }
    }

    return 0;
}