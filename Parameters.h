#ifndef PARAMETERS_H
#define PARAMETERS_H

struct Parameters {
    int minReviewsPerSubmission = 0; // caso nao seja definido
    int maxReviewsPerReviewer = 0;
    int primaryReviewerExpertise = 0;
    int secondaryReviewerExpertise = 0;
    int primarySubmissionDomain = 0;
    int secondarySubmissionDomain = 0;
};

#endif //PARAMETERS_H
