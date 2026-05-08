#include "data_manager.h"
#include <mysql.h>
#include <iostream>

using namespace std;

MYSQL* conn;

bool initDatabase() {
    conn = mysql_init(0);

    conn = mysql_real_connect(conn, "localhost", "root", "H24vanban$", "quiz_system", 3306, NULL, 0);

    if (conn) {
        cout << "Connected to MySQL successfully!" << endl;
        return true;
    }
    else {
        cout << "MySQL Connection Failed: " << mysql_error(conn) << endl;
        return false;
    }
}

void closeDatabase() {
    if (conn) {
        mysql_close(conn);
    }
}

// ---- User ----

bool addUser(const User& newUser) {
    string query = "INSERT INTO users (username, password, role, fullname) VALUES ('" +
        newUser.username + "', '" + newUser.password + "', '" +
        newUser.role + "', '" + newUser.fullname + "')";

    if (mysql_query(conn, query.c_str())) {
        cout << "Error adding user: " << mysql_error(conn) << endl;
        return false;
    }
    return true;
}

User findUserByLogin(const string& username, const string& password) {
    User u;
    u.id = -1; // Default to -1 (not found)

    string query = "SELECT id, username, password, role, fullname FROM users WHERE username = '" +
        username + "' AND password = '" + password + "'";

    if (mysql_query(conn, query.c_str())) return u;

    MYSQL_RES* res = mysql_store_result(conn);
    if (res) {
        MYSQL_ROW row = mysql_fetch_row(res);
        if (row) {
            u.id = stoi(row[0]);
            u.username = row[1];
            u.password = row[2];
            u.role = row[3];
            u.fullname = row[4];
        }
        mysql_free_result(res);
    }
    return u;
}

User findUserById(int userId) {
    User u;
    u.id = -1;
    string query = "SELECT id, username, password, role, fullname FROM users WHERE id = " + to_string(userId);

    if (mysql_query(conn, query.c_str())) return u;

    MYSQL_RES* res = mysql_store_result(conn);
    if (res) {
        MYSQL_ROW row = mysql_fetch_row(res);
        if (row) {
            u.id = stoi(row[0]);
            u.username = row[1];
            u.password = row[2];
            u.role = row[3];
            u.fullname = row[4];
        }
        mysql_free_result(res);
    }
    return u;
}

// ---- Quiz ----

int addQuiz(const Quiz& newQuiz) {

    string query = "INSERT INTO quizzes (title, teacher_id, duration_minutes, createdDate) VALUES ('" +
        newQuiz.title + "', " +
        to_string(newQuiz.teacherId) + ", " +
        to_string(newQuiz.durationMinutes) + ", CURDATE())";

    if (mysql_query(conn, query.c_str())) {
        cout << "Error adding quiz: " << mysql_error(conn) << endl;
        return -1;
    }

    int generated_id = (int)mysql_insert_id(conn);
    return generated_id;
}

vector<Quiz> getQuizzesByTeacher(int teacherId) {
    vector<Quiz> quizzes;
    string query = "SELECT id, title, teacher_id, duration_minutes, createdDate FROM quizzes WHERE teacher_id = " + to_string(teacherId);

    if (mysql_query(conn, query.c_str())) return quizzes;

    MYSQL_RES* res = mysql_store_result(conn);
    if (res) {
        MYSQL_ROW row;
        while ((row = mysql_fetch_row(res))) {
            Quiz q;
            q.id = stoi(row[0]);
            q.title = row[1];
            q.teacherId = stoi(row[2]);
            q.durationMinutes = stoi(row[3]);
            q.createdDate = row[4] ? row[4] : "";
            quizzes.push_back(q);
        }
        mysql_free_result(res);
    }
    return quizzes;
}

Quiz findQuizById(int quizId) {
    Quiz q;
    q.id = -1;
    string query = "SELECT id, title, teacher_id, duration_minutes, createdDate FROM quizzes WHERE id = " + to_string(quizId);

    if (mysql_query(conn, query.c_str())) return q;

    MYSQL_RES* res = mysql_store_result(conn);
    if (res) {
        MYSQL_ROW row = mysql_fetch_row(res);
        if (row) {
            q.id = stoi(row[0]);
            q.title = row[1];
            q.teacherId = stoi(row[2]);
            q.durationMinutes = stoi(row[3]);
            q.createdDate = row[4] ? row[4] : "";
        }
        mysql_free_result(res);
    }
    return q;
}

