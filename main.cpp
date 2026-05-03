#include "httplib.h"
#include "data_manager.h"
#include "grader.h"
#include <fstream>
#include <iostream>

using namespace httplib;

// Hàm đọc file HTML
std::string get_html_content(const std::string& filename) {
    std::ifstream file("web/" + filename);
    if (!file.is_open()) return "<h1>404 Not Found</h1>";
    return std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
}

int main() {
    Server svr;
    initDataFiles(); // Khởi tạo các file CSV

    // 1. Tuyến đường cho Giáo viên tạo bài thi
    svr.Get("/teacher", [](const Request&, Response& res) {
        res.set_content(get_html_content("teacher quiz.html"), "text/html");
        });

    // 2. Tuyến đường cho Học sinh làm bài
    svr.Get("/student", [](const Request&, Response& res) {
        res.set_content(get_html_content("student_test.html"), "text/html");
        });

    // 3. Xử lý dữ liệu nộp bài (Người 5 logic)
    svr.Post("/submit", [](const Request& req, Response& res) {
        std::cout << "Du lieu nhan duoc: " << req.body << std::endl;

        // Tại đây bạn sẽ lấy đáp án đúng từ Người 2 (data_manager)[cite: 6]
        // Và đưa vào GradingSystem để tính điểm[cite: 1]

        res.set_content("Nộp bài thành công! Điểm của bạn đã được lưu.", "text/plain; charset=utf-8");
        });

    std::cout << "Server dang chay tai: http://localhost:8080" << std::endl;
    svr.listen("0.0.0.0", 8080);
    return 0;
}