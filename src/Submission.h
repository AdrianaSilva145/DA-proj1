#ifndef SUBMISSION_H
#define SUBMISSION_H

#include <string>

/**
 * @brief Estrutura que representa uma submissão de artigo científico.
 *
 * Cada submissão tem um identificador único, título, autores, endereço de
 * email e um ou dois tópicos científicos. O domínio primário é
 * obrigatório; o domínio secundário é opcional e representado por -1 quando
 * ausente.
 */
struct Submission {
    int id;               ///< Identificador único da submissão.
    std::string title;    ///< Título do artigo.
    std::string authors;  ///< Autores do artigo.
    std::string email;    ///< Endereço de email de contacto.
    int primaryTopic;     ///< Domínio/tópico primário da submissão.
    int secondaryTopic;   ///< Domínio/tópico secundário (-1 se não existir).

    Submission() = default;

    /**
     * @brief Constrói uma Submission com todos os campos.
     * @param id        Identificador único.
     * @param title     Título do artigo.
     * @param authors   Autores.
     * @param email     Endereço de email de contacto.
     * @param primary   Domínio primário.
     * @param secondary Domínio secundário (-1 se não existir).
     */
    Submission(int id, const std::string &title, const std::string &authors,
               const std::string &email, int primary, int secondary)
        : id(id), title(title), authors(authors), email(email),
          primaryTopic(primary), secondaryTopic(secondary) {}
};

#endif //SUBMISSION_H