#pragma once
#include "models.h"
#include <vector>
#include <string>

const std::string FILE_USERS     = "data/users.csv";
const std::string FILE_QUIZZES   = "data/quizzes.csv";
const std::string FILE_QUESTIONS = "data/questions.csv";
const std::string FILE_RESULTS   = "data/results.csv";

void initDataFiles();

std::vector<std::string> splitCSV(const std::string& line);
int getNextId(const std::string& filename);

// --- User ---
std::vector<User> loadUsers();
bool saveUsers(const std::vector<User>& users);
bool addUser(const User& newUser);
User findUserByLogin(const std::string& username, const std::string& password);
User findUserById(int userId);

// --- Quiz ---
std::vector<Quiz> loadQuizzes();
bool saveQuizzes(const std::vector<Quiz>& quizzes);
bool addQuiz(const Quiz& newQuiz);
std::vector<Quiz> getQuizzesByTeacher(int teacherId);
Quiz findQuizById(int quizId);

// --- Question ---
std::vector<Question> loadQuestions();
bool saveQuestions(const std::vector<Question>& questions);
bool addQuestion(const Question& newQuestion);
std::vector<Question> getQuestionsByQuiz(int quizId);

// --- Result ---
std::vector<QuizResult> loadResults();
bool saveResults(const std::vector<QuizResult>& results);
bool addResult(const QuizResult& result);
std::vector<QuizResult> getResultsByStudent(int studentId);