// ---- Question ----

bool addQuestion(const Question& q) {
    string query = "INSERT INTO questions (quiz_id,type,content, option_a, option_b, option_c, option_d, correct_answer) VALUES (" +
        to_string(q.quizId) + ", '" + 
        q.type + "','" + q.content + "', '" +
        q.optionA + "', '" + q.optionB + "', '" +
        q.optionC + "', '" + q.optionD + "', '" +
        string (1,q.correctAnswer) + "')";

    if (mysql_query(conn, query.c_str())) {
        cout << "Error adding question: " << mysql_error(conn) << endl;
        return false;
    }
    return true;
}

vector<Question> getQuestionsByQuiz(int quizId) {
    vector<Question> questions;
    string query = "SELECT id, quiz_id, content, option_a, option_b, option_c, option_d, correct_answer FROM questions WHERE quiz_id = " + to_string(quizId);

    if (mysql_query(conn, query.c_str())) return questions;

    MYSQL_RES* res = mysql_store_result(conn);
    if (res) {
        MYSQL_ROW row;
        while ((row = mysql_fetch_row(res))) {
            Question q;
            q.id = stoi(row[0]);
            q.quizId = stoi(row[1]);
            q.content = row[2];
            q.optionA = row[3];
            q.optionB = row[4];
            q.optionC = row[5];
            q.optionD = row[6];
            q.correctAnswer = row[7][0]; // Extract first char
            questions.push_back(q);
        }
        mysql_free_result(res);
    }
    return questions;
}

// ---- Result ----

bool addResult(const QuizResult& r) {
    string query = "INSERT INTO results (student_id, quiz_id, score, total_questions, correct_count, submitted_at) VALUES (" +
        to_string(r.studentId) + ", " + to_string(r.quizId) + ", " +
        to_string(r.score) + ", " + to_string(r.totalQuestions) + ", " +
        to_string(r.correctCount) + ", '" + r.submittedAt + "')";

    if (mysql_query(conn, query.c_str())) {
        cout << "Error adding result: " << mysql_error(conn) << endl;
        return false;
    }
    return true;
}

vector<QuizResult> getResultsByStudent(int studentId) {
    vector<QuizResult> results;
    string query = "SELECT student_id, quiz_id, score, total_questions, correct_count, submitted_at FROM results WHERE student_id = " + to_string(studentId);

    if (mysql_query(conn, query.c_str())) return results;

    MYSQL_RES* res = mysql_store_result(conn);
    if (res) {
        MYSQL_ROW row;
        while ((row = mysql_fetch_row(res))) {
            QuizResult r;
            r.studentId = stoi(row[0]);
            r.quizId = stoi(row[1]);
            r.score = stof(row[2]);
            r.totalQuestions = stoi(row[3]);
            r.correctCount = stoi(row[4]);
            r.submittedAt = row[5] ? row[5] : "";
            results.push_back(r);
        }
        mysql_free_result(res);
    }
    return results;
}
vector<ClassInfo> getClassesForStudent(int studentId) {
    vector<ClassInfo> classes;
    // Join the classes table with the enrollments table to find this specific student's classes
    string query = "SELECT c.id, c.name FROM classes c JOIN enrollments e ON c.id = e.class_id WHERE e.student_id = " + to_string(studentId);

    if (mysql_query(conn, query.c_str())) return classes;

    MYSQL_RES* res = mysql_store_result(conn);
    if (res) {
        MYSQL_ROW row;
        while ((row = mysql_fetch_row(res))) {
            ClassInfo c;
            c.id = stoi(row[0]);
            c.name = row[1];
            classes.push_back(c);
        }
        mysql_free_result(res);
    }
    return classes;
}
// --- Class Management Implementation ---

