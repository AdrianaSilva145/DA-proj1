#ifndef CONTROL_H
#define CONTROL_H

#include <string>

/**
 * @brief Estrutura que representa as configurações de controlo do programa.
 *
 * Lida da secção #Control do ficheiro CSV de entrada. Determina o comportamento
 * do programa relativamente à geração de atribuições e análise de risco.
 */
struct Control {
    int generateAssignments = 0;          ///< Modo de geração de atribuições (0=sem output, 1=só primários, 2=secundários submissão, 3=todos).
    int riskAnalysis = 0;                 ///< Nível de análise de risco K (0=nenhuma, 1=um revisor, K>1=K revisores).
    std::string outputFileName = "output.csv"; ///< Nome do ficheiro de saída da atribuição.
};

#endif //CONTROL_H