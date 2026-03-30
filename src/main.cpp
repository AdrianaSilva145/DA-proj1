#include <iostream>
#include <string>
#include "Parser.h"
#include "Assignment.h"

using namespace std;

// Apresenta o menu principal do programa no modo interativo.
void showMenu() {
    cout << "\n===== MENU PRINCIPAL =====\n";
    cout << "1. Carregar ficheiro de entrada\n";
    cout << "2. Mostrar submissoes\n";
    cout << "3. Mostrar revisores\n";
    cout << "4. Mostrar parametros\n";
    cout << "5. Gerar atribuicao\n";
    cout << "6. Correr analise de risco\n";
    cout << "0. Sair\n";
    cout << "Opcao: ";
}

/**
 * @brief Ponto de entrada principal do programa.
 *
 * Suporta dois modos de execucao:
 *
 * **Modo batch** (linha de comandos):
 * @code
 * ./myProg -b input.csv [risk_output.csv]
 * @endcode
 * - O 2o argumento e o ficheiro CSV de entrada.
 * - O 3o argumento (opcional) e o ficheiro de saida da analise de risco.
 * - O ficheiro de saida da atribuicao e determinado pelo campo OutputFileName do CSV.
 * - Mensagens de erro sao enviadas para stderr.
 *
 * **Modo interativo** (sem argumentos):
 * Apresenta um menu que permite carregar ficheiros, visualizar dados e
 * executar as funcionalidades implementadas.
 *
 * @param argc Numero de argumentos da linha de comandos.
 * @param argv Array de argumentos da linha de comandos.
 * @return 0 se bem-sucedido, 1 em caso de erro.
 */
int main(int argc, char* argv[]) {
    vector<Submission> submissions;
    vector<Reviewer> reviewers;
    Parameters params;
    Control control;

    bool fileLoaded = false;

    // --- Modo Batch ---
    if (argc >= 3 && string(argv[1]) == "-b") {
        string inputFile = argv[2];
        string riskOutputFile = (argc >= 4 ? argv[3] : "risk.csv");

        if (!Parser::parseCSV(inputFile, submissions, reviewers, params, control)) {
            cerr << "Erro: nao foi possivel ler o ficheiro de entrada: " << inputFile << "\n";
            return 1;
        }

        string assignOutputFile = control.outputFileName.empty() ? "output.csv" : control.outputFileName;

        if (control.generateAssignments != 0) {
            if (!Assignment::generateAssignment(submissions, reviewers, params, control, assignOutputFile)) {
                cerr << "Erro: falha na geracao da atribuicao.\n";
                return 1;
            }
            cout << "Atribuicao escrita em: " << assignOutputFile << "\n";
        }

        if (control.riskAnalysis > 0) {
            if (!Assignment::generateRiskAnalysis(submissions, reviewers, params, control, riskOutputFile)) {
                cerr << "Erro: falha na analise de risco.\n";
                return 1;
            }
            cout << "Analise de risco escrita em: " << riskOutputFile << "\n";
        }

        return 0;
    }

    // --- Modo Interativo ---
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
            cout << "Caminho do ficheiro: ";
            cin >> filename;

            if (Parser::parseCSV(filename, submissions, reviewers, params, control)) {
                cout << "Ficheiro carregado com sucesso.\n";
                cout << "  Submissoes: " << submissions.size() << "\n";
                cout << "  Revisores:  " << reviewers.size() << "\n";
                fileLoaded = true;
            } else {
                cerr << "Erro: nao foi possivel carregar o ficheiro.\n";
            }
            break;
        }

        case 2: {
            if (!fileLoaded) { cout << "Carregue um ficheiro primeiro.\n"; break; }
            cout << "\n--- Submissoes ---\n";
            for (const auto &s : submissions)
                cout << "  [" << s.id << "] " << s.title
                     << " | Primario: " << s.primaryTopic
                     << " | Secundario: " << (s.secondaryTopic == -1 ? 0 : s.secondaryTopic) << "\n";
            break;
        }

        case 3: {
            if (!fileLoaded) { cout << "Carregue um ficheiro primeiro.\n"; break; }
            cout << "\n--- Revisores ---\n";
            for (const auto &r : reviewers)
                cout << "  [" << r.id << "] " << r.name
                     << " | Primario: " << r.primaryExpertise
                     << " | Secundario: " << (r.secondaryExpertise == -1 ? 0 : r.secondaryExpertise) << "\n";
            break;
        }

        case 4: {
            if (!fileLoaded) { cout << "Carregue um ficheiro primeiro.\n"; break; }
            cout << "\n--- Parametros ---\n";
            cout << "  MinReviewsPerSubmission: " << params.MinReviewsPerSubmission << "\n";
            cout << "  MaxReviewsPerReviewer:   " << params.MaxReviewsPerReviewer << "\n";
            cout << "  GenerateAssignments:     " << control.generateAssignments << "\n";
            cout << "  RiskAnalysis:            " << control.riskAnalysis << "\n";
            cout << "  OutputFileName:          " << control.outputFileName << "\n";
            break;
        }

        case 5: {
            if (!fileLoaded) { cout << "Carregue um ficheiro primeiro.\n"; break; }
            string outFile = control.outputFileName.empty() ? "output.csv" : control.outputFileName;
            cout << "A gerar atribuicao (modo " << control.generateAssignments << ")...\n";
            if (Assignment::generateAssignment(submissions, reviewers, params, control, outFile)) {
                cout << "Concluido. Resultados guardados em: " << outFile << "\n";
            } else {
                cerr << "Erro durante a geracao da atribuicao.\n";
            }
            break;
        }

        case 6: {
            if (!fileLoaded) { cout << "Carregue um ficheiro primeiro.\n"; break; }
            if (control.riskAnalysis == 0) {
                cout << "RiskAnalysis esta definido a 0 no ficheiro. Nada a fazer.\n";
                break;
            }
            string riskFile = "risk.csv";
            cout << "A correr analise de risco (K=" << control.riskAnalysis << ")...\n";
            if (Assignment::generateRiskAnalysis(submissions, reviewers, params, control, riskFile)) {
                cout << "Concluido. Analise de risco guardada em: " << riskFile << "\n";
            } else {
                cerr << "Erro durante a analise de risco.\n";
            }
            break;
        }

        case 0:
            cout << "A sair...\n";
            break;

        default:
            cout << "Opcao invalida.\n";
        }
    }

    return 0;
}