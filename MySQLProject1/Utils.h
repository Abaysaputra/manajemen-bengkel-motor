#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <iostream> // Untuk std::cout, std::cin, std::cerr
#include <limits>   // Untuk std::numeric_limits
#include <sstream>  // Untuk std::stringstream
#include <iomanip>  // Untuk std::fixed, std::setprecision

// Deklarasi fungsi-fungsi bantu
void clearScreen();
void pause();
std::string escape_sql_string(const std::string& s);
std::string format_double_for_sql(double val);
int getValidatedInt(const std::string& prompt);
bool folderExists(const std::string& folder);
bool createFolder(const std::string& folder);

#endif // UTILS_H#pragma once
