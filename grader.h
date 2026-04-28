#ifndef GRADER_H
#define GRADER_H

#include <vector>

class GradingSystem {
public:
    static double calculateScore(const std::vector<char>& studentAnswers, const std::vector<char>& correctAnswers) {
        if (correctAnswers.empty()) {
            return 0.0;
        }

        int totalQuestions = correctAnswers.size();
        int correctCount = 0;

        for (size_t i = 0; i < totalQuestions; ++i) {
            char studentAns = (i < studentAnswers.size()) ? studentAnswers[i] : ' ';

            if (studentAns != ' ' && studentAns == correctAnswers[i]) {
                correctCount++;
            }
        }

        double finalScore = (static_cast<double>(correctCount) / totalQuestions) * 10.0;

        return finalScore;
    }
};

#endif