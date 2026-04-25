#pragma once
#include <string>
#include <vector>

class User {
public:
    int id;
    std::string username;
    std::string password;
    std::string role;       // "student" hoac "teacher"
    std::string fullname;

    User() : id(0), username(""), password(""), role("student"), fullname("") {}
    User(int id, std::string username, std::string password,
         std::string role, std::string fullname)
        : id(id), username(username), password(password),
          role(role), fullname(fullname) {}

    bool checkLogin(const std::string& uname, const std::string& pwd) const {
        return (username == uname && password == pwd);
    }
    void print() const;
};


class Question {
public:
    int id;
    int quizId;
    std::string content;
    std::string optionA, optionB, optionC, optionD;
    char correctAnswer;     // 'A', 'B', 'C', hoac 'D'

    Question() : id(0), quizId(0), correctAnswer('A') {}
    Question(int id, int quizId, std::string content,
             std::string a, std::string b, std::string c, std::string d,
             char correct)
        : id(id), quizId(quizId), content(content),
          optionA(a), optionB(b), optionC(c), optionD(d),
          correctAnswer(correct) {}

    bool isCorrect(char studentAnswer) const {
        return (correctAnswer == studentAnswer);
    }
    void print() const;
};


class Quiz {
public:
    int id;
    std::string title;
    int teacherId;
    int durationMinutes;
    std::string createdDate;    // dinh dang "YYYY-MM-DD"

    Quiz() : id(0), teacherId(0), durationMinutes(30) {}
    Quiz(int id, std::string title, int teacherId,
         int duration, std::string createdDate)
        : id(id), title(title), teacherId(teacherId),
          durationMinutes(duration), createdDate(createdDate) {}

    void print() const;
};


// Nguoi 5 dung struct nay de truyen ket qua vao addResult()
struct QuizResult {
    int studentId;
    int quizId;
    float score;
    int totalQuestions;
    int correctCount;
    std::string submittedAt;    // "YYYY-MM-DD HH:MM"
};