bool createClass(const string& className, const string& passcode) {

    string query =
        "INSERT INTO classes (name, passcode) VALUES ('" + className + "', '" + passcode + "')";

    if (mysql_query(conn, query.c_str())) {
        cout << "MySQL Error: " << mysql_error(conn) << endl;
        return false;
    }

    return true;
}

string inviteStudent(int classId, const string& username) {
    string query = "SELECT id FROM users WHERE username = '" + username + "' AND role = 'student'";
    if (mysql_query(conn, query.c_str()) != 0) return "Lỗi CSDL.";

    MYSQL_RES* result = mysql_store_result(conn);
    if (!result) return "Không tìm thấy học sinh này.";

    MYSQL_ROW row = mysql_fetch_row(result);
    if (!row) {
        mysql_free_result(result);
        return "Tài khoản không tồn tại";
    }

    string studentId = row[0];
    mysql_free_result(result);

    string insert_query = "INSERT IGNORE INTO enrollments (student_id, class_id) VALUES (" + studentId + ", " + to_string(classId) + ")";
    mysql_query(conn, insert_query.c_str());

    string msg = "Giáo viên đã thêm bạn vào lớp có ID: " + to_string(classId);
    string notif_query = "INSERT INTO notifications (student_id, message) VALUES (" + studentId + ", '" + msg + "')";
    mysql_query(conn, notif_query.c_str());

    return "Thành công!";

}

string joinClass(int studentId, int classId, const string& passcode) {
    string query = "SELECT passcode FROM classes WHERE id = " + to_string(classId);
    if (mysql_query(conn, query.c_str()) != 0) return "L?i h? th?ng.";

    MYSQL_RES* result = mysql_store_result(conn);
    if (!result) return "Lớp học không tồn tại.";

    MYSQL_ROW row = mysql_fetch_row(result);
    if (!row) {
        mysql_free_result(result);
        return "Lớp học không tồn tại.";
    }

    string correctPasscode = row[0] ? row[0] : "";
    mysql_free_result(result);

    if (correctPasscode != passcode) return "Mã tham gia không chính xác!";

    string insert_query = "INSERT IGNORE INTO enrollments (student_id, class_id) VALUES (" + to_string(studentId) + ", " + to_string(classId) + ")";
    mysql_query(conn, insert_query.c_str());

    return "Tham gia lớp thành công!";
}
vector<Notification> getNotificationsForStudent(int studentId) {
    vector<Notification> list;
    string query = "SELECT id, message, created_at FROM notifications WHERE student_id = " + to_string(studentId) + " ORDER BY created_at DESC";
    if (mysql_query(conn, query.c_str()) != 0) return list;

    MYSQL_RES* res = mysql_store_result(conn);
    if (res) {
        MYSQL_ROW row;
        while ((row = mysql_fetch_row(res))) {
            list.push_back({ stoi(row[0]), row[1], row[2] });
        }
        mysql_free_result(res);
    }
    return list;
}
// --- Thêm hàm này vào cuối file data_manager.cpp ---

bool assignQuizToClass(int classId, int quizId) {
    string query =
        "INSERT INTO class_quizzes (class_id, quiz_id) VALUES (" +
        to_string(classId) + ", " +
        to_string(quizId) + ")";

    return mysql_query(conn, query.c_str()) == 0;
}
Quiz findQuizByClassId(int classId) {
    Quiz q;
    q.id = -1; // Default: Not found

    // Query to cross the bridge from Class -> class_quizzes -> Quizzes
    string query = "SELECT q.id, q.title, q.duration_minutes FROM quizzes q "
                   "JOIN class_quizzes cq ON q.id = cq.quiz_id "
                   "WHERE cq.class_id = " + to_string(classId) + 
                   " ORDER BY q.id DESC LIMIT 1"; // Gets the most recently assigned quiz

    if (mysql_query(conn, query.c_str())) {
        cout << "Error finding quiz: " << mysql_error(conn) << endl;
        return q;
    }

    MYSQL_RES* res = mysql_store_result(conn);
    if (res) {
        MYSQL_ROW row = mysql_fetch_row(res);
        if (row) {
            q.id = stoi(row[0]);
            q.title = row[1];
            q.durationMinutes = stoi(row[2]);
        }
        mysql_free_result(res);
    }
    return q;
}
