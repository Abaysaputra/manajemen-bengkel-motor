#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <iostream> /
#include <limits>   
#include <sstream>  
#include <iomanip>  

void clearScreen();
void pause();
std::string escape_sql_string(const std::string& s);
std::string format_double_for_sql(double val);
int getValidatedInt(const std::string& prompt);
bool folderExists(const std::string& folder);
bool createFolder(const std::string& folder_name); 

#endif // UTILS_H
