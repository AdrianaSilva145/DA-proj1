#ifndef SUBMISSION_H
#define SUBMISSION_H

#include <string>

struct Submission {
    int id;
    std::string title;
    std::string authors;
    std::string email;
    int primaryTopic;
    int secondaryTopic; // nao esquecer que pode ser opcional (-1 se nao existir)

    Submission() = default;

    Submission(int id, const std::string &title, const std::string &authors,
               const std::string &email, int primary, int secondary)
        : id(id), title(title), authors(authors), email(email),
          primaryTopic(primary), secondaryTopic(secondary) {}
};

#endif //SUBMISSION_H
