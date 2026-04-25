// Bien dich: g++ -o test data_test.cpp data_manager.cpp -std=c++17
// Chay:      ./test

#include "data_manager.h"
#include <iostream>

void sep(const std::string& title) {
    std::cout << "\n=== " << title << " ===\n";
}

int main() {
    initDataFiles();

    sep("Them User");
    addUser({0, "giaovien01", "gv123", "teacher", "Nguyen Van An"});
    addUser({0, "hocsinh01",  "hs123", "student", "Tran Thi Bich"});
    addUser({0, "hocsinh02",  "hs456", "student", "Le Van Cuong"});
    for (const auto& u : loadUsers()) u.print();

    sep("Dang nhap");
    User found = findUserByLogin("giaovien01", "gv123");
    std::cout << (found.id != -1 ? "OK: " : "FAIL") << "\n";
    if (found.id != -1) found.print();

    User bad = findUserByLogin("giaovien01", "sai");
    std::cout << "Mat khau sai -> id=" << bad.id << "\n";

    sep("Them Quiz");
    addQuiz({0, "Toan lop 10 - Chuong 1", 1, 45, "2025-06-01"});
    addQuiz({0, "Vat ly - Dao dong co",    1, 30, "2025-06-05"});
    for (const auto& q : loadQuizzes()) q.print();

    sep("Them Question (quizId=1)");
    addQuestion({0, 1, "1 + 1 bang bao nhieu?",              "1","2","3","4", 'B'});
    addQuestion({0, 1, "So nguyen to nho nhat?",             "0","1","2","3", 'C'});
    addQuestion({0, 1, "5 x 5 = ?",                          "20","25","30","35", 'B'});
    addQuestion({0, 1, "Cau hoi co dau phay, test: 1,2,3",   "A","B","C","D", 'A'});
    for (const auto& q : getQuestionsByQuiz(1)) q.print();

    sep("Cham diem");
    auto questions = getQuestionsByQuiz(1);
    std::vector<char> answers = {'B', 'C', 'D', 'A'};  // cau 3 sai
    int correct = 0;
    for (size_t i = 0; i < questions.size(); ++i) {
        bool ok = questions[i].isCorrect(answers[i]);
        if (ok) correct++;
        std::cout << "Cau " << i+1 << ": " << answers[i]
                  << " | Dung: " << questions[i].correctAnswer
                  << " -> " << (ok ? "DUNG" : "SAI") << "\n";
    }
    float score = (float)correct / questions.size() * 10.0f;
    std::cout << "Diem: " << correct << "/" << questions.size() << " = " << score << "/10\n";

    sep("Luu Result");
    QuizResult r;
    r.studentId = 2; r.quizId = 1; r.score = score;
    r.totalQuestions = questions.size(); r.correctCount = correct;
    r.submittedAt = "2025-06-01 09:30";
    addResult(r);
    for (const auto& res : getResultsByStudent(2))
        std::cout << "quizId=" << res.quizId << " | score=" << res.score
                  << " | " << res.correctCount << "/" << res.totalQuestions
                  << " | " << res.submittedAt << "\n";

    return 0;
}