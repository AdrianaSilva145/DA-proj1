#ifndef REVIEWER_H
#define REVIEWER_H

#include <string>

/**
 * @brief Estrutura que representa um revisor científico.
 *
 * Cada revisor tem um identificador único, nome, email e uma ou duas áreas
 * de expertise (primária obrigatória, secundária opcional).
 */
struct Reviewer {
    int id;                    ///< Identificador único do revisor.
    std::string name;          ///< Nome completo do revisor.
    std::string email;         ///< Email do revisor.
    int primaryExpertise;      ///< Área de expertise primária.
    int secondaryExpertise;    ///< Área de expertise secundária (-1 se não existir).

    Reviewer() = default;

    /**
     * @brief Constrói um Reviewer com todos os campos.
     * @param id       Identificador único.
     * @param name     Nome do revisor.
     * @param email    Email do revisor.
     * @param primary  Expertise primária.
     * @param secondary Expertise secundária (-1 se não existir).
     */
    Reviewer(int id, const std::string &name, const std::string &email,
             int primary, int secondary)
        : id(id), name(name), email(email),
          primaryExpertise(primary), secondaryExpertise(secondary) {}
};

#endif //REVIEWER_H