#include "data_manager.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <sys/stat.h>
#include <direct.h> 
void User::print() const {
    std::cout << "[User] id=" << id
              << " | username=" << username
              << " | role=" << role
              << " | fullname=" << fullname << "\n";
}

void Question::print() const {
    std::cout << "[Question] id=" << id
              << " | quizId=" << quizId
              << " | correct=" << correctAnswer
              << " | content=" << content << "\n";
}

void Quiz::print() const {
    std::cout << "[Quiz] id=" << id
              << " | title=" << title
              << " | teacherId=" << teacherId
              << " | duration=" << durationMinutes << "min\n";
}


// Xu ly dung truong hop: "noi dung co, dau phay"
std::vector<std::string> splitCSV(const std::string& line) {
    std::vector<std::string> result;
    std::string field;
    bool inQuotes = false;

    for (size_t i = 0; i < line.size(); ++i) {
        char c = line[i];
        if (c == '"') {
            inQuotes = !inQuotes;
        } else if (c == ',' && !inQuotes) {
            result.push_back(field);
            field.clear();
        } else {
            field += c;
        }
    }
    result.push_back(field);
    return result;
}

// Boc chuoi trong ngoac kep neu chuoi chua dau phay
static std::string quoteCSV(const std::string& s) {
    if (s.find(',') == std::string::npos && s.find('"') == std::string::npos)
        return s;
    std::string out = "\"";
    for (char c : s) {
        if (c == '"') out += "\"\"";
        else out += c;
    }
    out += "\"";
    return out;
}


void initDataFiles() {
#ifdef _WIN32
    _mkdir("data");
#else
    mkdir("data", 0755);
#endif

    auto createIfNotExists = [](const std::string& path, const std::string& header) {
        std::ifstream check(path);
        if (!check.good()) {
            std::ofstream f(path);
            if (f.is_open()) f << header << "\n";
        }
    };

    createIfNotExists(FILE_USERS,     "id,username,password,role,fullname");
    createIfNotExists(FILE_QUIZZES,   "id,title,teacherId,durationMinutes,createdDate");
    createIfNotExists(FILE_QUESTIONS, "id,quizId,content,optionA,optionB,optionC,optionD,correctAnswer");
    createIfNotExists(FILE_RESULTS,   "studentId,quizId,score,totalQuestions,correctCount,submittedAt");
}


int getNextId(const std::string& filename) {
    std::ifstream f(filename);
    if (!f.is_open()) return 1;
    std::string line;
    std::getline(f, line);  // bo header
    int maxId = 0;
    while (std::getline(f, line)) {
        if (line.empty()) continue;
        auto fields = splitCSV(line);
        if (!fields.empty()) {
            try { int id = std::stoi(fields[0]); if (id > maxId) maxId = id; }
            catch (...) {}
        }
    }
    return maxId + 1;
}


// ---- User ----

std::vector<User> loadUsers() {
    std::vector<User> users;
    std::ifstream f(FILE_USERS);
    if (!f.is_open()) return users;
    std::string line;
    std::getline(f, line);
    while (std::getline(f, line)) {
        if (line.empty()) continue;
        auto col = splitCSV(line);
        if (col.size() < 5) continue;
        users.push_back({std::stoi(col[0]), col[1], col[2], col[3], col[4]});
    }
    return users;
}

bool saveUsers(const std::vector<User>& users) {
    std::ofstream f(FILE_USERS);
    if (!f.is_open()) return false;
    f << "id,username,password,role,fullname\n";
    for (const auto& u : users)
        f << u.id << "," << quoteCSV(u.username) << "," << quoteCSV(u.password)
          << "," << quoteCSV(u.role) << "," << quoteCSV(u.fullname) << "\n";
    return true;
}

bool addUser(const User& newUser) {
    User u = newUser;
    u.id = getNextId(FILE_USERS);
    std::ofstream f(FILE_USERS, std::ios::app);
    if (!f.is_open()) return false;
    f << u.id << "," << quoteCSV(u.username) << "," << quoteCSV(u.password)
      << "," << quoteCSV(u.role) << "," << quoteCSV(u.fullname) << "\n";
    return true;
}

User findUserByLogin(const std::string& username, const std::string& password) {
    for (const auto& u : loadUsers())
        if (u.checkLogin(username, password)) return u;
    User notFound; notFound.id = -1; return notFound;
}

User findUserById(int userId) {
    for (const auto& u : loadUsers())
        if (u.id == userId) return u;
    User notFound; notFound.id = -1; return notFound;
}


// ---- Quiz ----

std::vector<Quiz> loadQuizzes() {
    std::vector<Quiz> quizzes;
    std::ifstream f(FILE_QUIZZES);
    if (!f.is_open()) return quizzes;
    std::string line;
    std::getline(f, line);
    while (std::getline(f, line)) {
        if (line.empty()) continue;
        auto col = splitCSV(line);
        if (col.size() < 5) continue;
        quizzes.push_back({std::stoi(col[0]), col[1], std::stoi(col[2]),
                           std::stoi(col[3]), col[4]});
    }
    return quizzes;
}

