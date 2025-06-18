#include "Utils.h" // Include header baru
#include <cstdlib> // Untuk system("cls") atau system("clear")
#include <direct.h>    // untuk _mkdir
#include <sys/stat.h>    // untuk _stat

using namespace std;
// Implementasi fungsi-fungsi bantu
bool folderExists(const string& folder) {
#ifdef _WIN32
    struct _stat info;
    return _stat(folder.c_str(), &info) == 0 && (info.st_mode & _S_IFDIR);
#else
    struct stat info;
    return stat(folder.c_str(), &info) == 0 && S_ISDIR(info.st_mode);
#endif
}

bool createFolder(const string& folder_name) {
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
    cout << "\nTekan Enter untuk melanjutkan...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

string escape_sql_string(const string& s) {
    string escaped_s = s;
    size_t pos = 0;
    while ((pos = escaped_s.find("'", pos)) != string::npos) {
        escaped_s.replace(pos, 1, "''");
        pos += 2;
    }
    return escaped_s;
}

string format_double_for_sql(double val) {
    stringstream ss;
    ss << fixed << setprecision(2) << val;
    return ss.str();
}

int getValidatedInt(const string& prompt) {
    int value;
    while (true) {
        cout << prompt;
        cin >> value;
        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "❌ Input tidak valid! Masukkan angka.\n";
        }
        else {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return value;
        }
    }
}