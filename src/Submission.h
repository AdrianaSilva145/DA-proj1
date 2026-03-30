#ifndef SUBMISSION_H
#define SUBMISSION_H

#include <string>

/**
 * @brief Estrutura que representa uma submissão de artigo científico.
 *
 * Cada submissão tem um identificador único, título, autor(es), email de contacto
 * e um ou dois tópicos científicos (domínio primário obrigatório, secundário opcional).
 */
struct Submission {
    int id;               ///< Identificador único da submissão.
    std::string title;    ///< Título do artigo.
    std::string authors;  ///< Autor(es) do artigo.
    std::string email;    ///< Email de contacto.
    int primaryTopic;     ///< Domínio/tópico primário da submissão.
    int secondaryTopic;   ///< Domínio/tópico secundário (-1 se não existir).

    Submission() = default;

    /**
     * @brief Constrói uma Submission com todos os campos.
     * @param id       Identificador único.
     * @param title    Título do artigo.
     * @param authors  Autor(es).
     * @param email    Email de contacto.
     * @param primary  Domínio primário.
     * @param secondary Domínio secundário (-1 se não existir).
     */
    Submission(int id, const std::string &title, const std::string &authors,
               const std::string &email, int primary, int secondary)
        : id(id), title(title), authors(authors), email(email),
          primaryTopic(primary), secondaryTopic(secondary) {}
};

#endif //SUBMISSION_H