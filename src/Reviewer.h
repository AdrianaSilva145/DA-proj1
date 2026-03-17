#ifndef REVIEWER_H
#define REVIEWER_H

#include <string>

struct Reviewer {
    int id;
    std::string name;
    std::string email;
    int primaryExpertise;
    int secondaryExpertise; // -1 se nao existir

    Reviewer() = default;

    Reviewer(int id, const std::string &name, const std::string &email,
             int primary, int secondary)
        : id(id), name(name), email(email),
          primaryExpertise(primary), secondaryExpertise(secondary) {}
};

#endif //REVIEWER_H