bool saveQuizzes(const std::vector<Quiz>& quizzes) {
    std::ofstream f(FILE_QUIZZES);
    if (!f.is_open()) return false;
    f << "id,title,teacherId,durationMinutes,createdDate\n";
    for (const auto& q : quizzes)
        f << q.id << "," << quoteCSV(q.title) << "," << q.teacherId
          << "," << q.durationMinutes << "," << quoteCSV(q.createdDate) << "\n";
    return true;
}

bool addQuiz(const Quiz& newQuiz) {
    Quiz q = newQuiz;
    q.id = getNextId(FILE_QUIZZES);
    std::ofstream f(FILE_QUIZZES, std::ios::app);
    if (!f.is_open()) return false;
    f << q.id << "," << quoteCSV(q.title) << "," << q.teacherId
      << "," << q.durationMinutes << "," << quoteCSV(q.createdDate) << "\n";
    return true;
}

std::vector<Quiz> getQuizzesByTeacher(int teacherId) {
    std::vector<Quiz> result;
    for (const auto& q : loadQuizzes())
        if (q.teacherId == teacherId) result.push_back(q);
    return result;
}

Quiz findQuizById(int quizId) {
    for (const auto& q : loadQuizzes())
        if (q.id == quizId) return q;
    Quiz notFound; notFound.id = -1; return notFound;
}


// ---- Question ----

std::vector<Question> loadQuestions() {
    std::vector<Question> questions;
    std::ifstream f(FILE_QUESTIONS);
    if (!f.is_open()) return questions;
    std::string line;
    std::getline(f, line);
    while (std::getline(f, line)) {
        if (line.empty()) continue;
        auto col = splitCSV(line);
        if (col.size() < 8) continue;
        Question q;
        q.id = std::stoi(col[0]); q.quizId = std::stoi(col[1]);
        q.content = col[2]; q.optionA = col[3]; q.optionB = col[4];
        q.optionC = col[5]; q.optionD = col[6];
        q.correctAnswer = col[7].empty() ? 'A' : col[7][0];
        questions.push_back(q);
    }
    return questions;
}

bool saveQuestions(const std::vector<Question>& questions) {
    std::ofstream f(FILE_QUESTIONS);
    if (!f.is_open()) return false;
    f << "id,quizId,content,optionA,optionB,optionC,optionD,correctAnswer\n";
    for (const auto& q : questions)
        f << q.id << "," << q.quizId << "," << quoteCSV(q.content) << ","
          << quoteCSV(q.optionA) << "," << quoteCSV(q.optionB) << ","
          << quoteCSV(q.optionC) << "," << quoteCSV(q.optionD) << ","
          << q.correctAnswer << "\n";
    return true;
}

bool addQuestion(const Question& newQuestion) {
    Question q = newQuestion;
    q.id = getNextId(FILE_QUESTIONS);
    std::ofstream f(FILE_QUESTIONS, std::ios::app);
    if (!f.is_open()) return false;
    f << q.id << "," << q.quizId << "," << quoteCSV(q.content) << ","
      << quoteCSV(q.optionA) << "," << quoteCSV(q.optionB) << ","
      << quoteCSV(q.optionC) << "," << quoteCSV(q.optionD) << ","
      << q.correctAnswer << "\n";
    return true;
}

std::vector<Question> getQuestionsByQuiz(int quizId) {
    std::vector<Question> result;
    for (const auto& q : loadQuestions())
        if (q.quizId == quizId) result.push_back(q);
    return result;
}


// ---- Result ----

std::vector<QuizResult> loadResults() {
    std::vector<QuizResult> results;
    std::ifstream f(FILE_RESULTS);
    if (!f.is_open()) return results;
    std::string line;
    std::getline(f, line);
    while (std::getline(f, line)) {
        if (line.empty()) continue;
        auto col = splitCSV(line);
        if (col.size() < 6) continue;
        QuizResult r;
        r.studentId = std::stoi(col[0]); r.quizId = std::stoi(col[1]);
        r.score = std::stof(col[2]); r.totalQuestions = std::stoi(col[3]);
        r.correctCount = std::stoi(col[4]); r.submittedAt = col[5];
        results.push_back(r);
    }
    return results;
}

bool saveResults(const std::vector<QuizResult>& results) {
    std::ofstream f(FILE_RESULTS);
    if (!f.is_open()) return false;
    f << "studentId,quizId,score,totalQuestions,correctCount,submittedAt\n";
    for (const auto& r : results)
        f << r.studentId << "," << r.quizId << "," << r.score << ","
          << r.totalQuestions << "," << r.correctCount << ","
          << quoteCSV(r.submittedAt) << "\n";
    return true;
}

bool addResult(const QuizResult& result) {
    std::ofstream f(FILE_RESULTS, std::ios::app);
    if (!f.is_open()) return false;
    f << result.studentId << "," << result.quizId << "," << result.score << ","
      << result.totalQuestions << "," << result.correctCount << ","
      << quoteCSV(result.submittedAt) << "\n";
    return true;
}

std::vector<QuizResult> getResultsByStudent(int studentId) {
    std::vector<QuizResult> result;
    for (const auto& r : loadResults())
        if (r.studentId == studentId) result.push_back(r);
    return result;
}
