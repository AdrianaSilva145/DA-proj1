#ifndef REVIEWER_H
#define REVIEWER_H

#include <string>

struct Reviewer {
    int id;
    std::string name;
    std::string email;
    int primaryExpertise;
    int secondaryExpertise;
};

#endif //REVIEWER_H
