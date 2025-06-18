#pragma once
#pragma once
#include <cppconn/driver.h>
#include <cppconn/connection.h>
#include <cppconn/statement.h>
#include <cppconn/resultset.h>

using namespace std;

class Database {
private:
    sql::Driver* driver;
    sql::Connection* con;
    sql::Statement* stmt;
public:
    Database();
    ~Database();
    void connect();
    sql::ResultSet* query(const string& sql);
    void execute(const string& sql);
};
