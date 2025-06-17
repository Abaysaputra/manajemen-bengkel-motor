#include "Utils.h" // Include header baru
#include <cstdlib> // Untuk system("cls") atau system("clear")
#include <direct.h>    // untuk _mkdir
#include <sys/stat.h>    // untuk _stat

// Implementasi fungsi-fungsi bantu
bool folderExists(const std::string& folder) {
#ifdef _WIN32
    struct _stat info;
    return _stat(folder.c_str(), &info) == 0 && (info.st_mode & _S_IFDIR);
#else
    struct stat info;
    return stat(folder.c_str(), &info) == 0 && S_ISDIR(info.st_mode);
#endif
}

bool createFolder(const std::string& folder_name) {
#ifdef _WIN32
    return _mkdir(folder_name.c_str()) == 0;
#else
    // Default permissions: rwxr-xr-x (0755)
    mode_t permissions = S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH;
    return mkdir(folder_name.c_str(), permissions) == 0;
#endif
}

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void pause() {
    std::cout << "\nTekan Enter untuk melanjutkan...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

std::string escape_sql_string(const std::string& s) {
    std::string escaped_s = s;
    size_t pos = 0;
    while ((pos = escaped_s.find("'", pos)) != std::string::npos) {
        escaped_s.replace(pos, 1, "''");
        pos += 2;
    }
    return escaped_s;
}

std::string format_double_for_sql(double val) {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2) << val;
    return ss.str();
}

int getValidatedInt(const std::string& prompt) {
    int value;
    while (true) {
        std::cout << prompt;
        std::cin >> value;
        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "❌ Input tidak valid! Masukkan angka.\n";
        }
        else {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return value;
        }
    }
}