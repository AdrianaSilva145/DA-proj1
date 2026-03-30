#ifndef PARAMETERS_H
#define PARAMETERS_H

/**
 * @brief Estrutura que representa os parâmetros do problema de atribuição de revisões.
 *
 * Lida da secção #Parameters do ficheiro CSV de entrada. Define os limites
 * de revisões por submissão e por revisor, bem como os pesos dos domínios.
 */
struct Parameters {
    int MinReviewsPerSubmission = 0;    ///< Número mínimo de revisões por submissão.
    int MaxReviewsPerReviewer = 0;      ///< Número máximo de revisões por revisor.
    int primaryReviewerExpertise = 0;   ///< Peso da expertise primária do revisor.
    int secondaryReviewerExpertise = 0; ///< Peso da expertise secundária do revisor.
    int primarySubmissionDomain = 0;    ///< Peso do domínio primário da submissão.
    int secondarySubmissionDomain = 0;  ///< Peso do domínio secundário da submissão.
};

#endif //PARAMETERS_H