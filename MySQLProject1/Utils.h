#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <iostream> // Untuk cout, cin, cerr
#include <limits>   // Untuk numeric_limits
#include <sstream>  // Untuk stringstream
#include <iomanip>  // Untuk fixed, setprecision
using namespace std;
// Deklarasi fungsi-fungsi bantu
void clearScreen();
void pause();
string escape_sql_string(const string& s);
string format_double_for_sql(double val);
int getValidatedInt(const string& prompt);
bool folderExists(const string& folder);
bool createFolder(const string& folder);

#endif // UTILS_H#pragma once
