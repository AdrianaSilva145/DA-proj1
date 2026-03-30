#ifndef REVIEWER_H
#define REVIEWER_H

#include <string>

/**
 * @brief Estrutura que representa um revisor científico.
 *
 * Cada revisor tem um identificador único, nome, endereço de correio electrónico
 * e uma ou duas áreas de especialização. A área primária é obrigatória; a área
 * secundária é opcional e representada por -1 quando ausente.
 */
struct Reviewer {
    int id;                    ///< Identificador único do revisor.
    std::string name;          ///< Nome completo do revisor.
    std::string email;         ///< Endereço de correio electrónico do revisor.
    int primaryExpertise;      ///< Área de especialização primária.
    int secondaryExpertise;    ///< Área de especialização secundária (-1 se não existir).

    Reviewer() = default;

    /**
     * @brief Constrói um Reviewer com todos os campos.
     * @param id        Identificador único.
     * @param name      Nome do revisor.
     * @param email     Endereço de correio electrónico do revisor.
     * @param primary   Área de especialização primária.
     * @param secondary Área de especialização secundária (-1 se não existir).
     */
    Reviewer(int id, const std::string &name, const std::string &email,
             int primary, int secondary)
        : id(id), name(name), email(email),
          primaryExpertise(primary), secondaryExpertise(secondary) {}
};

#endif //REVIEWER_H