#include <iostream>
#include <string>

#include "Parser.h"
#include "Assignment.h"

using namespace std;

void showMenu() {
    cout << "\n===== MAIN MENU =====\n";
    cout << "1. Load input file\n";
    cout << "2. Run Max-Flow Assignment\n";
    cout << "3. Show results\n";
    cout << "0. Exit\n";
    cout << "Option: ";
}

int main(int argc, char* argv[]) {

    vector<Submission> submissions;
    vector<Reviewer> reviewers;
    Parameters params;
    Control control;

    bool fileLoaded = false;
    bool assignmentDone = false;

    // BATCH MODE
    if (argc >= 3 && string(argv[1]) == "-b") {

        string inputfile = argv[2];
        string outputfile = (argc == 4 ? argv[3] : "output.csv");

        cout << "Executing batch mode with file: " << inputfile << endl;

        if (!Parser::parseCSV(inputfile, submissions, reviewers, params, control)) {
            cerr << "Error: could not read input file.\n";
            return 1;
        }

        if (control.generateAssignments == 1) {
            Assignment::generateAssignment(submissions, reviewers, params, control, outputfile);
        }

        if (control.riskAnalysis == 1) {
            Assignment::riskAnalysis1(submissions, reviewers, params);
        }

        return 0;
    }

    // INTERACTIVE MODE
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
            cout << "Enter file name: ";
            cin >> filename;

            if (Parser::parseCSV(filename, submissions, reviewers, params, control)) {
                cout << "File loaded successfully.\n";
                fileLoaded = true;
                assignmentDone = false;
            } else {
                cout << "Error loading file.\n";
            }
            break;
        }

        case 2:
            if (!fileLoaded) {
                cout << "Please load a file first.\n";
                break;
            }
            cout << "Running Max-Flow Assignment...\n";
            Assignment::generateAssignment(submissions, reviewers, params, control, control.outputFileName);
            assignmentDone = true;
            break;

        case 3:
            if (!assignmentDone) {
                cout << "No assignment has been generated yet.\n";
                break;
            }
            cout << "Results saved in " << control.outputFileName << "\n";
            break;

        case 0:
            cout << "Exiting program...\n";
            break;

        default:
            cout << "Invalid option.\n";
        }
    }

    return 0;
}