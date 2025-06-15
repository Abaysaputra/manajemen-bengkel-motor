#include "Database.h"
#include <iostream>

Database::Database() {
    driver = nullptr;
    con = nullptr;
    stmt = nullptr;
}

Database::~Database() {
    delete stmt;
    delete con;
}

void Database::connect() {
    try {
        driver = get_driver_instance();
        con = driver->connect("tcp://127.0.0.1:3306", "root", ""); // ganti sesuai
        con->setSchema("bengkel_db");
        stmt = con->createStatement();
    }
    catch (sql::SQLException& e) {
        std::cerr << "Database error: " << e.what() << std::endl;
    }
}

sql::ResultSet* Database::query(const std::string& sql) {
    return stmt->executeQuery(sql);
}

void Database::execute(const std::string& sql) {
    stmt->execute(sql);
}
