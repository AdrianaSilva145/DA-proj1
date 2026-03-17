#ifndef SUBMISSION_H
#define SUBMISSION_H

#include <string>

struct Submission {
    int id;
    std::string title;
    std::string authors;
    std::string email;
    int primaryTopic;
    int secondaryTopic;
};

#endif //SUBMISSION_H
